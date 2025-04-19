/*
Group B
Author: Caden Blust
Email: caden.blust@okstate.edu
Date: 4/12/2025

Description: Reads requests and responses sent between server and trains to write to a simulation.log file.
             Keeps track of simulated time and deadlock resolution steps.
*/

#include "logging.hpp"

using namespace std;

std::ofstream loggingFile("logging.txt");
// ^ Wasn't sure where to put the loggingFile.close();
// ^ May need to move std::ofstream loggingFile("logging.txt"); to a different file

// TODO: combine logging events to one function (Granting, Waiting, Denying requests)

// TODO: make a logging function called after parsing intersections and when every train has completed its route

// TODO: rework each to take sim_time and convert to HH:MM:SS] timestamp\


//==========================================================================================
// INITIAL LOG
    void writeLog::log(const std::string& source, const std::string& message) {
        std::string timestamp = getCurrentTime();
        loggingFile << "[" << timestamp << "] " << source << ": " << message << std::endl;
    }

//==========================================================================================


    // TRAIN AND INTERSECTION REQUEST
    void writeLog::logTrainRequest(const std::string& trainLetter, const std::string& intersectionLetter) {
        std::string timestamp = getCurrentTime();
        loggingFile << "[" << timestamp << "] TRAIN" << trainLetter << ": Sent ACQUIRE request for Intersection" << intersectionLetter << "." << std::endl;
    }

//==========================================================================================

    //GRANT INTERSECTION
    //NOTE: ADDITIONAL MESSAGE IS FOR IF WE WANT TO ADD ANY ADDITIONAL DETAILS, SUCH AS A SEMAPHORE COUNT
    void writeLog::logGrant(const std::string& trainLetter, const std::string& intersectionLetter, const std::string& additionalMessage) {
        std::string timestamp = getCurrentTime();
        loggingFile << "[" << timestamp << "] SERVER: GRANTED Intersection" << intersectionLetter << " to Train" << trainLetter;

        if (!additionalMessage.empty()) {
            loggingFile << " " << additionalMessage;
        }

        loggingFile << std::endl;
    }

//==========================================================================================
    //ADD TO WAIT QUEUE
    void writeLog::logLock(const std::string& trainLetter, const std::string& intersectionLetter) {
        std::string timestamp = getCurrentTime();
        loggingFile << "[" << timestamp << "] SERVER: Intersection" << intersectionLetter << " is locked. Train" << trainLetter << " added to wait queue." << std::endl;
    }


//==========================================================================================

	// INTERSECTION FULL LOG AKA other ADD TO WAIT QUEUE
	void writeLog::logIntersectionFull(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	loggingFile << "[" << timestamp << "] SERVER: Intersection" << intersectionLetter << " is full. Train" << trainLetter << " added to wait queue." << std::endl;
	}

//==========================================================================================

    // DEADLOCK DETECTION LOG
    // NOTE: cycle is for what trains are at a deadlock. cycle Example: "Train1 ↔ Train3". This could also be changed to take both trains instead of a "cycle".
	void writeLog::logDeadlockDetected(const std::string& cycle) {
    	std::string timestamp = getCurrentTime();
    	loggingFile << "[" << timestamp << "] SERVER: Deadlock detected! Cycle: " << cycle << "." << std::endl;
	}


//==========================================================================================

	// RELEASE INTERSECTION
	void writeLog::logRelease(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	loggingFile("logging.txt"); << "[" << timestamp << "] TRAIN" << trainLetter << ": Released Intersection" << intersectionLetter << "." << std::endl;
	}

//==========================================================================================

	// DEADLOCK RESOLUTION LOG (PREEMPTION) AKA the other release
	void writeLog::logPreemption(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	loggingFile << "[" << timestamp << "] SERVER: Preempting Intersection" << intersectionLetter << " from Train" << trainLetter << "." << std::endl;
    	loggingFile << "[" << timestamp << "] SERVER: Train" << trainLetter << " released Intersection" << intersectionLetter << " forcibly." << std::endl;
	}


//==========================================================================================

	// GRANT INTERSECTION AFTER PREEMPTION
	void writeLog::logGrantAfterPreemption(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	loggingFile << "[" << timestamp << "] SERVER: GRANTED Intersection" << intersectionLetter << " to Train" << trainLetter << "." << std::endl;
	}


//==========================================================================================

	// TRAIN PROCEEDING LOG
	void writeLog::logProceeding(const std::string& trainLetter, const std::string& intersectionLetter) {
    	std::string timestamp = getCurrentTime();
    	loggingFile << "[" << timestamp << "] TRAIN" << trainLetter << ": Acquired Intersection" << intersectionLetter << ". Proceeding..." << std::endl;
	}

//==========================================================================================

	// SIMULATION COMPLETE LOG
	void writeLog::logSimulationComplete() {
    	std::string timestamp = getCurrentTime();
    	loggingFile << "[" << timestamp << "] SIMULATION COMPLETE. All trains reached destinations." << std::endl;
	}

//==========================================================================================

    void writeLog::getCurrentTime() {

        auto now = system_clock::now();
		int hours: sim_time / 3600
		int minutes: (sim_time % 3600) / 60
		int seconds: sim_time % 60
		cout << hours:minutes:seconds
	}

