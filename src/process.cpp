#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
 #include <iostream>

#include "process.h"
#include "linux_parser.h"
#include "helper.h"
#include "format.h"

using std::string;
using std::to_string;

Process::Process(int pid): pid_(pid){
    std::vector<string> cputimes = LinuxParser::ReadCPUtimes(pid);
    //convert all strings to float and set cpu times
    utime_ = std::stof(cputimes[13]);
    stime_ = std::stof(cputimes[14]);
    cutime_ = std::stof(cputimes[15]);
    cstime_ = std::stof(cputimes[16]);
    starttime_ = std::stof(cputimes[21]);
    Hertz_ = sysconf(_SC_CLK_TCK);

    cpuUtilization_ = CpuUtilization();
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    // total time spent for the process (utime and stime), including child processes(cutime, cstime)
    float total_time = utime_ + stime_ + cutime_ + cstime_;
    // total elapsed time in seconds since process started
    float seconds = LinuxParser::UpTime() - (starttime_ / Hertz_);
    //finally, calculate cpu usuage 
    float cpu_usuage = ((total_time/ Hertz_)/ seconds);

    return cpu_usuage; 
}

// Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(Pid());
}

// Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(Pid());
}

// Return the user (name) that generated this process
string Process::User() {     
    return LinuxParser::User(Pid());
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    long seconds = LinuxParser::UpTime() - (starttime_/ Hertz_);
    return seconds;
}

// Sort by CPU utilization
bool Process::operator<(Process const& a) const {
  return  a.cpuUtilization_ < this->cpuUtilization_;
}