#pragma once
#include <string>

class Logger{
    private:
        static std::string monthStr;
        static std::string hourStr;
        static std::string minStr;
        static std::string secStr;

      
    public:
        static void Log(const std::string& message);
        static void Err(const std::string& message);
        
        
};

