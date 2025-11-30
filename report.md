# 程设大作业报告：机器人编程游戏

## 一、项目工程结构

本项目按照“OJ 测试代码 + 完整游戏代码”分层组织，其中 OJ 使用的精简版代码在 [code/main.cpp](code/main.cpp)，完整游戏框架位于 [code/src](code/src) 目录下。根据题目要求，本报告主要针对 `src` 目录下的完整游戏结构进行说明。

### 1. 顶层结构

- [code/src/game.cpp](code/src/game.cpp)  
  - 整个游戏的入口（`int main()`）与一局游戏的控制逻辑（`game()`）。
  - 负责：
    - 启动游戏、循环游戏（支持多局）。
    - 关卡选择与关卡信息展示的调度。
    - 用户选择指令输入方式（命令行输入 / 文件输入）。
    - 调用执行器 [`Actuator`](code/src/actuator.h) 运行游戏，以及调用视图层展示结果。

- [code/src/actuator.h](code/src/actuator.h)、[code/src/actuator.cpp](code/src/actuator.cpp)  
  - 执行器模块，是“机器人 + 关卡规则 + 程序指令”的核心调度者。
  - 主要职责：
    - 将用户输入的文本指令解析为 [`Instruction`](code/src/instruction.h) 列表（`parse_instruction_line` 辅助函数）。
    - 维护一个 [`Robot`](code/src/robot.h) 对象，逐条执行其 `program` 中的指令（`Actuator::run`）。
    - 进行关卡相关的合法性检查（指令是否允许、空地是否合法、跳转行号是否合法等）。
    - 记录执行结果 [`RunResult`](code/src/actuator.h)（`SUCCESS / FAIL / ERROR`）并配合视图层输出。

- [code/src/robot.h](code/src/robot.h)、[code/src/robot.cpp](code/src/robot.cpp)  
  - 机器人与游戏状态模型。
  - 封装了“某一时刻游戏状态”的全部信息，包括：
    - `level_id`：当前关卡 ID。
    - `input_box` / `output_box`：输入 / 输出传送带状态。
    - `empty_spaces`：空地数组（用 `INT_MIN` 表示“无积木”）。
    - `pc`：当前正在执行的指令下标（0-based，原始行号为 `pc+1`）。
    - `exec_count`：已执行的指令条数。
    - `current_box`：机器人手中当前积木。
    - `program`：解析后的 [`Instruction`](code/src/instruction.h) 序列。
  - 提供：
    - `Robot(int level_id)` 构造函数，根据关卡初始化输入传送带和空地。
    - `void reset(int level_id)`，在每次开始运行前根据关卡信息重置游戏状态。
    - `void print_outbox() const`，在 CLI 模式下打印输出传送带。

- [code/src/level.h](code/src/level.h)、[code/src/level.cpp](code/src/level.cpp)  
  - 关卡配置与管理模块。
  - [`Level`](code/src/level.h) 类：
    - 关卡 ID、标题、解锁状态、输入序列、目标输出、空地数量、可用指令集、描述信息等。
  - [`LevelManager`](code/src/level.h) 类：
    - 维护静态关卡数组 `static vector<Level> levels`。
    - 在构造函数中集中初始化所有基础关卡与创新关卡（参见 [code/src/level.cpp](code/src/level.cpp)）。
    - 提供：
      - `const Level& get_level(int level_id) const` 取关卡。
      - `bool is_level_unlocked(int level_id) const` 查询解锁状态。
      - `void set_level_unlocked(int level_id)` 修改内存中的解锁状态。
      - `void set_local_level_unlocked(int level_id)` / `void load_local_levels()` 与本地文件交互，持久化解锁信息。

- [code/src/instruction.h](code/src/instruction.h)  
  - 指令模型与合法性判定。
  - `enum class instruction_type`：定义 `INBOX / OUTBOX / ADD / SUB / COPYTO / COPYFROM / JUMP / JUMPIFZERO`。
  - `struct Instruction`：
    - `instruction_type instruction; int arg; bool is_valid;`
    - `bool has_arg() const`：指示该指令是否应该有整数参数。
    - `void judge_valid()`：根据“是否需要参数”、“参数是否给出”、“参数是否非负整数”等规则判定合法性。
    - `void set_error()`：在解析阶段或运行阶段标记该指令非法。

