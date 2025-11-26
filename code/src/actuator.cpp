#include "actuator.h"
#include "robot.h"
#include "level.h"
#include<iostream>
#include<vector>
using namespace std;

void Actuator::read_as_program(int ins_num) {
    //TODO: 从输入读取 ins_num 条指令，解析后存入 robot.program
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