#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

string ToString(int time) {
  string timeAsString = std::to_string(time);
  return string(2 - timeAsString.length(), '0') + timeAsString;
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int  hour = 0;
    int  min = 0;
    int  sec = 0;

    hour = seconds/3600;
    seconds = seconds%3600;
    min = seconds/60;
    seconds = seconds%60;
    sec = seconds;
    return ToString(hour) + ":"+ ToString(min) + ":" + ToString(sec); 
}