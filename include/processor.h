#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

using std::vector;

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

 private:
    vector<double> ReadCPUTimes();
    double prev_idle_time;
    double prev_total_time;
};

#endif