- [code/src/view.h](code/src/view.h)  
  - CLI 界面与交互层。
  - 主要接口：
    - `int read_menu_key()`：读取键盘按键（支持方向键）。
    - `int initialize_view(const LevelManager&)`：绘制关卡选择界面，返回选中的关卡 ID。
    - `void show_level_info(const Level&)`：展示关卡说明、输入 / 输出序列、可用指令集等。
    - `void show_one_step(const Robot&)`：在每执行一步后绘制游戏主界面（输入、输出、空地、当前积木、机器人 ASCII 图、指令列表及当前执行行等）。
    - `void show_final_result(const Actuator&, const RunResult&)`：绘制关卡结束后的结果界面，展示成功 / 失败 / 错误信息及统计数据。
    - `void show_read_from_cli()` / `show_read_from_file()`：在选择指令输入方式时给出提示。

- [code/src/conf.h](code/src/conf.h)  
  - 全局配置路径：
    - `const std::string local_level_path="../local_levels/";`
    - `const std::string test_path="../test_input/";`

- 资源文件
  - [code/local_levels/level_unlock_status.txt](code/local_levels/level_unlock_status.txt)：本地关卡解锁状态持久化文件。
  - [code/test_level](code/test_level)：存放关卡测试输入，用于调试和自检。

整体上，工程采用“模型（Robot/Level/Instruction）+ 执行器（Actuator）+ 视图（View）+ 控制器（Game）”的清晰分层，便于后续扩展和 OJ 部署。

---

## 二、项目设计思路

### 1. 总体架构思路

项目整体围绕“模拟 Human Resource Machine 类似的机器人搬运游戏”展开，核心需求可以分解为三层：

1. **数据与规则层（Model）**  
   - 机器人状态：传送带、空地、当前积木、程序计数器等（[Robot](code/src/robot.h)）。
   - 关卡配置：输入序列、目标输出、可用指令集等（[`Level`](code/src/level.h)）。
   - 指令定义：每条机器人指令的语义和合法性约束（[`Instruction`](code/src/instruction.h)）。

2. **执行与规则检查层（Controller / Engine）**  
   - 从用户输入（或文件）中解析指令字符串为 `Instruction` 列表（`parse_instruction_line`）。
   - 执行过程中根据关卡规则检查：
     - 指令是否在当前关卡允许的指令集内（`Actuator::is_instruction_allowed`）。
     - 空地编号是否合法 / 是否存在积木（`Actuator::is_valid_empty_space_arg`）。
     - 跳转目标行号是否合法。
     - 特定指令的异常情况（无当前积木、输入传送带为空等）。
   - 统一输出执行结果 [`RunResult`](code/src/actuator.h)，区分：
     - `SUCCESS`：程序结束且输出与目标完全一致。
     - `FAIL`：程序正常结束但输出与目标不一致。
     - `ERROR`：执行过程中出错，如非法指令、越界、无当前积木等。

3. **交互与展示层（View & Game）**  
   - `game()` 负责一局游戏的主流程控制：关卡选择 → 关卡信息 → 指令输入 → 程序执行 → 结果展示。
   - `view.h` 提供统一的命令行界面绘制：
     - 初始关卡选择界面，支持 ↑↓ 选择关卡、展示是否解锁。
     - 单步执行界面，图形化展示机器人状态。
     - 结束结果界面，展示统计信息与提示。

这种分层架构带来的好处是：
- **可测试性**：OJ 部分只需要复用 `Robot / Level / Instruction / Actuator` 的逻辑，而不需要视图层代码。
- **可扩展性**：添加新的关卡只需在 [`LevelManager`](code/src/level.cpp) 中增加配置；若要增加新指令，只需扩展 `instruction_type`、解析函数和执行逻辑。
- **可维护性**：界面逻辑与业务逻辑分离，修改界面不会影响执行规则。

### 2. 指令解析与合法性设计

根据需求文档，异常情况包括：

- 不在指令表中的未定义指令。
- 不属于当前关卡固定可用指令集的指令。
- 不符合语法或参数要求的指令（操作数非整数、参数个数错误、负数参数等）。
- 指令特定运行时错误（如空地不存在、无当前积木等）。

为此，设计了两层检查机制：

