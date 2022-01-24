#include <fstream>
#include <iostream>
#include <cassert>
#include "mdReader.h"

namespace sp{

MDReader::MDReader(std::string path){
    this->path = path;
    std::ifstream fin(path);
    if(!fin.is_open()){
        std::cout << "app file not opened." << std::endl;
    }
    assert(fin.is_open());
    char c;
    char last;
    std::string name = "";
    std::string s = "";
    fin >> c;
    std::vector<std::vector<std::string>> v;
    std::vector<std::string> line;
    while(true){
        while(fin >> c){     // 解析表名
            if(c == ':'){
                name = s;
                s = "";
                break;
            }
            s += c;
        }
        if(fin >> c && c != '|') // 文件末尾
            break;
        bool endFlag = false;
        while(true){
            if(endFlag)
                break;
            bool clFlag = false;
            while(true){
                if(clFlag)
                    break;
                while(fin >> c){
                    if(c == '|' && last == '|'){   // 一行结束
                        clFlag = true;
                        if(!line.empty())
                            v.push_back(line);
                        line.clear();
                        break;
                    }else if(c == '|'){    // 一个内容结束
                        bool empty = true;
                        for(int i = 0; i < s.size(); i++)
                            if(s[i] != '-'){
                                empty = false;
                                break;
                            }
                        if(!empty)
                            line.push_back(s);
                        s = "";
                        last = c;
                        break;
                    }else if(c == '#'){    // 一个表结束
                        clFlag = true;
                        endFlag = true;
                        if(!line.empty())
                            v.push_back(line);
                        m[name] = v;
                        v.clear();
                        line.clear();
                        break;
                    }
                    s += c;
                    last = c;
                }
            }
        }
    }
    fin.close();
    genFsTable();
    genDeployTable();
    printFsTable();
    printDeployTable();
}

MDReader::~MDReader(){

}

void MDReader::printM(){
    for(auto i : m){
        std::cout << i.first << ":" << std::endl;
        for(int j = 0; j < i.second.size(); j++){
            for(int k = 0; k < i.second[0].size(); k++){
                std::cout << i.second[j][k] << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void MDReader::printFsTable(){
    for(auto i : Fs_table){
        std::cout << "Function Name: " << std::get<0>(i) << "  pre_Qs: ";
        for(auto j : std::get<1>(i))
            std::cout << j << "  ";
        std::cout << "next_Qs: ";
        for(auto j : std::get<2>(i))
            std::cout << j << "  ";
        std::cout << "Params: ";
        for(auto j : std::get<3>(i))
            std::cout << j << "  ";
        std::cout << std::endl;
    }
}

void MDReader::printDeployTable(){
    for(auto i : D_table){
        std::cout << "lcore id: " << i.first << " functions: ";
        for(auto j : i.second)
            std::cout << j << "  ";
        std::cout << std::endl;
    }
}

void MDReader::genFsTable(){
    assert(m["tasks"].size() >= 1);
    // gen function table
    for(int i = 1; i < m["tasks"].size(); i++){
        std::vector<int> pre_Qs, next_Qs;
        std::string function_name = "";
        std::vector<std::string> params;
        /** pre_Qs **/
        if(m["tasks"][i][1] == "None"){
            // 输入队列组为空
        }else{
            int id = 0;
            for(int j = 0; j < m["tasks"][i][1].size(); j++){
                if(m["tasks"][i][1][j] >= '0' && m["tasks"][i][1][j] <= '9'){
                    id *= 10;
                    id += m["tasks"][i][1][j] - '0';
                }else if(m["tasks"][i][1][j] == ','){
                    pre_Qs.push_back(id);
                    id = 0;
                }
            }
            pre_Qs.push_back(id);
        }
        /** next_Qs **/
        if(m["tasks"][i][2] == "None"){
            // 输出队列组为空
        }else{
            int id = 0;
            for(int j = 0; j < m["tasks"][i][2].size(); j++){
                if(m["tasks"][i][2][j] >= '0' && m["tasks"][i][2][j] <= '9'){
                    id *= 10;
                    id += m["tasks"][i][2][j] - '0';
                }else if(m["tasks"][i][2][j] == ','){
                    next_Qs.push_back(id);
                    id = 0;
                }
            }
            next_Qs.push_back(id);
        }
        /** Function name **/
        function_name = m["tasks"][i][3];
        /** param **/
        std::string cur = "";
        for(int j = 0; j < m["tasks"][i][4].size(); j++){
            if(m["tasks"][i][4][j] == ','){
                if(cur != ""){
                    if(cur != "" && cur != "None")
                        params.push_back(cur);
                    cur = "";
                }
            }else
                cur += m["tasks"][i][4][j];
        }
        if(cur != "" && cur != "None")
            params.push_back(cur);
        /** add to FsTable */
        Fs_table.push_back(std::make_tuple(function_name, pre_Qs, next_Qs, params));
    }
}

void MDReader::genDeployTable(){
    assert(m["deploy"].size() >= 1);
    for(int i = 1; i < m["deploy"].size(); i++){
        int lcore_id = atoi(m["deploy"][i][0].c_str());
        std::vector<int> tasks_id_vec;
        int task_id = 0;
        for(int j = 0; j < m["deploy"][i][1].size(); j++){
            if(m["deploy"][i][1][j] >= '0' && m["deploy"][i][1][j] <= '9'){
                task_id *= 10;
                task_id += m["deploy"][i][1][j] - '0';
            }else if(m["deploy"][i][1][j] == ','){
                tasks_id_vec.push_back(task_id);
                task_id = 0;
            }
        }
        tasks_id_vec.push_back(task_id);
        D_table[lcore_id] = tasks_id_vec;
    }
}

std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>> MDReader::getFsTable(){
    return Fs_table;
}

std::map<int, std::vector<int>> MDReader::getDeployTable(){
    return D_table;
}

};