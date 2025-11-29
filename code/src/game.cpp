#include "view.h"
#include "actuator.h"
#include "level.h"
#include "robot.h"
#include <limits>

Actuator actuator;  //全局执行器对象


int game(){
    LevelManager level_manager;
    int level_id = initialize_view(level_manager); // 返回0表示退出
    if (level_id == 0) {
        cout << "\033[2J\033[H";
        cout << "已退出游戏。" << endl;
        return 1;
    }
    actuator.reset(level_id);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //处理用户输入
    int input_error_count=0;
    while(1){
        cout << "\033[2J\033[H";
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
            cout << "\033[2J\033[H";
            cout<<"请依次输入"<<ins_num<<"条指令，注意每行一个指令，指令和操作数之间用空格分隔："<<endl;
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
    show_final_result(actuator, result); //显示最终结果
    return 0;
}

int main() {
    while(true){
        cout<<"欢迎来到机器人编程游戏！请决定是否开始游戏（y/n）"<<endl;
        char start_choice;
        cin>>start_choice;
        if(start_choice == 'y' || start_choice == 'Y'){
            if(game() != 0){
                break;
            }
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