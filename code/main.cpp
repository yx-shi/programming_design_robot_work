#include<iostream>
#include<vector>
#include<string>
#include<queue>
#include<climits>

using namespace std;
//文件实现OJ的小规模测试，复用项目接口

#pragma once
#include<vector>
#include<set>
#include<string>
using namespace std;

//关卡类
class Level{
    int id;           //关卡ID
    bool is_unlocked;   //是否解锁
    vector<int> input;  //输入序列
    vector<int> target_output;  //目标输出序列
    int empty_count;  //空地数量
    set<string> valid_instructions;  //有效指令集合
    string description;  //关卡描述
public:
    void unlock();
    Level(int level_id=0):id(level_id),is_unlocked(false),empty_count(0){}
    void set_input(const vector<int>& in);    //设置输入序列
    void set_target_output(const vector<int>& out);  //设置目标输出序列
    void set_empty_count(int count);  //设置空地数量
    void set_valid_instructions(const set<string>& instructions);  //设置有效指令集合
    void set_description(const string& desc);  //设置关卡描述
    int get_id() const { return id; } //获取关卡ID    
    int get_empty_count() const{return empty_count;}   //获取空地数量
    bool is_unlocked_level() const { return is_unlocked; } //获取解锁状态
    vector<int> get_input() const { return input; }
    vector<int> get_target_output() const { return target_output; }
    set<string> get_valid_instructions() const { return valid_instructions; }
};

class LevelManager{
    static vector<Level> levels; //关卡列表
public:
    LevelManager();
    const Level& get_level(int level_id) const;
    // 返回关卡总数
    int get_level_count() const { return levels.size(); }
    //返回静态关卡数组的引用，便于菜单中遍历
    const vector<Level>& get_all_levels() const { return levels; }
    bool is_level_unlocked(int level_id) const;
    void set_local_level_unlocked(int level_id); //本地存储解锁状态
    void set_level_unlocked(int level_id);
    void load_local_levels(); //从本地存储加载关卡解锁状态
};

void Level::unlock(){
    is_unlocked = true;
}
void Level::set_input(const vector<int>& in){
    input = in;
}
void Level::set_target_output(const vector<int>& out){
    target_output = out;
}
void Level::set_empty_count(int count){
    empty_count = count;
}
void Level::set_valid_instructions(const set<string>& instructions){
    valid_instructions = instructions;
}
void Level::set_description(const string& desc){
    description = desc;
}

vector<Level> LevelManager::levels; // 静态成员初始化

