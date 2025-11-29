#include "actuator.h"
#include "robot.h"
#include "level.h"
#include "view.h"
#include<fstream>
#include<iostream>
#include<vector>
using namespace std;

//辅助函数，解析单行指令并处理
Instruction parse_instruction_line(const string& line) {
    // 解析指令
    string instr_str;
    string arg_str;
    int arg = -1;
    size_t space_pos = line.find(' ');
    if (space_pos != string::npos) {
        instr_str = line.substr(0, space_pos);
        //处理参数部分，包括检查参数数量是否唯一，以及参数是否是整数
        arg_str = line.substr(space_pos + 1);
    } else {
        instr_str = line;
    }
    //将指令转为小写字母串以便识别
    for (char& c : instr_str) {
        c = tolower(c);
    }
    // 将字符串转换为 instruction_type
    instruction_type instr_type;
    bool is_valid = true;
    if (instr_str == "inbox") {
        instr_type = instruction_type::INBOX;
    } else if (instr_str == "outbox") {
        instr_type = instruction_type::OUTBOX;
    } else if (instr_str == "add") {
        instr_type = instruction_type::ADD;
    } else if (instr_str == "sub") {
        instr_type = instruction_type::SUB;
    } else if (instr_str == "copyto") {
        instr_type = instruction_type::COPYTO;
    } else if (instr_str == "copyfrom") {
        instr_type = instruction_type::COPYFROM;
    } else if (instr_str == "jump") {
        instr_type = instruction_type::JUMP;
    } else if (instr_str == "jumpifzero") {
        instr_type = instruction_type::JUMPIFZERO;
    } else {
        is_valid = false; // 非法指令
    }
    Instruction instr(instr_type, arg, is_valid);
    //处理指令合法性
    if (is_valid)   //指令本身合法，继续检查参数合法性
    {
        //处理参数部分
        if (!arg_str.empty()) {
            //检查参数是否唯一,且是否为整数
            arg_str.erase(0, arg_str.find_first_not_of(' ')); //去除前导空格
            arg_str.erase(arg_str.find_last_not_of(' ') + 1); //去除尾随空格
            size_t extra_space_pos = arg_str.find(' ');
            if (extra_space_pos != string::npos) {
                instr.set_error(); //参数不唯一，视为非法指令
            } else {
                try {
                    arg = stoi(arg_str);
                    instr.arg = arg;
                } catch (const invalid_argument&) {
                    instr.set_error(); //参数不是整数，视为非法指令
                }
            }
        }
    }
    //指令参数均合法，进行最终合法性判定
    instr.judge_valid();
    return instr;
}


void Actuator::read_from_cli(int ins_num) {
    robot.program.clear();
    for (int i = 0; i < ins_num; ++i) {
        string line;
        getline(cin, line);
        Instruction instr = parse_instruction_line(line);
        robot.program.push_back(instr);
    }
}

bool Actuator::read_from_file(const string& file_path) {
    robot.program.clear();
    ifstream infile(file_path);
    if (!infile.is_open()) {
        cerr << "无法打开文件: " << file_path << endl;
        return false;
    }
    string line;    
    while (getline(infile, line)) {
        Instruction instr = parse_instruction_line(line);
        robot.program.push_back(instr);
    }
    infile.close();
    return true;
}

void Actuator::reset(int level_id) {
    robot.reset(level_id);
}

