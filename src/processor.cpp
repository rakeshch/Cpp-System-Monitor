#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <sstream>
#include <vector>
#include <numeric>

using std::string;
using std::vector;

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    const vector<double> cpu_times = ReadCPUTimes();
    if(cpu_times.size() < 4)
        return false;
    
    double idle_time = cpu_times[3];
    double total_time = std::accumulate(cpu_times.begin(), cpu_times.end(),0);

    const float idle_time_delta = idle_time - prev_idle_time;
    const float total_time_delta = total_time - prev_total_time;
    const float utilization = (1 - idle_time_delta/ total_time_delta);

    prev_idle_time = idle_time;
    prev_total_time = total_time;
    
    return utilization; 
}

// Read CPU data from "/proc/stat" first line (CPU) into a vector
vector<double> Processor::ReadCPUTimes(){
    string line, key;
    double time;

    vector<double> cpu_times;
    std::ifstream stream(LinuxParser::kProcDirectory+LinuxParser::kStatFilename);
    if(stream.is_open()){
        while(std::getline(stream, line)){
            std::istringstream linestream(line);
            while(linestream>> key){
                if(key == LinuxParser::cpu){
                    while(linestream >> time)
                        cpu_times.push_back(time);                
                }
            }        
        }
    }
    return cpu_times;
}