LevelManager::LevelManager(){
    if(levels.empty()){
        // ========== 初始化关卡1 ==========
        Level level1(1); // 关卡ID为1
        // 设置输入序列
        vector<int> input1 = {1, 2};
        level1.set_input(input1);
        // 设置目标输出序列
        vector<int> output1 = {1, 2};
        level1.set_target_output(output1);
        // 设置空地数量
        level1.set_empty_count(0);
        // 设置有效指令集合
        set<string> inst1 = {"inbox", "outbox"};
        level1.set_valid_instructions(inst1);
        // 设置关卡描述
        string desc1 = "让机器人取出输入序列上的每个积木放入输出序列中";
        level1.set_description(desc1);
        // 解锁关卡1（默认第一个关卡解锁）
        level1.unlock();
        // 添加到关卡列表
        levels.push_back(level1);

        // ========== 初始化关卡2 ==========
        Level level2(2); // 关卡ID为2
        // 设置输入序列
        vector<int> input2 = {3, 9, 5, 1, -2, -2, 9, -9};
        level2.set_input(input2);
        // 设置目标输出序列
        vector<int> output2 = {-6, 6, 4, -4, 0, 0, 18, -18};
        level2.set_target_output(output2);
        // 设置空地数量
        level2.set_empty_count(3);
        // 设置有效指令集合
        set<string> inst2 = {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"};
        level2.set_valid_instructions(inst2);
        // 设置关卡描述
        string desc2 = "对于输入序列中的每两个东西，先把第1个减去第2个，并把结果放在输出序列中，然后把第2个减去第1个，再把结果放在输出序列中，重复。";
        level2.set_description(desc2);
        // 关卡2默认未解锁（可根据需求调整）
        //默认解锁
        level2.unlock();
        levels.push_back(level2);

        // ========== 初始化关卡3 ==========
        Level level3(3); // 关卡ID为3
        // 设置输入序列
        vector<int> input3 = {6, 2, 7, 7, -9, 3, -3, -3};
        level3.set_input(input3);
        // 设置目标输出序列
        vector<int> output3 = {7, -3};
        level3.set_target_output(output3);
        // 设置空地数量
        level3.set_empty_count(3);
        // 设置有效指令集合
        set<string> inst3 = {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"};
        level3.set_valid_instructions(inst3);
        // 设置关卡描述
        string desc3 = "从输入序列中依次取2个数字，如果相等则将其中一个输出，否则扔掉。重复这个过程直到输入传送带为空。";
        level3.set_description(desc3);
        // 关卡3默认未解锁（可根据需求调整）
        level3.unlock();
        levels.push_back(level3);
    }
}

// 实现get_level函数：根据ID获取关卡
const Level& LevelManager::get_level(int level_id) const {
    for (const auto& level : levels) {
        if (level.get_id() == level_id) { 
            return level;
        }
    }
    // 若未找到，返回默认构造的Level对象
    return Level(); 
}

bool LevelManager::is_level_unlocked(int level_id) const {
    for (const auto& level : levels) {
        if (level.get_id() == level_id) {
            return level.is_unlocked_level();
        }
    }
    return false; // 未找到该关卡，视为未解锁
}

void LevelManager::set_level_unlocked(int level_id){
    for(auto& level : levels){
        if(level.get_id() == level_id){
            level.unlock();
            return;
        }
    }
}

enum class instruction_type {
    INBOX,
    OUTBOX,
    ADD,
    SUB,
    COPYTO,
    COPYFROM,
    JUMP,
    JUMPIFZERO,
};

struct Instruction {
    instruction_type instruction; // 操作码
    int arg;                      // 操作数（如空地编号 / 跳转目标行号）
    bool is_valid;       //标注指令是否合法
    bool has_arg() const {
        return instruction == instruction_type::ADD ||
               instruction == instruction_type::SUB ||
               instruction == instruction_type::COPYTO ||
               instruction == instruction_type::COPYFROM ||
               instruction == instruction_type::JUMP ||
               instruction == instruction_type::JUMPIFZERO;
    }
    void judge_valid(){
        //如果指令需要参数但没有提供参数，或者指令不需要参数但提供了参数，则视为非法指令    
        if(!has_arg() && arg!=-1){
            is_valid=false;
        }
        else if(has_arg() && arg==-1){
            is_valid=false;
        }
        //如果操作数不是非负整数，也视为非法指令
        else if(has_arg() && arg<0){
            is_valid=false;
        } 
    }
    void set_error(){
        is_valid=false;
    }
    Instruction(instruction_type t, int arg = -1, bool is_valid = true) : instruction(t), arg(arg), is_valid(is_valid) {}
    Instruction() : instruction(instruction_type::INBOX), arg(-1), is_valid(true) {}
};

enum class RunResultType {
    SUCCESS, // 输出与关卡要求完全一致
    FAIL,    // 程序正常结束，但输出不符合要求
    ERROR    // 执行过程中出错（包括非法指令、越界、无当前积木等）
};

// 统一封装一局游戏的执行结果的结构体
struct RunResult {
    RunResultType type; // 结果类型
    int error_index;    // 若 type == ERROR，则为“出错的原始指令序号（从1开始）”；否则为 -1
    long long exec_count; // 总共执行的指令条数（用于统计信息）
    RunResult(RunResultType t = RunResultType::SUCCESS,
              int err = -1,
              long long cnt = 0)
        : type(t), error_index(err), exec_count(cnt) {}
};

//Robot类，其中封装了某一时刻机器人和环境的全部状态。
class Robot{
public:
    int level_id;    //当前关卡ID
    queue<int> input_box;  //输入传送带状态
    vector<int> output_box; //输出传送带状态
    vector<int> empty_spaces; //空地状态，INT_MIN表示无积木
    int pc;              //当前指令在原始指令序列的编号,注意指令是1-based
    int exec_count;      //已执行指令计数
    int current_box;     //当前的积木
    vector<Instruction> program; //当前加载的程序
    Robot():pc(0),exec_count(0),current_box(INT_MIN){}
    Robot(int level_id);   //根据关卡ID初始化机器人状态
    void reset(int level_id); //重置机器人状态
};

Robot::Robot(int level_id){
    this->level_id = level_id;
    reset(level_id);
}

void Robot::reset(int level_id){
    this->level_id = level_id;
    // 清空传送带和空地状态
    while(!input_box.empty()) input_box.pop();
    output_box.clear();
    empty_spaces.clear();
    pc = 0;
    exec_count = 0;
    current_box = INT_MIN;

    // 获取关卡信息
    LevelManager lm;
    Level level = lm.get_level(level_id);

    //根据关卡信息初始化输入传送带
    for(int val : level.get_input()){
        input_box.push(val);
    }
    // 初始化空地状态
    empty_spaces.resize(level.get_empty_count(), INT_MIN);
}

class Actuator {
    Robot robot; // 机器人程序

public:
    Actuator() = default;
    Actuator(int level_id) : robot(level_id) {}
    void read_from_cli(int ins_num);
    bool read_from_file(const string& file_path);
    RunResult run();
    void reset(int level_id); // 重置执行器状态（清空程序等）
    void print_outbox() const;
private:
    bool is_instruction_allowed(const Level& level, const Instruction& instr) const;
    string instruction_to_string(instruction_type t) const;
    bool is_valid_empty_space_arg(const Level& level, const Instruction& instr) const;
};

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
                    // lm.set_local_level_unlocked(robot.level_id+1); // 本地存储解锁状态
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
    for (const int& item : robot.output_box) {
        cout << item << " ";
    }
    cout << endl;
}

int main(){
    int level_id;
    cin>>level_id;
    int ins_num;
    cin>>ins_num;
    Actuator actuator(level_id);
    cin.ignore(); //忽略换行符
    actuator.read_from_cli(ins_num);
    RunResult result = actuator.run();
    if(result.type == RunResultType::SUCCESS){
        cout<<"Success"<<endl;
    }
    else if(result.type == RunResultType::FAIL){
        cout<<"Fail"<<endl;
    }
    else{
        cout<<"Error on instruction "<<result.error_index<<endl;
    }
    return 0;
}