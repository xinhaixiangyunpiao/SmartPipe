#include <iomanip>
#include <climits>
#include <fstream>
#include <algorithm>
#include "profile.h"
#include "util.h"

namespace sp{

Profile::Profile(){
    
}

Profile::~Profile(){

}

void Profile::update_executor_lifecycle_detail_map(short executor_id, short cnt, std::string name, struct timeval timestamp){
    executor_lifecycle_detail_map[executor_id].push_back(std::make_tuple(cnt, name, timestamp));
}

void Profile::update_data_lifecycle_detail_map(short data_id, short executor_id, std::string name, struct timeval timestamp){
    data_lifecycle_detail_map[data_id].push_back(std::make_tuple(executor_id, name, timestamp));
}

void Profile::cal(){
    // calculate detail
    for(auto i : executor_lifecycle_detail_map){
        double time_start_recv = 0.0;
        double time_start_process = 0.0;
        double time_start_send = 0.0;
        double time_end_send = 0.0;
        for(int j = 0; j < i.second.size(); j++){
            if(j%4 == 0)
                time_start_recv = (std::get<2>(i.second[j]).tv_sec + (double)std::get<2>(i.second[j]).tv_usec/1000000)*1000; 
            else if(j%4 == 1){
                time_start_process = (std::get<2>(i.second[j]).tv_sec + (double)std::get<2>(i.second[j]).tv_usec/1000000)*1000;
            }else if(j%4 == 2){
                time_start_send = (std::get<2>(i.second[j]).tv_sec + (double)std::get<2>(i.second[j]).tv_usec/1000000)*1000;
            }else if(j%4 == 3){
                time_end_send = (std::get<2>(i.second[j]).tv_sec + (double)std::get<2>(i.second[j]).tv_usec/1000000)*1000;
                executor_detail[i.first].push_back(std::make_tuple(time_start_process-time_start_recv, time_start_send-time_start_process, time_end_send-time_start_send));
            }
        }
    }
    for(auto i : data_lifecycle_detail_map){
        double time_start = (std::get<2>(i.second[0]).tv_sec + (double)std::get<2>(i.second[0]).tv_usec/1000000)*1000;
        double time_cur = 0.0;
        for(int j = 1; j < i.second.size(); j++){
            time_cur = (std::get<2>(i.second[j]).tv_sec + (double)std::get<2>(i.second[j]).tv_usec/1000000)*1000;
            std::string name = "unknow";
            if(j%2 == 1){
                // 传输
                std::string last_str = std::get<1>(i.second[j-1]);
                std::string cur_str = std::get<1>(i.second[j]);
                name = last_str.erase(0, 6) + " convey to " + cur_str.erase(0, 7);
            }else{
                // 处理
                std::string cur_str = std::get<1>(i.second[j]);
                name = cur_str.erase(0, 6);
            }
            data_detail[i.first].push_back(std::make_pair(name, time_cur - time_start));
            time_start = time_cur;
        }
    }
    // calculate statistics
    // avg, max, min
    for(auto i : executor_detail){
        double time_recv_sum = 0.0;
        double time_process_sum = 0.0;
        double time_send_sum = 0.0;
        double time_recv_max = 0.0;
        double time_process_max = 0.0;
        double time_send_max = 0.0;
        double time_recv_min = DBL_MAX;
        double time_process_min = DBL_MAX;
        double time_send_min = DBL_MAX;
        for(auto j : i.second){
            time_recv_sum += std::get<0>(j);
            time_process_sum += std::get<1>(j);
            time_send_sum += std::get<2>(j);
            time_recv_max = std::max(time_recv_max, std::get<0>(j));
            time_process_max = std::max(time_process_max, std::get<1>(j));
            time_send_max = std::max(time_send_max, std::get<2>(j));
            time_recv_min = std::min(time_recv_min, std::get<0>(j));
            time_process_min = std::min(time_process_min, std::get<1>(j));
            time_send_min = std::min(time_send_min, std::get<2>(j));
        }
        int size = i.second.size();
        executor_avg[i.first] = std::make_tuple(time_recv_sum/size, time_process_sum/size, time_send_sum/size);
        executor_max[i.first] = std::make_tuple(time_recv_max, time_process_max, time_send_max);
        executor_min[i.first] = std::make_tuple(time_recv_min, time_process_min, time_send_min);
    }
    assert(data_detail.find(0) != data_detail.end());
    int executor_num = data_detail[0].size();
    delay_max = 0;
    delay_min = DBL_MAX;
    double delay_sum = 0.0;
    double delay_cur = 0.0;
    for(int i = 0; i < executor_num; i++){
        std::string name = data_detail[0][i].first;
        double sum = 0.0;
        double time_cost_max = 0.0;
        double time_cost_min = DBL_MAX;
        for(auto j : data_detail){
            sum += j.second[i].second;
            time_cost_max = std::max(time_cost_max, j.second[i].second);
            time_cost_min = std::min(time_cost_min, j.second[i].second);
        }
        data_avg.push_back(std::make_pair(name, sum/data_detail.size()));
        data_max.push_back(std::make_pair(name, time_cost_max));
        data_min.push_back(std::make_pair(name, time_cost_min));
    }
    // delay
    for(auto i : data_detail){
        double delay_cur = 0.0;
        for(auto j : i.second){
            delay_cur += j.second;
        }
        delay_sum += delay_cur;
        delay_max = std::max(delay_max, delay_cur);
        delay_min = std::min(delay_min, delay_cur);
    }
    delay_avg = delay_sum/data_detail.size();
    // std
    
}

void Profile::print_detail(){
    std::cout << "profile executor size: " << executor_detail.size() << std::endl;
    for(auto i : executor_detail){
        std::cout << "executor " << i.first << ": " << std::endl;
        for(auto j : i.second){
            std::cout << "   time recv: " << std::get<0>(j) << " ms" << std::endl;
            std::cout << "   time process: " << std::get<1>(j) << " ms" << std::endl;
            std::cout << "   time send: " << std::get<2>(j) << " ms" << std::endl;
        }
    }
    std::cout << "profile data size: " << data_detail.size() << std::endl; 
    for(auto i : data_detail){
        std::cout << "data " << i.first << ": " << std::endl;
        for(auto j : i.second){
            std::cout << "   " << std::get<0>(j) << ": " << std::get<1>(j) << " ms" << std::endl;
        }
    }
}

void Profile::print_statistics(){
std::cout << "profile executor number: " << executor_avg.size() << std::endl;
    for(auto i : executor_avg){
        std::cout << "executor " << i.first << ": " << std::endl;
        std::cout << "   time recv avg: " << std::get<0>(i.second) << " ms" << std::endl;
        std::cout << "      max: " << std::get<0>(executor_max[i.first]) << " ms" << std::endl;
        std::cout << "      min: " << std::get<0>(executor_min[i.first]) << " ms" << std::endl;
        std::cout << "   time process avg: " << std::get<1>(i.second) << " ms" << std::endl;
        std::cout << "      max: " << std::get<1>(executor_max[i.first]) << " ms" << std::endl;
        std::cout << "      min: " << std::get<1>(executor_min[i.first]) << " ms" << std::endl;
        std::cout << "   time send avg: " << std::get<2>(i.second) << " ms" << std::endl;
        std::cout << "      max: " << std::get<2>(executor_max[i.first]) << " ms" << std::endl;
        std::cout << "      min: " << std::get<2>(executor_min[i.first]) << " ms" << std::endl;
    }
    std::cout << "data executor number: " << data_avg.size() << std::endl;
    for(int i = 0; i < data_avg.size(); i++){
        std::cout << "   " << data_avg[i].first << ": " << data_avg[i].second << " ms" << std::endl;
        std::cout << "      " << "max: " << data_max[i].second << " ms" << std::endl;
        std::cout << "      " << "min: " << data_min[i].second << " ms" << std::endl;
    }
    std::cout << "time delay avg: " << delay_avg << " ms" << std::endl;
    std::cout << "   max: " << delay_max << " ms" << std::endl;
    std::cout << "   min: " << delay_min << " ms" << std::endl;
}

void Profile::export_executor_json(){
    std::string outputfile = "../output/" + Util::currentDateToString() + "_executors.json";
    std::ofstream fout(outputfile);
    fout << "[";
    std::string pid = "Main";
    int ii = 0;
    for(auto i : executor_lifecycle_detail_map){
        std::string tid = std::to_string(i.first);
        for(int j = 0; j < i.second.size(); j++){
            int data_index = std::get<0>(i.second[j]);
            long before_recv, before_process, before_send, after_send;
            if(j%4 == 0){
                // 接收之前的时间戳
                before_recv = std::get<2>(i.second[j]).tv_sec*1000000+std::get<2>(i.second[j]).tv_usec;
            }else if(j%4 == 1){
                // 接收完毕，处理之前的时间戳
                before_process = std::get<2>(i.second[j]).tv_sec*1000000+std::get<2>(i.second[j]).tv_usec;
            }else if(j%4 == 2){
                // 处理之后，发送之前的时间戳
                before_send = std::get<2>(i.second[j]).tv_sec*1000000+std::get<2>(i.second[j]).tv_usec;
            }else if(j%4 == 3){
                // 发送之后的时间戳
                after_send = std::get<2>(i.second[j]).tv_sec*1000000+std::get<2>(i.second[j]).tv_usec;
                // 写入 处理过程
                fout << "{\"name\": \"" << "data " + std::to_string(data_index) + " process" << "\",\"ph\": \"X\", \"pid\": \"Main\", \"tid\": \"" << tid << "\", \"ts\": " << before_process << ", \"dur\": " << before_send - before_process << "}," << std::endl;
                // flow event
                if(ii == 0){
                    // 只往下传
                    fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"s\", \"id\": \"" << std::to_string(data_index) << "\", \"pid\": \"Main\", \"tid\": \"" << tid << "\", \"ts\": " << std::to_string(before_send) << "}," << std::endl;
                }else if(ii == executor_lifecycle_detail_map.size()-1){
                    // 只接上面的, 最后一个不加，
                    if(j == i.second.size()-1)
                        fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"f\", \"bp\": \"e\", \"id\": \"" << std::to_string(data_index) << "\", \"pid\": \"Main\", \"tid\": \"" << tid << "\", \"ts\": " << before_process << "}" << std::endl;
                    else
                        fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"f\", \"bp\": \"e\", \"id\": \"" << std::to_string(data_index) << "\", \"pid\": \"Main\", \"tid\": \"" << tid << "\", \"ts\": " << before_process << "}," << std::endl;
                }else{
                    // 承上启下
                    fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"s\", \"id\": \"" << std::to_string(data_index) << "\", \"pid\": \"Main\", \"tid\": \"" << tid << "\", \"ts\": " << before_send << "}," << std::endl;
                    fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"f\", \"bp\": \"e\", \"id\": \"" << std::to_string(data_index) << "\", \"pid\": \"Main\", \"tid\": \"" << tid << "\", \"ts\": " << before_process << "}," << std::endl;
                }
            }
        }
        ii++;
    }
    fout << "]";
    fout.close();
}

void Profile::export_data_json(){
    std::string outputfile = "../output/" + Util::currentDateToString() + "_datas.json";
    std::ofstream fout(outputfile);
    fout << "[";
    std::string pid = "Main";
    int ii = 0;
    for(auto i : data_lifecycle_detail_map){
        int data_id = i.first;
        for(int j = 0; j < i.second.size(); j++){
            long begin_time, end_time;
            if(j%2 == 0){
                // before process
                begin_time = std::get<2>(i.second[j]).tv_sec*1000000 + std::get<2>(i.second[j]).tv_usec;
            }else{
                // end process
                end_time = std::get<2>(i.second[j]).tv_sec*1000000 + std::get<2>(i.second[j]).tv_usec;
                int tid = std::get<0>(i.second[j]);
                std::string name_str = std::get<1>(i.second[j]);
                std::string name = name_str.erase(0, 6);
                // 接上一个
                if(j != 1){
                    fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"f\", \"bp\": \"e\", \"id\": " << data_id << ", \"pid\": \"Main\", \"tid\": " << tid << ", \"ts\": " << begin_time << ", \"cat\": \"transfer data\"}," << std::endl;
                }
                // process
                if(ii == data_lifecycle_detail_map.size()-2 && j == i.second.size()-1)
                    fout << "{\"name\": \"" << name << ":" << data_id << "\", \"ph\": \"X\", \"pid\": \"Main\", \"tid\": " << tid << ", \"ts\": " << begin_time << ", \"dur\": " << end_time - begin_time << "}" << std::endl;   
                else
                    fout << "{\"name\": \"" << name << ":" << data_id << "\", \"ph\": \"X\", \"pid\": \"Main\", \"tid\": " << tid << ", \"ts\": " << begin_time << ", \"dur\": " << end_time - begin_time << "}," << std::endl;   
                // 处理下一个
                if(j != i.second.size()-1){
                    fout << "{\"name\": \"" << "connect" << "\", \"ph\": \"s\", \"id\": " << data_id << ", \"pid\": \"Main\", \"tid\": " << tid << ", \"ts\": " << end_time << ", \"cat\": \"transfer data\"}," << std::endl;
                }
            }
        }
        ii++;
    }
    fout << "]" << std::endl;
}

void Profile::profile(){
    cal();
    std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(3);
    // print_detail();
    print_statistics();
    // export_executor_json();
    export_data_json();
}

};