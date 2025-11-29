#include<iostream>
#include "actuator.h"
#include "level.h"
#include"robot.h"
using namespace std;

void initialize_view(const LevelManager& level_manager) {
    // 显示标题与关卡列表
    cout << "==================== 关卡选择 ====================" << endl;
    cout << "📌 请选择要挑战的关卡（已解锁关卡可直接进入）" << endl;
    cout << "------------------------------------------------" << endl;

    // 遍历所有关卡，显示编号、解锁状态
    for (int i = 1; i <= level_manager.get_level_count(); ++i) {
        string status = level_manager.is_level_unlocked(i) ? "✅ 已解锁" : "❌ 未解锁";
        cout << "  关卡" << i << "：" << status << endl;
    }

    // 输入提示
    cout << "------------------------------------------------" << endl;
    cout << "请输入关卡编号（1-" << level_manager.get_level_count() << "）：";
}

void show_one_step(const Robot& robot) {
    /*TODO: 执行run()过程中每一步显示，显示当前界面，也就是大作业文档中所附的那张图片的效果
    * 即显示：关卡信息描述、输入传送带、当前积木、机器人、空地、机器人程序、当前指令等游戏元素
    */
}

void show_final_result(const Actuator& actuator, const RunResult& result) {
    // 显示结果标题
    Robot robot = actuator.get_robot();
    int level_id = robot.level_id;
    LevelManager level_manager;
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

void show_read_from_cli() {
    cout<<"当前输入模式，直接从命令行输入指令。\n请输入总的指令数量："<<endl;
}

void show_read_from_file() {
    cout<<"当前输入模式，从文件读取指令。\n请输入文件路径："<<endl;
}