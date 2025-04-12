#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

class writeLog {
public:
    static void log(const std::string& source, const std::string& message);
    static void logTrainRequest(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logGrant(const std::string& trainLetter, const std::string& intersectionLetter, const std::string& additionalMessage = "");
    static void logLock(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logIntersectionFull(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logDeadlockDetected(const std::string& cycle);
    static void logRelease(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logPreemption(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logGrantAfterPreemption(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logProceeding(const std::string& trainLetter, const std::string& intersectionLetter);
    static void logSimulationComplete();


private:
    static std::string getCurrentTime();
};

#endif
