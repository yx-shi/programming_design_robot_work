#pragma once
#include <vector>
#include <string>
#include <set>

using namespace std;

// 前向声明，避免头文件循环包含
class Robot;
class Level;

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

    bool has_arg() const {
        return instruction == instruction_type::ADD ||
               instruction == instruction_type::SUB ||
               instruction == instruction_type::COPYTO ||
               instruction == instruction_type::COPYFROM ||
               instruction == instruction_type::JUMP ||
               instruction == instruction_type::JUMPIFZERO;
    }

    Instruction(instruction_type t, int arg = -1) : instruction(t), arg(arg) {}
    Instruction() : instruction(instruction_type::INBOX), arg(-1) {}
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



/**
 * @brief Actuator：执行器类，在运行过程中处理各种指令，并对Robot状态进行更新
 *
 * 职责：
 * - 真正地保存一个Robot对象
 * - 正确处理所有指令语义（inbox、outbox、add、sub、copyto、copyfrom、jump、jumpifzero）
 * - 处理各种异常情况，并在出错时返回 "Error on instruction X"
 * - 模拟结束后，根据输出与目标输出的比较返回 "Success" 或 "Fail"
 */
class Actuator {
    Robot robot; // 机器人程序

public:
    Actuator() = default;
    Actuator(int level_id) : robot(level_id) {}
    /**
     * @brief 读取用户输入，设置要执行的机器人程序
     * @param ins_num 需要读取的指令数量
     *
     * 使用方式：
     * - 在外部解析好用户输入的文本指令（字符串）并转为 Instruction 序列后，
     *   调用 set_program() 存到 Actuator 中。
     */
    void read_as_program(int ins_num);

    /**
     * @brief 执行当前 program，在指定关卡上运行一局游戏
     *
     * 需要在实现中完成的主要逻辑：
     * 1. 重置 robot 状态，并根据 level 填充 input_box 和 empty_spaces 大小
     * 2. 从 pc = 0 开始循环执行：
     *    - 检查 pc 是否在 [0, program.size()) 范围内，越界则视为“指令执行完”，结束循环
     *    - 取出当前 Instruction，记录其原始指令编号 = pc + 1
     *    - 先检查：
     *        a) 指令是否在当前关卡的 valid_instructions 集合中，否则立刻 ERROR
     *        b) 是否应该有操作数 / 不该有操作数，参数是否合法
     *    - 根据 instruction_type 执行具体语义：
     *    - 每执行一条指令，exec_count++
     * 3. 循环结束（正常结束）后，对比 robot.output_box 和 level.target_output：
     *    - 完全相同 → 返回 SUCCESS
     *    - 不同 → 返回 FAIL
     */
    RunResult run();
    void reset(int level_id); // 重置执行器状态（清空程序等）
private:
    /**
     * @brief 内部工具函数：检查某一指令在当前关卡中是否被允许
     * 实现中：
     * - 需要将 instruction_type 转换成对应的字符串（如 "inbox"、"outbox"...）
     * - 在 level 的 valid_instructions（set<string>）里查找
     */
    bool is_instruction_allowed(const Level& level, const Instruction& instr) const;

    /**
     * @brief 内部工具函数：将 instruction_type 转换成对应的字符串
     *
     * 实现中：
     * - 使用 switch(instruction_type) 返回固定字符串即可
     */
    string instruction_to_string(instruction_type t) const;

    /**
     * @brief 内部工具函数：检查指令参数是否在有效空地范围内
     *
     * @param level 当前关卡（提供空地数量）
     * @param instr 一条指令
     * @return true 如果该指令不需要检查空地（如 INBOX、OUTBOX、JUMP 等）
     *              或者需要检查且参数在 [0, empty_count) 范围内
     * @return false 若需要空地参数但越界
     *
     * 实现中：
     * - 对 ADD、SUB、COPYTO、COPYFROM，需要保证 0 <= instr.arg < level.get_empty_count()
     */
    bool is_valid_empty_space_arg(const Level& level, const Instruction& instr) const;
};