#ifndef _PROFILE_H_
#define _PROFILE_H_

#include <string>
#include <vector>
#include <sys/time.h>
#include <map>
#include "executor.h"

namespace sp{

struct cmp {
	bool operator()(const struct timeval &lhs, const struct timeval &rhs) const{
        return (lhs.tv_sec == rhs.tv_sec)?(lhs.tv_usec < rhs.tv_usec):(lhs.tv_sec < rhs.tv_sec);
	}
};

class Profile{
private:
    // 原始数据：executor, data
    std::map<short, std::vector<std::tuple<short, std::string, struct timeval>>> executor_lifecycle_detail_map; // map记录每个executor的执行时间点 <第几次执行， 执行标识， 时间戳>
    std::map<short, std::vector<std::tuple<short, std::string, struct timeval>>> data_lifecycle_detail_map; // map记录每个data的声明周期时间节点 <executor_id， 标识， 时间戳>
    
    // 初步处理
    std::map<short, std::vector<std::tuple<double, double, double>>> executor_detail; // 每个executor <接收时间， 处理时间， 发送时间>
    std::map<short, std::vector<std::pair<std::string, double>>> data_detail;     // 每个data <做什么， 花费时间>

    // 统计量
    std::map<short, std::tuple<double, double, double>> executor_avg; // 每个executor属性平均值 <接收时间， 处理时间， 发送时间>
    std::map<short, std::tuple<double, double, double>> executor_std; // 每个executor属性标准差 <接收时间， 处理时间， 发送时间>
    std::map<short, std::tuple<double, double, double>> executor_max; // 每个executor属性最大值 <接收时间， 处理时间， 发送时间>
    std::map<short, std::tuple<double, double, double>> executor_min; // 每个executor属性最小值 <接收时间， 处理时间， 发送时间>
    std::vector<std::pair<std::string, double>> data_avg;         // 每个data属性平均值 <做什么， 花费时间>
    std::vector<std::pair<std::string, double>> data_std;         // 每个data属性标准差 <做什么， 花费时间>
    std::vector<std::pair<std::string, double>> data_max;         // 每个data属性最大值 <做什么， 花费时间>
    std::vector<std::pair<std::string, double>> data_min;         // 每个data属性最小值 <做什么， 花费时间>
    double delay_avg;  // 总时延平均值
    double delay_max;  // 总时延最大值
    double delay_min;  // 总时延最小值

public:
    Profile();
    ~Profile();
    void update_executor_lifecycle_detail_map(short executor_id, short cnt, std::string name, struct timeval timestamp);
    void update_data_lifecycle_detail_map(short data_id, short executor_id, std::string name, struct timeval timestamp);
    void cal();
    void print_detail();
    void print_statistics();
    void export_executor_json();
    void export_data_json();
    void profile();
};  

};

#endif