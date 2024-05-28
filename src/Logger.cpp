#include "Logger.h"
#include <iostream>
#include <ctime>
#include <string>

#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define RESET   "\033[0m"      

std::string Logger::monthStr;
std::string Logger::minStr;
std::string Logger::secStr;
std::string Logger::hourStr;
std::vector<LogEntry> Logger::messages;

namespace{
void formatTime(tm* ltm, std::string& monthStr, std::string& hourStr, std::string& minStr, std::string& secStr){

    std::string month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    monthStr = month[ltm->tm_mon];

    int hour = ltm->tm_hour;
    if(hour < 10){
        hourStr = "0" + std::to_string(hour);
    } else {
        hourStr = std::to_string(hour);
    }

    if(ltm->tm_min < 10){
        minStr = "0" + std::to_string(ltm->tm_min);
    } else {
        minStr = std::to_string(ltm->tm_min);
    }

    if(ltm->tm_sec < 10){
        secStr = "0" + std::to_string(ltm->tm_sec);
    } else {
        secStr = std::to_string(ltm->tm_sec);
    }
}
}

void Logger::Log(const std::string& message){
    LogEntry entry;
    entry.type = LOG_INFO;
    
    time_t now = time(0);
    tm* ltm = localtime(&now);  
    formatTime(ltm, monthStr, hourStr, minStr, secStr);
    entry.message = "LOG: [" + std::to_string(ltm->tm_mday)  + "/" + monthStr + "/" + std::to_string( 1900 + ltm->tm_year) + " " + hourStr + ":" + 
    minStr + ":" + secStr + "]" + message ;


    std::cout << GREEN << entry.message << RESET << std::endl;

    messages.push_back(entry);
}

void Logger::Err(const std::string& message){
    LogEntry entry;
    entry.type = LOG_ERROR;
    
    time_t now = time(0);
    tm* ltm = localtime(&now);  
    formatTime(ltm, monthStr, hourStr, minStr, secStr);
    entry.message = "ERROR: [" + std::to_string(ltm->tm_mday)  + "/" + monthStr + "/" + std::to_string( 1900 + ltm->tm_year) + " " + hourStr + ":" + 
    minStr + ":" + secStr + "]" + message ;


    std::cout << RED << entry.message << RESET << std::endl;

    messages.push_back(entry);      

}