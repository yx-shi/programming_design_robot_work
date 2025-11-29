#pragma once

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
    int arg;                      // 操作数
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