1. **解析阶段（语法检查）**  
   - `parse_instruction_line(const string& line)` 在 [code/src/actuator.cpp](code/src/actuator.cpp) 中实现：
     - 解析出指令名和参数字符串。
     - 将指令名统一转为小写，以实现大小写不敏感。
     - 检查：
       - 指令名是否在已知集合（`inbox/outbox/add/sub/copyto/copyfrom/jump/jumpifzero`）中。
       - 参数个数是否为 0 或 1，多余空格导致的额外参数被视为非法。
       - 参数是否为整数、是否能用 `stoi` 解析。
     - 如果存在解析错误，调用 `Instruction::set_error()` 将 `is_valid` 置为 `false`。
     - 最后调用 `instr.judge_valid()`：
       - 如果指令要求参数而未给出，或不要求参数但给出了参数，判定为非法。
       - 对需要参数的指令，若参数为负数（`arg < 0`），也视为非法。
   - 在执行前，执行器对每条指令检查 `is_valid`，若为 `false`，直接以 `ERROR` 结束游戏，并输出 `"Error on instruction X"`。

2. **运行阶段（语义与关卡规则检查）**  
   - 在 `Actuator::run()` 中，对于每条待执行指令，依次进行：
     1. `pc` 越界检查：若 `pc < 0` 或 `pc >= program.size()`，说明程序已经自然结束。
     2. 指令合法性检查：若 `instr.is_valid == false`，立即返回错误。
     3. 关卡允许指令集检查：通过 `is_instruction_allowed(level, instr)` 将 `instruction_type` 映射为字符串，再查当前关卡的 `valid_instructions` 集合。
     4. 运行语义检查（按指令类型）：
        - `INBOX`：若 `input_box` 为空，则根据文档，视为“程序正常结束”的一种情况（条件 (b)），不算错误。
        - `OUTBOX`：若当前积木不存在（`current_box == INT_MIN`），报错。
        - `ADD / SUB`：通过 `is_valid_empty_space_arg()` 检查空地下标合法性和空地中是否有积木，同时要求当前积木不为空。
        - `COPYTO`：要求当前积木存在，且目标空地下标在范围内。
        - `COPYFROM`：目标空地下标在范围内，且对应空地中存在积木。
        - `JUMP / JUMPIFZERO`：跳转目标行号在原始指令范围内 `[1, program.size()]`，同时 `JUMPIFZERO` 要求当前积木存在。
   - 一旦出错，记录出错行号 `error_index = pc + 1`，并立即结束。

通过“解析阶段 + 运行阶段”的分层，既保障了对输入程序的鲁棒性，又与题目描述中的错误处理规则严格对应。

### 3. 关卡与解锁机制设计

为满足“关卡选择”和“重启后保留过关情况”的要求，设计了多层次的关卡配置方案：

1. **内置关卡配置（固定逻辑）**  
   - 在 [`LevelManager::LevelManager`](code/src/level.cpp) 中，如果静态数组 `levels` 为空，则依次构造关卡：
     - 关卡 1：简单搬运（输入=`{1,2}`，输出=`{1,2}`，无空地，只允许 `inbox/outbox`）。
     - 关卡 2：成对加减运算（含空地和加减指令的练习）。
     - 关卡 3：比较输出，只有相等的成对数字输出。
     - 关卡 4：模拟乘法（三倍放大）。
     - 关卡 5：模拟一般正整数乘法（利用计数器存储在空地上）。
   - 每一关定义：
     - 输入序列（`set_input`）。
     - 目标输出（`set_target_output`）。
     - 空地数量（`set_empty_count`）。
     - 可用指令集（`set_valid_instructions`）。
     - 标题与描述（`set_title` / `set_description`）。
   - 关卡 1 默认解锁，其余关卡可通过配置或通关来解锁。

2. **本地解锁状态持久化**  
   - 在 [`LevelManager::load_local_levels`](code/src/level.cpp) 中，从 [code/local_levels/level_unlock_status.txt](code/local_levels/level_unlock_status.txt) 读取已解锁关卡 ID，并调用 `set_level_unlocked` 更新内存状态。
   - 在通关检测处（`Actuator::run` 的结尾，根据输出对比结果）：
     - 若当前结果为 `SUCCESS`，则：
       - 在不越界的前提下解锁下一关 `level_id + 1`。
       - 调用 `set_local_level_unlocked` 将该解锁状态追加写入本地文件，使下次运行仍然保留结果。

3. **关卡选择界面与权限控制**  
   - 在 [code/src/view.h](code/src/view.h) 中，`initialize_view` 将所有关卡以列表形式展示，显示“已解锁 / 未解锁”状态。
   - 使用上下方向键选择关卡，按 Enter 确认。
   - 若选择了未解锁关卡，则给出提示信息并停留在菜单界面，防止非法进入更高关卡。

