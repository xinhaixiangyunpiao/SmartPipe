#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

namespace sp{
class Record;

class Util{
public:
    static std::string currentDateToString();
    static void genBarChart(std::string title, std::unordered_map<std::string, double> data);
    static void ordinaryLeastSquares(std::vector<Record> records, int& k, int& b);
    static pid_t gettid();
};

};

#endif