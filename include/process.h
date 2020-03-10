#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int);
  int Pid();                               // See src/process.cpp
  std::string User();                      // See src/process.cpp
  std::string Command();                   // See src/process.cpp
  float CpuUtilization();                  // See src/process.cpp
  std::string Ram();                       // See src/process.cpp
  long int UpTime();                       // See src/process.cpp
  bool operator<(Process const& a) const;  // See src/process.cpp

 private:
  int pid_;
  float utime_{0.0}; //#14 from /proc/[PID]/stat
  float stime_ {0.0}; //#15 from /proc/[PID]/stat
  float cutime_{0.0}; //#16 from /proc/[PID]/stat
  float cstime_{0.0}; //#17 from /proc/[PID]/stat
  float starttime_{0.0}; //#22 from /proc/[PID]/stat
  long Hertz_;
  float cpuUtilization_{0.0};
};

#endif