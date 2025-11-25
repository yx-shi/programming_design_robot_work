#pragma once
#include<vector>
#include<set>
#include<string>
using namespace std;

class Level{
    int id;           //关卡ID
    bool is_unlocked;   //是否解锁
    vector<int> input;  //输入序列
    vector<int> target_output;  //目标输出序列
    int empty_count;  //空地数量
    set<string> valid_instructions;  //有效指令集合
    string description;  //关卡描述
public:
    void unlock();
    Level(int level_id=0):id(level_id),is_unlocked(false),empty_count(0){}
    int get_empty_count() const{return empty_count;}
    void set_input(const vector<int>& in);
    void set_target_output(const vector<int>& out);
    void set_empty_count(int count);
    void set_valid_instructions(const set<string>& instructions);
    void set_description(const string& desc);
};

class LevelManager{
    static vector<Level> levels; //关卡列表
public:
    LevelManager();
    Level get_level(int level_id);
    int get_level_count();
};