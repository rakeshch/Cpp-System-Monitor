#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "helper.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memtotal = Helper::GetValuebyKey<int>(kMeminfoFilename, memTotal);
  float memfree = Helper::GetValuebyKey<int>(kMeminfoFilename, memFree);
  return (memtotal - memfree)/ memtotal;   
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime = 0;
  uptime = Helper::GetValuebyName<long>(kUptimeFilename); 
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cputimes = LinuxParser::CpuUtilization();
  long totalTime = 0;
  for (unsigned int i = 0; i < cputimes.size(); i++) {
    if (i == LinuxParser::CPUStates::kGuest_ || i == LinuxParser::CPUStates::kGuestNice_) {
      totalTime -= stol(cputimes[i]);
    } else {
      totalTime += stol(cputimes[i]);
    }
  }
  return totalTime;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  std::vector<string> cputimes = LinuxParser::ReadCPUtimes(pid);
  float utime_ = std::stof(cputimes[13]);
  float stime_ = std::stof(cputimes[14]);
  float cutime_ = std::stof(cputimes[15]);
  float cstime_ = std::stof(cputimes[16]);

  return utime_ + stime_ + cutime_ + cstime_; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cputimes = LinuxParser::CpuUtilization();
  return stol(cputimes[LinuxParser::CPUStates::kIdle_]) + stol(cputimes[LinuxParser::CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
    string line, key, value;
    vector<string> cputimes;

    std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        linestream >> key;
        if (key == "cpu") {
          while (linestream >> value) {
            cputimes.push_back(value);
          }
        }
      }
    }
    return cputimes;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  return Helper::GetValuebyKey<int>(kStatFilename, processes); 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  return Helper::GetValuebyKey<int>(kStatFilename, runningProcesses);  
}

// Read and return the command associated with a process
// /proc/[pid]/cmdline
string LinuxParser::Command(int pid) { 
  return Helper::GetValuebyName<string>(to_string(pid) + kCmdlineFilename);
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  long ramsize = Helper::GetValuebyKey<long>(to_string(pid) + LinuxParser::kStatusFilename, vmSize);
  return to_string(ramsize/1024);
}

// Read and return the user ID associated with a process
// /proc/<<pid>>/status
int LinuxParser::Uid(int pid) { 
  return Helper::GetValuebyKey<int>((to_string(pid) + kStatusFilename), uid); 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  int userid = Uid(pid);

  //Read username from /etc/passwd based on the userid from previous step
  // info stored as "username:x:userid:...."
  string line, username, x;
  int streamuid;

  std::ifstream filestream(kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> username >> x >> streamuid){
        if(streamuid == userid)
          return username;
      }
    }
  }
  return username; 
}

// Read all CPU times into a vector - index from 0
std::vector<string> LinuxParser::ReadCPUtimes(int pid){
    string line;

    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> beg(linestream), end;
    std::vector<string> cputimes(beg, end);
    return cputimes;
}