RunResult Actuator::run() {
    // 根据当前 robot.level_id 获取关卡信息
    LevelManager lm;
    Level level = lm.get_level(robot.level_id);

    // 每次运行前重置机器人状态（会按关卡重新初始化输入传送带和空地）
    robot.reset(robot.level_id);
    RunResult result;
    show_one_step(robot);
    // 主执行循环
    while (true) {
        // 1. 如果 pc 越界，说明指令执行完成 → 关卡结束
        if (robot.pc < 0 || robot.pc >= (int)robot.program.size()) {
            break;
        }
        Instruction &instr = robot.program[robot.pc];
        int line_no = robot.pc + 1; // 原始指令序号（1-based）

        // 2. 如果指令在解析阶段就被标记为非法，直接报错
        if (!instr.is_valid) {
            result.type = RunResultType::ERROR;
            result.error_index = line_no;
            result.exec_count = robot.exec_count;
            return result;
        }

        // 3. 检查该指令是否在当前关卡允许的指令集中
        if (!is_instruction_allowed(level, instr)) {
            result.type = RunResultType::ERROR;
            result.error_index = line_no;
            result.exec_count = robot.exec_count;
            return result;
        }

        // 4. 执行当前指令
        bool error = false;

        switch (instr.instruction) {
            case instruction_type::INBOX: {
                if (robot.input_box.empty()) {
                    // 执行 inbox 时输入传送带为空 → 关卡结束（正常）
                    // 不视为错误，直接结束循环
                    robot.pc++; // pc 增加与否都不影响结束判断，这里加 1 以保持一致性
                    goto end_execution;
                }
                robot.current_box = robot.input_box.front();
                robot.input_box.pop();
                robot.pc++;
                break;
            }

            case instruction_type::OUTBOX: {
                if (robot.current_box == INT_MIN) {
                    error = true;
                } else {
                    robot.output_box.push_back(robot.current_box);
                    robot.current_box = INT_MIN;
                    robot.pc++;
                }
                break;
            }

            case instruction_type::ADD: {
                // 检查空地下标合法性以及空地中是否有积木
                if (robot.current_box == INT_MIN || !is_valid_empty_space_arg(level, instr)) {
                    error = true;
                } else {
                    int idx = instr.arg;
                    robot.current_box += robot.empty_spaces[idx];
                    robot.pc++;
                }
                break;
            }

            case instruction_type::SUB: {
                if (robot.current_box == INT_MIN || !is_valid_empty_space_arg(level, instr)) {
                    error = true;
                } else {
                    int idx = instr.arg;
                    robot.current_box -= robot.empty_spaces[idx];
                    robot.pc++;
                }
                break;
            }

            case instruction_type::COPYTO: {
                // 需要有当前积木，且空地下标合法
                if (robot.current_box == INT_MIN) {
                    error = true;
                } else {
                    int empty_count = level.get_empty_count();
                    int idx = instr.arg;
                    if (idx < 0 || idx >= empty_count) {
                        error = true;
                    } else {
                        robot.empty_spaces[idx] = robot.current_box;
                        robot.pc++;
                    }
                }
                break;
            }

            case instruction_type::COPYFROM: {
                int empty_count = level.get_empty_count();
                int idx = instr.arg;
                if (idx < 0 || idx >= empty_count || robot.empty_spaces[idx] == INT_MIN) {
                    error = true;
                } else {
                    robot.current_box = robot.empty_spaces[idx];
                    robot.pc++;
                }
                break;
            }

            case instruction_type::JUMP: {
                int target = instr.arg; // 1-based 行号
                if (target < 1 || target > (int)robot.program.size()) {
                    error = true;
                } else {
                    robot.pc = target - 1;
                }
                break;
            }

            case instruction_type::JUMPIFZERO: {
                int target = instr.arg; // 1-based 行号
                if (robot.current_box == INT_MIN) {
                    error = true;
                } else if (robot.current_box == 0) {
                    if (target < 1 || target > (int)robot.program.size()) {
                        error = true;
                    } else {
                        robot.pc = target - 1;
                    }
                } else {
                    robot.pc++;
                }
                break;
            }

            default:
                error = true;
                break;
        }

        if (error) {
            result.type = RunResultType::ERROR;
            result.error_index = line_no;
            result.exec_count = robot.exec_count;
            return result;
        }
        // 成功执行一条指令
        robot.exec_count++;
        show_one_step(robot);
    }

end_execution:
    // 正常结束后，对比输出与目标输出
    {
        const vector<int> &out = robot.output_box;
        const vector<int> target = level.get_target_output();
        if (out.size() == target.size()) {
            bool same = true;
            for (size_t i = 0; i < out.size(); ++i) {
                if (out[i] != target[i]) {
                    same = false;
                    break;
                }
            }
            if (same) {
                result.type = RunResultType::SUCCESS;
                if(robot.level_id + 1 <= lm.get_level_count()) {
                    lm.set_level_unlocked(robot.level_id+1); // 解锁下一关
                    lm.set_local_level_unlocked(robot.level_id+1); // 本地存储解锁状态
                }
            } else {
                result.type = RunResultType::FAIL;
            }
        } else {
            result.type = RunResultType::FAIL;
        }
        result.error_index = -1;
        result.exec_count = robot.exec_count;
    }
    return result;
}

/*TODO：似乎还不够鲁棒，需要检查指令参数的合法性,如是否越界等，空地合法性检查可以调用is_valid_empty_space_arg函数
* 对于jump的两个指令还没有检查
*/
bool Actuator::is_instruction_allowed(const Level& level, const Instruction& instr) const {
    string instr_str = instruction_to_string(instr.instruction);
    const set<string>& valid_instrs = level.get_valid_instructions();
    return valid_instrs.find(instr_str) != valid_instrs.end();
}

string Actuator::instruction_to_string(instruction_type t) const {
    switch (t) {
        case instruction_type::INBOX: return "inbox";
        case instruction_type::OUTBOX: return "outbox";
        case instruction_type::ADD: return "add";
        case instruction_type::SUB: return "sub";
        case instruction_type::COPYTO: return "copyto";
        case instruction_type::COPYFROM: return "copyfrom";
        case instruction_type::JUMP: return "jump";
        case instruction_type::JUMPIFZERO: return "jumpifzero";
        default: return "";
    }
}

bool Actuator::is_valid_empty_space_arg(const Level& level, const Instruction& instr) const {
    if (instr.instruction == instruction_type::ADD ||
        instr.instruction == instruction_type::SUB ||
        instr.instruction == instruction_type::COPYTO ||
        instr.instruction == instruction_type::COPYFROM) {
        int empty_count = level.get_empty_count();
        int empty_space_id = instr.arg;
        return empty_space_id >= 0 && empty_space_id < empty_count && robot.empty_spaces[empty_space_id] != INT_MIN;
    }
    return true; // 对于不需要检查空地参数的指令，直接返回 true
}

void Actuator::print_outbox() const {
    robot.print_outbox();
}