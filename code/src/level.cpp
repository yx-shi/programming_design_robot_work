#include "level.h"
#include<iostream>
#include<fstream>
#include "conf.h"

using namespace std;

void Level::unlock(){
    is_unlocked = true;
}
void Level::set_input(const vector<int>& in){
    input = in;
}
void Level::set_target_output(const vector<int>& out){
    target_output = out;
}
void Level::set_empty_count(int count){
    empty_count = count;
}
void Level::set_valid_instructions(const set<string>& instructions){
    valid_instructions = instructions;
}
void Level::set_description(const string& desc){
    description = desc;
}

vector<Level> LevelManager::levels; // 静态成员初始化

LevelManager::LevelManager(){
    if(levels.empty()){
        // ========== 初始化关卡1 ==========
        Level level1(1); // 关卡ID为1
        // 设置输入序列
        vector<int> input1 = {1, 2};
        level1.set_input(input1);
        // 设置目标输出序列
        vector<int> output1 = {1, 2};
        level1.set_target_output(output1);
        // 设置空地数量
        level1.set_empty_count(0);
        // 设置有效指令集合
        set<string> inst1 = {"inbox", "outbox"};
        level1.set_valid_instructions(inst1);
        // 设置关卡描述
        string desc1 = "让机器人取出输入序列上的每个积木放入输出序列中";
        level1.set_description(desc1);
        // 解锁关卡1（默认第一个关卡解锁）
        level1.unlock();
        // 添加到关卡列表
        levels.push_back(level1);

        // ========== 初始化关卡2 ==========
        Level level2(2); // 关卡ID为2
        // 设置输入序列
        vector<int> input2 = {3, 9, 5, 1, -2, -2, 9, -9};
        level2.set_input(input2);
        // 设置目标输出序列
        vector<int> output2 = {-6, 6, 4, -4, 0, 0, 18, -18};
        level2.set_target_output(output2);
        // 设置空地数量
        level2.set_empty_count(3);
        // 设置有效指令集合
        set<string> inst2 = {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"};
        level2.set_valid_instructions(inst2);
        // 设置关卡描述
        string desc2 = "对于输入序列中的每两个东西，先把第1个减去第2个，并把结果放在输出序列中，然后把第2个减去第1个，再把结果放在输出序列中，重复。";
        level2.set_description(desc2);
        // 关卡2默认未解锁（可根据需求调整）
        //根据本地存储的文件决定是否解锁
        levels.push_back(level2);

        // ========== 初始化关卡3 ==========
        Level level3(3); // 关卡ID为3
        // 设置输入序列
        vector<int> input3 = {6, 2, 7, 7, -9, 3, -3, -3};
        level3.set_input(input3);
        // 设置目标输出序列
        vector<int> output3 = {7, -3};
        level3.set_target_output(output3);
        // 设置空地数量
        level3.set_empty_count(3);
        // 设置有效指令集合
        set<string> inst3 = {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"};
        level3.set_valid_instructions(inst3);
        // 设置关卡描述
        string desc3 = "从输入序列中依次取2个数字，如果相等则将其中一个输出，否则扔掉。重复这个过程直到输入传送带为空。";
        level3.set_description(desc3);
        // 关卡3默认未解锁（可根据需求调整）
        levels.push_back(level3);
        load_local_levels(); // 从本地存储加载关卡解锁状态
    }
}

// 实现get_level函数：根据ID获取关卡
const Level& LevelManager::get_level(int level_id) const {
    for (const auto& level : levels) {
        if (level.get_id() == level_id) { 
            return level;
        }
    }
    // 若未找到，返回默认构造的Level对象
    return levels[0]; 
}

bool LevelManager::is_level_unlocked(int level_id) const {
    for (const auto& level : levels) {
        if (level.get_id() == level_id) {
            return level.is_unlocked_level();
        }
    }
    return false; // 未找到该关卡，视为未解锁
}

void LevelManager::set_local_level_unlocked(int level_id){
    // 将解锁的关卡ID写入本地存储文件
    string filepath = local_level_path + "level_unlock_status.txt";
    ofstream outfile(filepath, ios::app); // 以追加模式打开文件
    if(!outfile.is_open()){
        cerr<<"无法打开本地关卡存储文件："<<filepath<<endl;
        return;
    }
    outfile << level_id << endl;
    outfile.close();
}

void LevelManager::set_level_unlocked(int level_id){
    for(auto& level : levels){
        if(level.get_id() == level_id){
            level.unlock();
            return;
        }
    }
}

void LevelManager::load_local_levels(){
    string filepath = local_level_path + "level_unlock_status.txt";
    ifstream infile(filepath);
    if(!infile.is_open()){
        cerr<<"无法打开本地关卡存储文件："<<filepath<<endl;
        return;
    }
    int level_id;
    while(infile >> level_id){
        set_level_unlocked(level_id);
    }
    infile.close();
}