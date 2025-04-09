/*
Group B
Author: Caden Blust
Email: caden.blust@okstate.edu
Date: 4/5

Description: Reads requests and responses sent between server and trains to write to a simulation.log file.
             Keeps track of simulated time and deadlock resolution steps.
*/

#include "logging.hpp"

using namespace std;

//==========================================================================================
// INITIAL LOG
    void writeLog::log(const std::string& source, const std::string& message) {
        std::string timestamp = getCurrentTime();
        std::cout << "[" << timestamp << "] " << source << ": " << message << std::endl;
    }

//==========================================================================================

    // TRAIN AND INTERSECTION REQUEST
    void writeLog::logTrainRequest(const std::string& trainLetter, const std::string& intersectionLetter) {
        std::string timestamp = getCurrentTime();
        std::cout << "[" << timestamp << "] TRAIN" << trainLetter
                  << ": Sent ACQUIRE request for Intersection" << intersectionLetter << "." << std::endl;
    }

//==========================================================================================

    //GRANT INTERSECTION
    //NOTE: ADDITIONAL MESSAGE IS FOR IF WE WANT TO ADD ANY ADDITIONAL DETAILS, SUCH AS A SEMAPHORE COUNT
    void writeLog::logGrant(const std::string& trainLetter, const std::string& intersectionLetter, const std::string& additionalMessage) {
        std::string timestamp = getCurrentTime();
        std::cout << "[" << timestamp << "] SERVER: GRANTED Intersection"
                  << intersectionLetter << " to Train" << trainLetter;

        if (!additionalMessage.empty()) {
            std::cout << " " << additionalMessage;
        }

        std::cout << std::endl;
    }

//==========================================================================================
    //ADD TO WAIT QUEUE
    void writeLog::logLock(const std::string& trainLetter, const std::string& intersectionLetter) {
        std::string timestamp = getCurrentTime();
        std::cout << "[" << timestamp << "] SERVER: Intersection"
                  << intersectionLetter << " is locked. Train" << trainLetter << " added to wait queue." << std::endl;
    }


//==========================================================================================

	// INTERSECTION FULL LOG AKA other ADD TO WAIT QUEUE
	void writeLog::logIntersectionFull(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] SERVER: Intersection"
              	<< intersectionLetter << " is full. Train" << trainLetter << " added to wait queue." << std::endl;
	}

//==========================================================================================

    // DEADLOCK DETECTION LOG
    // NOTE: cycle is for what trains are at a deadlock. cycle Example: "Train1 ↔ Train3". This could also be changed to take both trains instead of a "cycle".
	void writeLog::logDeadlockDetected(const std::string& cycle) {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] SERVER: Deadlock detected! Cycle: " << cycle << "." << std::endl;
	}


//==========================================================================================

	// RELEASE INTERSECTION
	void writeLog::logRelease(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] TRAIN" << trainLetter
              	<< ": Released Intersection" << intersectionLetter << "." << std::endl;
	}

//==========================================================================================

	// DEADLOCK RESOLUTION LOG (PREEMPTION) AKA the other release
	void writeLog::logPreemption(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] SERVER: Preempting Intersection"
              << intersectionLetter << " from Train" << trainLetter << "." << std::endl;
    	std::cout << "[" << timestamp << "] SERVER: Train" << trainLetter << " released Intersection"
              << intersectionLetter << " forcibly." << std::endl;
	}


//==========================================================================================

	// GRANT INTERSECTION AFTER PREEMPTION
	void writeLog::logGrantAfterPreemption(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] SERVER: GRANTED Intersection"
              	<< intersectionLetter << " to Train" << trainLetter << "." << std::endl;
	}


//==========================================================================================

	// TRAIN PROCEEDING LOG
	void writeLog::logProceeding(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] TRAIN" << trainLetter
              	<< ": Acquired Intersection" << intersectionLetter << ". Proceeding..." << std::endl;
	}

//==========================================================================================

	// SIMULATION COMPLETE LOG
	void writeLog::logSimulationComplete() {
    	std::string timestamp = getCurrentTime();
    	std::cout << "[" << timestamp << "] SIMULATION COMPLETE. All trains reached destinations." << std::endl;
	}

//==========================================================================================


    std::string writeLog::getCurrentTime() {
        using namespace std::chrono;
        using std::chrono::seconds;
        using std::chrono::microseconds;


    //GET TIME
        auto now = system_clock::now();
        auto duration = now.time_since_epoch();
        auto sec = duration_cast<seconds>(duration);
        auto microsec = duration_cast<microseconds>(duration % seconds(1));

        std::time_t t = system_clock::to_time_t(now); //TAKE SYSTEM TIME INTO A VARIABLE
        std::tm tm = *std::localtime(&t); // BREAKS DOWN TIME INTO HOURS, MINUTES, SECONDS, MS.

        std::ostringstream timeStream;
        timeStream << std::put_time(&tm, "%H:%M:%S") << "." << microsec.count();
        return timeStream.str();
    }
