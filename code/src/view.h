#include<iostream>
#include "actuator.h"
#include "level.h"
#include"robot.h"
using namespace std;

void initialize_view(const LevelManager& level_manager) {
    //TODO：显示开始页面
}

void show_one_step(const Robot& robot) {
    //TODO: 执行单步后，显示当前界面
}

void show_final_result(const Robot& robot, const RunResult& result) {
    //TODO: 显示最终结果界面
}

void show_read_from_cli() {
    cout<<"当前输入模式，直接从命令行输入指令。\n请输入总的指令数量："<<endl;
}

void show_read_from_file() {
    cout<<"当前输入模式，从文件读取指令。\n请输入文件路径："<<endl;
}