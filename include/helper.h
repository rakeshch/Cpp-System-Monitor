#ifndef PARSER_HELPER_H
#define PARSER_HELPER_H

#include <fstream>
#include <string>

#include "linux_parser.h"

using std::string;

namespace Helper{

    template<typename T>
    T GetValuebyKey(string const &filename, string const searchstring){
        T value;
        string line, key;

        std::ifstream stream(LinuxParser::kProcDirectory + filename);
        if(stream.is_open()){
            while(std::getline(stream, line)){
                std::istringstream linestream(line);
                while(linestream >> key >> value){
                    if(key == searchstring)
                    return value;
                }   
            }
        }
        return value;
    };

    template<typename T>
    T GetValuebyName(string const &filename){
        T value;
        string line;

        std::ifstream stream(LinuxParser::kProcDirectory + filename);
        if(stream.is_open()){
            std::getline(stream, line);
            std::istringstream linestream(line);
            linestream >> value; 
        }
        return value;
    };
}

#endif