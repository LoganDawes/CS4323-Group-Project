#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

class writeLog {
public:
    ~writeLog();
    static void log(const std::string& source, const std::string& message, int sim_time = 0);
    static void logTrainRequest(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logGrant(const std::string& trainLetter, const std::string& intersectionLetter, const std::string& additionalMessage = "", int sim_time = 0);
    static void logLock(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logIntersectionFull(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logDeadlockDetected(const std::string& cycle, int sim_time = 0);
    static void logRelease(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logPreemption(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logGrantAfterPreemption(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logProceeding(const std::string& trainLetter, const std::string& intersectionLetter, int sim_time = 0);
    static void logSimulationComplete(int sim_time = 0);
    static std::string getCurrentTime(int sim_time = 0);
};

#endif
