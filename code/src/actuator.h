#pragma once
#include "robot.h"
#include "level.h"
#include "instruction.h"
#include <vector>
#include <string>
#include <set>

using namespace std;

enum class RunResultType {
    SUCCESS, 
    FAIL,    
    ERROR    
};

// 统一封装一局游戏的执行结果的结构体
struct RunResult {
    RunResultType type; 
    int error_index;    // 若 type == ERROR，则为“出错的原始指令序号（从1开始）”；否则为 -1
    long long exec_count; // 总共执行的指令条数
    RunResult(RunResultType t = RunResultType::SUCCESS,
              int err = -1,
              long long cnt = 0)
        : type(t), error_index(err), exec_count(cnt) {}
};



//执行器类，内部封装一个robot对象，在执行过程中对其更新
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
    Robot get_robot() const{ return robot; } 
private:
    // 内部工具函数
    bool is_instruction_allowed(const Level& level, const Instruction& instr) const;   //检查指令是否合法
    string instruction_to_string(instruction_type t) const;    //指令类型转字符串
    bool is_valid_empty_space_arg(const Level& level, const Instruction& instr) const;    //检查空地编号是否合法
};