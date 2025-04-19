/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: Resolving deadlocks by preempting an intersection in one of the active trains.
*/

#include "deadlock_recovery.hpp"

void deadlockRecovery(map<string, Train*>& trains,
    unordered_map<string, Intersection*>& intersections,
    map<string, vector<string>>& resourceGraph) {
    
    writeLog logger;


    // Check if the cycle is empty
    if (cycle.empty()) {
        logger.log("SERVER", "Deadlock recovery invoked, but no cycle detected.");
        return;
    }
    
    string cycleStr;
    for (size_t i = 0; i < cycle.size(); ++i) {
        cycleStr += cycle[i];
        if (i < cycle.size() - 1) cycleStr += " ↔ "; // Shows relationship between trains in a cycle
    }

    logger.logDeadlockDetected(cycleStr);

    // Preempt the first train
    string preemptTrainName = cycle[0];
    Train* preemptTrain = trains[preemptTrainName];
    Intersection* currentIntersection = preemptTrain->current_location;

    if (currentIntersection) {
        string intersectionName = currentIntersection->name;

        logger.logPreemption(preemptTrainName, intersectionName);

        // Release intersection
        if (currentIntersection->release(preemptTrain)) {
            logger.logRelease(preemptTrainName, intersectionName);
            auto& holders = resourceTable[intersectionName];
            holders.erase(remove(holders.begin(), holders.end(), preemptTrainName), holders.end());
        }
    } else {
        logger.log("SERVER", "Preempted train has no current location. Skipping release.");
    }
}

