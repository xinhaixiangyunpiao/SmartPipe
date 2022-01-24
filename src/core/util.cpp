#include <ctime>
#include "functions.h"
#include "util.h"

namespace sp{

std::string Util::currentDateToString()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", timeinfo);
	std::string str(buffer);
	return str;
}

void Util::genBarChart(std::string title, std::unordered_map<std::string, double> data){
    
}

void Util::ordinaryLeastSquares(std::vector<sp::Record> Records, int& k, int& b){

}

pid_t Util::gettid() {
	return syscall(SYS_gettid);
}

};