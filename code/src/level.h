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
    int get_empty_count() const{return empty_count;}   //获取空地数量
    void set_input(const vector<int>& in);    //设置输入序列
    void set_target_output(const vector<int>& out);  //设置目标输出序列
    void set_empty_count(int count);  //设置空地数量
    void set_valid_instructions(const set<string>& instructions);  //设置有效指令集合
    void set_description(const string& desc);  //设置关卡描述
    set<string> get_valid_instructions() const { return valid_instructions; }
};

class LevelManager{
    static vector<Level> levels; //关卡列表
public:
    LevelManager();
    Level get_level(int level_id) const;
    // 返回关卡总数）
    int get_level_count() const { return levels.size(); }

    //返回静态关卡数组的引用，便于菜单中遍历
    const vector<Level>& get_all_levels() const { return levels; }
};