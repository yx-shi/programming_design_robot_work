#include "actuator.h"
#include "robot.h"
#include "level.h"
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
    cin.ignore(); // 清除之前输入留下的换行符
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
    //TODO: 实现机器人程序执行逻辑
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