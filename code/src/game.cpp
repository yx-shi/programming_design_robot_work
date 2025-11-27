#include "view.h"
#include "actuator.h"
#include "level.h"
#include "robot.h"

Actuator actuator;        //全局执行器对象


void game(){
    LevelManager level_manager;
    initialize_view(level_manager);//初始化加载界面
    //选择关卡
    int level_id = 0; 
    //实现关卡选择界面，获取用户选择的 level_id
    cin.ignore(); //清除main函数中cin>>start_choice留下的换行符
    while(1){
        cout<<"请输入要挑战的关卡ID（1 - "<<level_manager.get_level_count()<<")："<<endl;
        string level_id_str;
        getline(cin, level_id_str);
        try{
            level_id = stoi(level_id_str);
        } catch (const invalid_argument&) {
            cout<<"关卡ID输入无效，请重新输入。"<<endl;
            continue;
        }
        
        if(!level_manager.is_level_unlocked(level_id)){
            cout<<"关卡未解锁或不存在，请重新输入！"<<endl;
        }
        else{
            Level level = level_manager.get_level(level_id);
            actuator.reset(level_id);
            break;
        }
    }
    //处理用户输入
    int input_error_count=0;
    while(1){
        cout<<"请选择指令输入方式：\n1-直接输入指令\n2-读取文件\n请选择“1”或“2”:"<<endl;
        //直接读取用户的全部输入，对输入采用更鲁棒的处理方式
        string choice_str;
        getline(cin, choice_str);
        //去除首尾空格
        try{
            stoi(choice_str);
        } catch (const invalid_argument&) {
            if(input_error_count>=3){
                cout<<"多次输入错误，程序退出。"<<endl;
                exit(1);
            }
            else{
                cout<<"输入有误，请重新选择“1”或“2”："<<endl;
                input_error_count++;
                continue;
            }
        }
        int choice=stoi(choice_str);
        if(choice==1){
            show_read_from_cli();
            string ins_num_str;
            getline(cin, ins_num_str);
            try{
                stoi(ins_num_str);
            } catch (const invalid_argument&) {
                cout<<"指令数量输入无效，请重新输入。"<<endl;
                continue;
            }
            int ins_num = stoi(ins_num_str);
            cout<<"请依次输入"<<ins_num<<"条指令,注意每行一个指令，指令和操作数之间用空格分隔："<<endl;
            actuator.read_from_cli(ins_num);
            break;
        }
        else if(choice==2){
            show_read_from_file();
            string file_path;
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
    } else if(result.type == RunResultType::ERROR) {
        cout << "Error on instruction " << result.error_index << "! " << endl;
    }
}

int main() {
    while(true){
        cout<<"欢迎来到机器人编程游戏！请决定是否开始游戏（y/n）"<<endl;
        char start_choice;
        cin>>start_choice;
        if(start_choice == 'y' || start_choice == 'Y'){
            game();
        }
        else if(start_choice == 'n' || start_choice == 'N'){
            break;
        }
        else{
            cout<<"输入无效，请输入'y'或'n'。"<<endl;
        }
    }
    return 0;
}