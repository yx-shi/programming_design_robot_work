#include "view.h"
#include "actuator.h"
#include "level.h"
#include "robot.h"

LevelManager level_manager; //全局关卡管理器
Actuator actuator;        //全局执行器对象

void initialize(){
    //TODO: 初始化游戏状态，如加载关卡等
}

void game(){
    initialize();//初始化游戏状态
    initialize_view(level_manager);//初始化加载界面
    //选择关卡
    int level_id = 0; 
    //TODO: 实现关卡选择界面，获取用户选择的 level_id
    Level level = level_manager.get_level(level_id);
    actuator.reset(level_id);
    //处理用户输入
    int m; cin >> m; //指令数量
    actuator.read_as_program(m); //读取并解析指令
    RunResult result = actuator.run(); //执行程序
    //其他待补充
}