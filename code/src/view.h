#include<iostream>
#include "actuator.h"
#include "level.h"
#include"robot.h"
using namespace std;

void initialize_view(const LevelManager& level_manager) {
    /*TODO：显示开始页面,也就是关卡选择页面，要求显示：
    * 关卡列表
    * 每个关卡的解锁状态
    * 提示用户选择关卡
    */
}

void show_one_step(const Robot& robot) {
    /*TODO: 执行run()过程中每一步显示，显示当前界面，也就是大作业文档中所附的那张图片的效果
    * 即显示：关卡信息描述、输入传送带、当前积木、机器人、空地、机器人程序、当前指令等游戏元素
    */
}

void show_final_result(const Robot& robot, const RunResult& result) {
    //TODO: 显示最终结果界面，包括成功或失败的信息，执行的指令数等
}

void show_read_from_cli() {
    cout<<"当前输入模式，直接从命令行输入指令。\n请输入总的指令数量："<<endl;
}

void show_read_from_file() {
    cout<<"当前输入模式，从文件读取指令。\n请输入文件路径："<<endl;
}