这种设计既满足了题目关于“关卡顺序”和“通关解锁”的要求，也为扩展更多关卡或改为外部配置文件留出了空间。

---

## 三、游戏界面的设计

本项目的游戏界面完全基于命令行实现，但通过字符艺术和排版尽量做到“清晰美观、信息完整”。核心逻辑集中在 [code/src/view.h](code/src/view.h)。

### 1. 关卡选择界面

- 函数：`int initialize_view(const LevelManager& level_manager)`。
- 功能：
  - 清屏后输出“关卡选择”标题与使用说明（支持方向键，提示未解锁关卡无法进入）。
  - 列出所有关卡，逐关显示编号与“已解锁 / 未解锁”状态。
  - 使用 `_getch()` 捕获键盘按键，解析上下键（72/80）移动光标，Enter 键确认。
  - 额外提供一项“退出游戏”，选择后返回 `0`。
- 用户体验：
  - 类似简易文本菜单，支持多次选择与错误提示（例如尝试进入未解锁关卡时提示“该关卡尚未解锁”）。

### 2. 关卡信息界面

- 函数：`void show_level_info(const Level& level)`。
- 显示内容包括：
  - 关卡 ID 与标题。
  - 关卡描述（任务说明）。
  - 输入序列。
  - 目标输出序列。
  - 空地数量。
  - 有效指令集合（逐条打印）。
- 交互：
  - 显示完毕后提示“按 Enter 键继续...”，等待用户确认进入指令输入阶段。

这样设计可以让玩家在编写程序前，即清楚了解本关任务与可用工具集。

### 3. 主游戏执行界面（单步模拟）

- 函数：`void show_one_step(const Robot& robot)`。
- 设计原则：
  - 每执行一步指令后，立即刷新整个界面，让玩家直观看到状态变化。
  - 将界面拆为左右两部分：
    - 左侧：输入传送带、机器人、当前积木、输出传送带、空地。
    - 右侧：程序指令列表与当前执行行标记。
