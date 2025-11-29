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
    void set_input(const vector<int>& in);    
    void set_target_output(const vector<int>& out);  
    void set_empty_count(int count);  
    void set_valid_instructions(const set<string>& instructions);  
    void set_description(const string& desc); 
    int get_id() const { return id; }   
    int get_empty_count() const{return empty_count;}   
    bool is_unlocked_level() const { return is_unlocked; } 
    vector<int> get_input() const { return input; }
    vector<int> get_target_output() const { return target_output; }
    set<string> get_valid_instructions() const { return valid_instructions; }
    string get_description() const { return description; }
};

class LevelManager{
    static vector<Level> levels; //关卡列表
public:
    LevelManager();
    const Level& get_level(int level_id) const;
    int get_level_count() const { return levels.size(); }
    const vector<Level>& get_all_levels() const { return levels; }
    bool is_level_unlocked(int level_id) const;
    void set_local_level_unlocked(int level_id); //本地存储解锁状态
    void set_level_unlocked(int level_id);
    void load_local_levels(); //从本地存储加载关卡解锁状态
};