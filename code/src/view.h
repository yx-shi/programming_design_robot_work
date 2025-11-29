#include<iostream>
#include "actuator.h"
#include "level.h"
#include"robot.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <conio.h>
using namespace std;

inline int read_menu_key() {
    int ch = _getch();
    if (ch == 0 || ch == 0xE0) ch = _getch();
    return ch;
}

inline int initialize_view(const LevelManager& level_manager) {
    const int level_count = level_manager.get_level_count();
    const int total_items = level_count + 1; // 加上“退出游戏”
    int cursor = 0;
    string message;

    while (true) {
        cout << "\033[2J\033[H";
        cout << "==================== 关卡选择 ====================" << endl;
        cout << "使用 ↑ ↓ 选择关卡，Enter 确认" << endl;
        cout << "（未解锁的关卡无法进入）" << endl;
        cout << "------------------------------------------------" << endl;

        for (int i = 0; i < level_count; ++i) {
            bool unlocked = level_manager.is_level_unlocked(i + 1);
            string status = unlocked ? "已解锁" : "未解锁";
            cout << (cursor == i ? "> " : "  ");
            cout << "关卡" << (i + 1) << ": " << status << endl;
        }
        cout << (cursor == level_count ? "> " : "  ") << "退出游戏" << endl;

        if (!message.empty()) {
            cout << "------------------------------------------------" << endl;
            cout << "提示：" << message << endl;
        }

        int ch = read_menu_key();

        if (ch == 72) { // Arrow Up
            cursor = (cursor - 1 + total_items) % total_items;
        } else if (ch == 80) { // Arrow Down
            cursor = (cursor + 1) % total_items;
        } else if (ch == 13) { // Enter
            if (cursor == level_count) {
                return 0; // 退出
            }
            if (!level_manager.is_level_unlocked(cursor + 1)) {
                message = "该关卡尚未解锁";
                continue;
            }
            return cursor + 1;
        }
    }
}

// 辅助函数：格式化指令显示（供界面与程序区使用）
inline string format_instr_for_view(const Instruction& instr, int index, int pc) {
    stringstream ss;
    // 标记当前执行行
    if (index == pc + 1) ss << " -> ";
    else ss << "    ";
    ss << setw(2) << index << ". ";

    string name;
    switch(instr.instruction) {
        case instruction_type::INBOX: name = "inbox"; break;
        case instruction_type::OUTBOX: name = "outbox"; break;
        case instruction_type::ADD: name = "add"; break;
        case instruction_type::SUB: name = "sub"; break;
        case instruction_type::COPYTO: name = "copyto"; break;
        case instruction_type::COPYFROM: name = "copyfrom"; break;
        case instruction_type::JUMP: name = "jump"; break;
        case instruction_type::JUMPIFZERO: name = "jumpifzero"; break;
    }
    ss << left << setw(12) << name;
    if (instr.has_arg()) {
        ss << instr.arg;
    }
    return ss.str();
}

