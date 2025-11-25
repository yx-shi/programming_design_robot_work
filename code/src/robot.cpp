#include "robot.h"
#include "level.h"
#include<iostream>

using namespace std;

Robot::Robot(int level_id){
    reset(level_id);
}

void Robot::reset(int level_id){
    // 清空传送带和空地状态
    while(!input_box.empty()) input_box.pop();
    output_box.clear();
    empty_spaces.clear();
    pc = 0;
    exec_count = 0;
    current_box = INT_MIN;

    // 获取关卡信息
    LevelManager lm;
    Level level = lm.get_level(level_id);

    //TODO: 根据关卡信息初始化输入传送带

    // 初始化空地状态
    empty_spaces.resize(level.get_empty_count(), INT_MIN);
}