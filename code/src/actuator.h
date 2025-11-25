#pragma once
#include <vector>
#include <string>
#include <set>

using namespace std;

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

// 统一封装一局游戏的执行结果
struct RunResult {
    RunResultType type; // 结果类型
    int error_index;    // 若 type == ERROR，则为“出错的原始指令序号（从1开始）”；否则为 -1
    long long exec_count; // 总共执行的指令条数（用于统计信息）
    RunResult(RunResultType t = RunResultType::SUCCESS,
              int err = -1,
              long long cnt = 0)
        : type(t), error_index(err), exec_count(cnt) {}
};

// 前向声明，避免头文件循环包含
class Robot;
class Level;

// 状态观察者接口：用于“每执行一步就通知外部打印状态”
struct StateObserver {
    virtual ~StateObserver() = default;

    /**
     * @brief 在执行每一条指令后被调用，用于输出当前状态
     * @param robot 当前机器人的运行时状态（input_box, output_box, empty_spaces, pc, current_box 等）
     * @param current_instruction_index 当前执行的指令号（从 1 开始，对应原始程序行号）
     * @param program 完整的机器人程序指令序列
     * @param level 当前关卡信息（可用于显示输入/目标输出等）
     *
     * 需要在实现类中：
     * - 清晰打印当前游戏状态（建议包含：关卡描述、输入传送带、输出传送带、空地、当前积木、程序、当前指令等）
     * - 可以根据需要清屏 / 加分割线，让命令行界面更清楚
     */
    virtual void on_step(const Robot& robot,
                         int current_instruction_index,
                         const vector<Instruction>& program,
                         const Level& level) = 0;
};

/**
 * @brief Actuator：负责在指定关卡上执行一段机器人程序
 *
 * 职责：
 * - 保存一份“已解析的指令序列”（program）
 * - 驱动 Robot 在某一 Level 上逐条执行这些指令
 * - 正确处理所有指令语义（inbox、outbox、add、sub、copyto、copyfrom、jump、jumpifzero）
 * - 处理各种异常情况，并在出错时返回 "Error on instruction X"
 * - 模拟结束后，根据输出与目标输出的比较返回 "Success" 或 "Fail"
 */
class Actuator {
    vector<Instruction> program; // 机器人程序（解析后的指令序列）

public:
    Actuator() = default;

    /**
     * @brief 设置要执行的机器人程序
     * @param prog 解析好的指令列表（下标 0 表示第 1 条指令）
     *
     * 使用方式：
     * - 在外部解析好用户输入的文本指令（字符串）并转为 Instruction 序列后，
     *   调用 set_program() 存到 Actuator 中。
     */
    void set_program(const vector<Instruction>& prog);

    /**
     * @brief 执行当前 program，在指定关卡上运行一局游戏（无状态观察者）
     *
     * @param level 当前关卡（包含输入序列、目标输出、空地数、允许指令集等）
     * @param robot 机器人运行时状态对象：
     *              - 函数内部应首先调用 robot.reset(level_id) 或类似接口重置状态
     *              - 然后根据 level 初始化 input_box / empty_spaces 等
     *              - 再按 program 从头执行
     *
     * @return RunResult：
     *   - type == SUCCESS：输出与关卡要求完全一致
     *   - type == FAIL：程序执行结束（pc 越界或 inbox 时无输入）但输出不符合关卡要求
     *   - type == ERROR：执行中遇到异常，需要设置 error_index
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
     *        * INBOX: 从 robot.input_box 取一个积木放到 current_box，若没有积木则游戏结束
     *        * OUTBOX: 将 current_box 放到 output_box，若当前无积木则 ERROR
     *        * ADD, SUB: 对 current_box 和指定空地中的积木做运算，各种非法情况按表中规则 ERROR
     *        * COPYTO, COPYFROM: 按规则读/写 empty_spaces，非法情况 ERROR
     *        * JUMP: 将 pc 设置为 X-1（X 为指令参数，且为 1-based 原始行号），若不存在该行则 ERROR
     *        * JUMPIFZERO: 若 current_box 为 0 则跳转，否则 pc 正常 +1；没有当前积木则 ERROR
     *    - 每执行一条指令，exec_count++
     * 3. 循环结束（正常结束）后，对比 robot.output_box 和 level.target_output：
     *    - 完全相同 → 返回 SUCCESS
     *    - 不同 → 返回 FAIL
     */
    RunResult run(Level& level, Robot& robot);

    /**
     * @brief 带状态观察者的执行版本，每执行一步就通知 observer
     *
     * @param level 关卡
     * @param robot 机器人状态
     * @param observer 状态观察者指针：
     *                 - 若为 nullptr，则行为与不带 observer 的 run(level, robot) 类似
     *                 - 若非空，则在每条指令执行完后调用 observer->on_step(...)
     *
     * 需要在实现中：
     * - 将核心执行逻辑封装在一个私有的内部函数（例如 execute_step），
     *   在循环中每步执行后调用 observer->on_step(...)。
     */
    RunResult run(Level& level, Robot& robot, StateObserver* observer);

private:
    /**
     * @brief 内部工具函数：检查某一指令在当前关卡中是否被允许
     *
     * @param level 当前关卡（含 valid_instructions）
     * @param instr 一条指令
     * @return true 如果该指令名在 level 的 valid_instructions 集合中
     * @return false 否则
     *
     * 实现中：
     * - 需要将 instruction_type 转换成对应的字符串（如 "inbox"、"outbox"...）
     * - 在 level 的 valid_instructions（set<string>）里查找
     */
    bool is_instruction_allowed(const Level& level, const Instruction& instr) const;

    /**
     * @brief 内部工具函数：将 instruction_type 转换成对应的字符串
     *
     * @param t 指令类型
     * @return 对应的指令名字符串，例如：
     *         INBOX -> "inbox", OUTBOX -> "outbox", ADD -> "add" ...
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
};