- 主要元素布局：
  1. **顶部标题栏**  
     - 使用长分隔线和居中标题，显示“关卡 ID + 标题”。
  2. **传送带和机器人区域**（左侧）
     - 使用 ASCII 画框实现类似“盒子”的效果：
       - 输入传送带和输出传送带用多格 `+---+` 形式展示，每格显示一个数字或 `*`。
       - 当前积木单独显示一个小方框，位于机器人旁侧。
     - 机器人使用 ASCII 艺术：
       - 如 `[^_^]`、`/|\`、`/_|\` 等组合，突出“机器人”形象。
     - 空地状态显示在界面下方：
       - 将每个空地编号和当前存储的数字以小格子呈现，空位显示为 `*`。
  3. **指令列表区域**（右侧）
     - 使用 `format_instr_for_view(const Instruction& instr, int index, int pc)` 进行格式化。
     - 每行显示形式为：
       - 当前执行行前面加 `" -> "` 标记，其它行为空格对齐。
       - 显示“行号 + 指令名 + 参数（如果有）”。
     - 最多显示前 30 条指令，避免程序过长导致界面溢出，后面通过“... 共 N 条指令，已省略后续”提示。
     - 底部还有“执行步数: X”统计信息。
  4. **交互节奏**
     - 每次调用 `show_one_step` 时，先通过 `"\033[2J\033[H"` 清屏，然后重绘完整界面。
     - 最后输出“按 Enter 执行下一步...”，等待用户按 Enter 才继续执行下一条指令（在 OJ 模式下关闭界面模块）。

这样的界面设计使得玩家可以清楚看到：
- 输入传送带的剩余元素。
- 输出传送带当前已有的输出。
- 空地中目前存放的数据及位置。
- 机器人当前手中的积木。
- 正在执行的哪条指令，以及执行总步数。

### 4. 结果展示界面

- 函数：`void show_final_result(const Actuator& actuator, const RunResult& result)`。
- 显示逻辑：
  - 清屏并输出“第 N 关 结果”的标题。
  - 若 `result.type == SUCCESS`：
    - 打印 `"Success! 您已通过第N关"`。
  - 若 `result.type == FAIL`：
    - 打印 `"Fail! 程序执行结束，但输出与目标输出不一致。"`
    - 打印目标输出序列（从关卡信息中获取）。
    - 打印玩家实际输出序列（通过 `actuator.print_outbox()`）。
    - 显示统计信息（程序长度、实际执行条数、最终 `pc` 值）。
  - 若 `result.type == ERROR`：
    - 打印 `"Error on instruction X!"`，与题目要求的错误提示格式保持一致。
- 统计信息的展示帮助玩家分析程序效率，理解某些程序可能进入死循环或做了无效跳转。

---

## 四、创新关卡的设计

在完成题目要求的三个基础关卡之后，项目在 [code/src/level.cpp](code/src/level.cpp) 中继续设计了两个 **模拟乘法** 类的创新关卡，以加深对指令组合的利用，并体现出“用简单指令构造复杂运算”的思想。

### 1. 关卡 4：模拟乘法——三倍放大

- 配置位置：`Level level4(4);` 相关代码位于 [code/src/level.cpp](code/src/level.cpp)。
- 任务描述（`set_description`）：
  > “考虑使用累计加法来模拟乘法”
- 关卡设置：
  - 输入序列：`{3, 1, -5, 4, 0, -2}`。
  - 目标输出序列：`{9, 3, -15, 12, 0, -6}`，即将每个输入数放大 3 倍。
  - 空地数量：`1`。
  - 可用指令集：
    - `"inbox", "outbox", "copyto", "copyfrom", "add", "jump"`。
  - 设计思路：
    - 玩家需要自行在空地上构造“常数累加”的过程，利用有限的指令在没有 `sub`、没有条件判断的条件下，实现对每个输入数进行三次相加。
    - 因为空地只有 1 个，玩家必须合理规划“中间变量”的重用，如：
      - 将当前输入复制到空地。
      - 利用 `add` 不断用空地上的值累加当前积木。
      - 合理使用无条件 `jump` 构造循环。
  - 教学目标：
    - 引导玩家理解“乘法可以通过加法和循环实现”这一编程基本思想。
    - 训练使用 `copyto/copyfrom/add/jump` 构造简单循环结构。

### 2. 关卡 5：模拟乘法——正整数相乘

- 配置位置：`Level level5(5);` 相关代码位于 [code/src/level.cpp](code/src/level.cpp)。
- 任务描述（`set_description`）：
  > “考虑在空地存储一个计数器实现乘法模拟”
- 关卡设置：
  - 输入序列：`{3, 4, 2, 5, 0, 1}`  
    - 可以理解为多组 `(a, b)`，需要输出对应的乘积。
  - 目标输出序列：`{12, 10, 0}`。
  - 空地数量：`4`。
  - 可用指令集：
    - `"inbox", "outbox", "copyto", "copyfrom", "add", "sub", "jump", "jumpifzero"`。
- 设计思路：
  - 本关在关卡 4 的基础上，引入了：
    - 多个空地。
    - `sub` 与 `jumpifzero`，即具备了完整的“计数器 + 条件跳转”能力。
  - 典型解法会涉及：
    - 使用空地存储被乘数与乘数。
    - 用某一个空地作为“循环计数器”，反复 `add` 并 `sub 1`（或 `sub` 空地计数），通过 `jumpifzero` 退出循环。
  - 相比前面关卡，这一关更接近真实编程中的“for/while 循环 + 变量 + 条件判断”，是对前几个关卡所学指令的综合运用。

### 3. 创新设计亮点

1. **任务从简单到复杂的递进性**  
   - 关卡 1：熟悉 `inbox/outbox` 的基本搬运流程。
   - 关卡 2：掌握 `add/sub/copyto/copyfrom/jump/jumpifzero` 的组合使用，体会对偶元素操作。
   - 关卡 3：引入条件判断的实际应用（比较两个数是否相等）。
   - 关卡 4~5（创新关卡）：从“固定倍数的放大”到“一般乘法”，在有限的命令集合下，训练玩家构造抽象的运算逻辑。

2. **鼓励算法思维与抽象能力**  
   - 将数学运算拆解为基本的机器指令组合，实际体验“从硬件视角写程序”的思路。
   - 玩家需要自己思考“如何用最基础的加减和跳转，构造循环与乘法”，从而加深对程序设计基础的理解。

3. **配合界面实现可视化调试**  
   - 创新关卡往往程序较长、逻辑更复杂，配合 `show_one_step` 的单步执行界面，玩家可以清晰跟踪每一次循环中的状态变化：
     - 计数器是否正确减小。
     - 输出是否逐步累积。
     - 跳转是否在预期行号之间循环。

通过这样的创新关卡设计，项目不仅完成了题目规定的基础功能和关卡任务，也进一步展示了“用有限指令构造复杂行为”的编程思想，增强了游戏性和教学性。

---