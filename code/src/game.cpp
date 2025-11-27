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
    int input_error_count=0;
    while(1){
        cout<<"请选择指令输入方式：\n1-直接输入指令\n2-读取文件\n请选择“1”或“2”:"<<endl;
        //直接读取用户的全部输入，对输入采用更鲁棒的处理方式
        string choice_str;
        getline(cin, choice_str);
        //去除首尾空格
        int choice=stoi(choice_str);
        if(choice==1){
            show_read_from_cli();
            int ins_num;
            cin>>ins_num;
            cout<<"请依次输入"<<ins_num<<"条指令,注意每行一个指令，指令和操作数之间用空格分隔："<<endl;
            actuator.read_from_cli(ins_num);
            break;
        }
        else if(choice==2){
            show_read_from_file();
            string file_path;
            cout<<"请输入文件路径："<<endl;
            getline(cin, file_path);
            if(!actuator.read_from_file(file_path)){
                cout<<"文件读取失败，请重新输入文件路径。"<<endl;
                continue;
            }
            break;
        }
        else{
            if(input_error_count>=3){
                cout<<"多次输入错误，程序退出。"<<endl;
                exit(1);
            }
            else{
                cout<<"输入有误，请重新选择“1”或“2”："<<endl;
                input_error_count++;
            }
        }
    }
    RunResult result = actuator.run(); //执行程序
    //其他待补充
}