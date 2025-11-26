#pragma once
#include<vector>
#include<queue>
#include<string>
#include<climits>
using namespace std;

struct Instruction;//前向声明

//Robot类，其中封装了某一时刻机器人和环境的全部状态。
class Robot{
public:
    int level_id;    //当前关卡ID
    queue<int> input_box;  //输入传送带状态
    vector<int> output_box; //输出传送带状态
    vector<int> empty_spaces; //空地状态，INT_MIN表示无积木
    int pc;              //当前指令编号,注意指令是1-based
    int exec_count;      //已执行指令计数
    int current_box;     //当前的积木
    vector<Instruction> program; //当前加载的程序
    Robot():pc(0),exec_count(0),current_box(INT_MIN){}
    Robot(int level_id);   //根据关卡ID初始化机器人状态
    void reset(int level_id); //重置机器人状态
};