inline void show_one_step(const Robot& robot) {
    // 清屏（支持大部分终端，若不支持则只是换到新行）
    cout << "\033[2J\033[H"; // ANSI 清屏+光标归位

    LevelManager level_manager;
    Level cur_level = level_manager.get_level(robot.level_id);
    string level_desc = cur_level.get_description();

    // 顶部关卡与标题信息
    cout << "======================================================================" << endl;
    cout << "                         机器人积木关卡";
    cout << " (Level " << robot.level_id << ")" << endl;
    cout << "-----------------------------------------------------------------------" << endl;
    cout << "描述: " << level_desc << endl;
    cout << "======================================================================" << endl;

    // 上半部分：游戏画面（输入/输出/机器人/当前积木）与右侧指令列表
    const size_t LABEL_WIDTH = 12;
    const size_t LEFT_WIDTH = 80;

    auto pad_label = [&](const string& label) {
        string txt = label;
        if (txt.size() < LABEL_WIDTH) txt.append(LABEL_WIDTH - txt.size(), ' ');
        else txt.push_back(' ');
        return txt;
    };

    auto display_width = [](const string& text) {
        size_t width = 0;
        for (size_t i = 0; i < text.size();) {
            unsigned char ch = static_cast<unsigned char>(text[i]);
            if (ch < 0x80) {
                ++width;
                ++i;
            } else if ((ch & 0xE0) == 0xC0 && i + 1 < text.size()) {
                width += 2;
                i += 2;
            } else if ((ch & 0xF0) == 0xE0 && i + 2 < text.size()) {
                width += 2;
                i += 3;
            } else if ((ch & 0xF8) == 0xF0 && i + 3 < text.size()) {
                width += 2;
                i += 4;
            } else {
                ++width;
                ++i;
            }
        }
        return width;
    };

    auto pad_to_width = [&](const string& text, size_t width) {
        string result = text;
        size_t current = display_width(result);
        if (current >= width) return result;
        result.append(width - current, ' ');
        return result;
    };

    auto render_horizontal_boxes = [](const vector<string>& slots) {
        vector<string> lines(3, "");
        for (const auto& raw : slots) {
            string val = raw;
            if (val.size() > 3) val = val.substr(0, 3);
            if (val.size() < 3) val.append(3 - val.size(), ' ');
            lines[0] += "+---+ ";
            lines[1] += "|" + val + "| ";
            lines[2] += "+---+ ";
        }
        for (auto& line : lines) {
            if (!line.empty() && line.back() == ' ') line.pop_back();
        }
        return lines;
    };

    auto center_text = [](const string& text, size_t width) {
        if (text.size() >= width) return text.substr(0, width);
        size_t total = width - text.size();
        size_t left = total / 2;
        size_t right = total - left;
        return string(left, ' ') + text + string(right, ' ');
    };

    auto queue_to_strings = [](queue<int> q) {
        vector<string> vals;
        while (!q.empty()) {
            vals.push_back(to_string(q.front()));
            q.pop();
        }
        return vals;
    };

    auto fill_slots = [](const vector<string>& values) {
        vector<string> slots(8, "*");
        size_t limit = min<size_t>(8, values.size());
        for (size_t i = 0; i < limit; ++i) slots[i] = values[i];
        return slots;
    };

    vector<string> input_slots = fill_slots(queue_to_strings(robot.input_box));

    vector<string> output_values_str;
    for (size_t i = 0; i < robot.output_box.size(); ++i) {
        output_values_str.push_back(to_string(robot.output_box[i]));
    }
    vector<string> output_slots = fill_slots(output_values_str);

    string current_val = (robot.current_box == INT_MIN) ? string("*") : to_string(robot.current_box);

    vector<string> left_lines;
    bool first_section = true;
    auto begin_section = [&]() {
        if (!first_section) left_lines.push_back("");
        first_section = false;
    };

    auto append_box_section = [&](const string& label, const vector<string>& slots) {
        begin_section();
        auto box_lines = render_horizontal_boxes(slots);
        left_lines.push_back(pad_label("") + box_lines[0]);
        left_lines.push_back(pad_label(label) + box_lines[1]);
        left_lines.push_back(pad_label("") + box_lines[2]);
    };

    append_box_section("INPUT_BOX", input_slots);
    vector<string> robot_art = {
        "      [^_^]      ",
        "       /|\\       ",
        "      /_|_\\      "
    };
    auto current_box_lines = render_horizontal_boxes({current_val});
    begin_section();
    for (size_t i = 0; i < robot_art.size(); ++i) {
        string label = (i == 1) ? "ROBOT" : "";
        string current_label = (i == 1) ? "   当前积木   " : "              ";
        string combined = robot_art[i] + current_label + current_box_lines[i];
        left_lines.push_back(pad_label(label) + combined);
    }
    append_box_section("OUTPUT_BOX", output_slots);

    // 右侧指令列表（保留最多 20 条）
    vector<string> instr_lines;
    instr_lines.push_back("指令序列 (最多显示前 20 条，-> 为当前执行)：");
    int total_instr = static_cast<int>(robot.program.size());
    int instr_limit = min(total_instr, 20);
    for (int i = 0; i < instr_limit; ++i) {
        instr_lines.push_back(format_instr_for_view(robot.program[i], i + 1, robot.pc));
    }
    if (total_instr > instr_limit) {
        instr_lines.push_back("    ... 共 " + to_string(total_instr) + " 条指令，已省略后续");
    }
    instr_lines.push_back("");
    instr_lines.push_back("执行步数: " + to_string(robot.exec_count));

    size_t rows = max(left_lines.size(), instr_lines.size());
    for (size_t i = 0; i < rows; ++i) {
        string left = (i < left_lines.size()) ? left_lines[i] : "";
        string right = (i < instr_lines.size()) ? instr_lines[i] : "";
        cout << pad_to_width(left, LEFT_WIDTH) << " || " << right << endl;
    }

    // 空地状态显示在最下方
    if (!robot.empty_spaces.empty()) {
        cout << endl;
        cout << "空地状态：" << endl;
        vector<string> space_values;
        vector<string> space_labels;
        for (size_t i = 0; i < robot.empty_spaces.size(); ++i) {
            if (robot.empty_spaces[i] == INT_MIN) space_values.push_back("*");
            else space_values.push_back(to_string(robot.empty_spaces[i]));
            space_labels.push_back(to_string(i));
        }
        auto space_box_lines = render_horizontal_boxes(space_values);
        for (const auto& line : space_box_lines) {
            cout << "    " << line << endl;
        }
        string label_line;
        for (const auto& lab : space_labels) {
            label_line += " " + center_text(lab, 3) + " ";
        }
        if (!label_line.empty() && label_line.back() == ' ') label_line.pop_back();
        cout << "    " << label_line << endl;
    }

    cout << "======================================================================" << endl;
    cout << "按 Enter 执行下一步..." << endl;
    string _step_pause;
    getline(cin, _step_pause);
}

