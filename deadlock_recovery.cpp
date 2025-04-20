/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: Resolving deadlocks by preempting an intersection in one of the active trains.
*/

#include "deadlock_recovery.hpp"

void deadlockRecovery(unordered_map<string, Train*>& trains,
    unordered_map<string, vector<string>>& resourceGraph,
    const vector<string>& cycle, int sim_time) {
    
    writeLog logger;


    // Check if the cycle is empty
    if (cycle.empty) {
        logger.log("SERVER", "Deadlock recovery invoked, but no cycle detected.", sim_time);
        return;
    }
    
    // Make a string to log that shows the relationships between trains stuck in a cycle
    string cycleString;
    for (size_t i = 0; i < cycle.size(); ++i) {
        cycleString += cycle[i];
        if (i < cycle.size() - 1) cycleString += " : "; 
    }

    logger.logDeadlockDetected(cycleString, sim_time);

    /* Preempting train logic, releases it early so that another train can take its place and 
    the cycle is broken*/
    
    string preemptTrainName = cycle[0]; // Get the name of the first train in the cycle
    Train* preemptTrain = trains[preemptTrainName]; // Find that train in the vector
    Intersection* currentIntersection = preemptTrain->current_location; // Find the intersection it's in 

    if (currentIntersection) { // If the current intersection is indeed a thing
        string intersectionName = currentIntersection->name; //Store the name of the intersection so it can be logged
        // Log that you're preempting a train
        logger.logPreemption(preemptTrainName, intersectionName, sim_time);

        // Release intersection
        if (currentIntersection->release(preemptTrain)) {
            logger.logRelease(preemptTrainName, intersectionName, sim_time);
            auto& holders = resourceGraph[intersectionName];
            holders.erase(remove(holders.begin(), holders.end(), preemptTrainName), holders.end());
        }
    } else {
        logger.log("SERVER", "Preempted train has no current location. Skipping release.", sim_time);
    }
}

