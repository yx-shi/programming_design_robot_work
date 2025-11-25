#include "level.h"
#include<iostream>

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

vector<Level> LevelManager::levels; //静态成员初始化

LevelManager::LevelManager(){
    //TODO:初始化关卡列表，根据文档要求硬编码实现前三关
}