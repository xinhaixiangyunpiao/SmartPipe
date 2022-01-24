#ifndef _MD_READER_H_
#define _MD_READER_H_

#include <vector>
#include <map>
#include <unordered_map>

namespace sp{

class MDReader{
private:
    std::string path;
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> m; // 原始数据
    std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>> Fs_table; // 得到F表 <Function_name, type, pre_Qs, next_Qs, params>
    std::map<int, std::vector<int>> D_table;
public:
    MDReader(std::string path);
    ~MDReader();
    void genDeployTable();
    void genFsTable();
    std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>> getFsTable();
    std::map<int, std::vector<int>> getDeployTable();
    void printM();
    void printFsTable();
    void printDeployTable();
};

};

#endif