inline void show_final_result(const Actuator& actuator, const RunResult& result) {
    // 显示结果标题
    Robot robot = actuator.get_robot();
    int level_id = robot.level_id;
    LevelManager level_manager;
    cout << "\033[2J\033[H";
    cout << "==================== 第" << level_id << "关 结果 ====================" << endl;
    if(result.type == RunResultType::SUCCESS) {
        cout << "Success! 您已通过第" << level_id << "关" << endl;
    } else if(result.type == RunResultType::FAIL) {
        cout << "Fail! 程序执行结束，但输出与目标输出不一致。" << endl;
        cout<<"目标输出为：";
        Level cur_level = level_manager.get_level(level_id);
        for(int val : cur_level.get_target_output()){
            cout<<val<<" ";
        }
        cout << endl;
        cout<<"您的输出为：";  
        actuator.print_outbox();
        // 显示详细统计信息
        cout << "📊 执行统计：" << endl;
        cout << "  - 总指令数：" << robot.program.size() << "条" << endl;
        cout << "  - 实际执行：" << robot.exec_count << "条" << endl;
        cout << "  - 最终指令进度：第" << robot.pc << "条" << endl;
        cout << endl;
    } else if(result.type == RunResultType::ERROR) {
        cout << "Error on instruction " << result.error_index << "! " << endl;
    }
}

inline void show_read_from_cli() {
    cout << "\033[2J\033[H";
    cout<<"当前输入模式：直接从命令行输入指令。\n请输入总的指令数量："<<endl;
}

inline void show_read_from_file() {
    cout << "\033[2J\033[H";
    cout<<"当前输入模式：从文件读取指令。\n请输入文件路径："<<endl;
}


