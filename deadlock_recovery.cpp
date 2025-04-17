/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: Resolving deadlocks by preempting an intersection in one of the active trains.
*/

#include "deadlock_recovery.hpp"

vector<string> getDeadlockCycle(const map<string, vector<string>> &resourceTable) {
    unordered_map<string, bool> visited;
    unordered_map<string, bool> inStack;
    vector<string> path;
    vector<string> cycle;

    function<bool(string)> dfs = [&](string node) -> bool {
        visited[node] = true;
        inStack[node] = true;
        path.push_back(node);

        if (resourceTable.find(node) != resourceTable.end()) {
            for (const string& neighbor : resourceTable.at(node)) {
                if (!visited[neighbor] && dfs(neighbor)) {
                    return true;
                } else if (inStack[neighbor]) {
                    // Found a cycle: extract cycle from path
                    auto it = find(path.begin(), path.end(), neighbor);
                    if (it != path.end()) {
                        cycle.assign(it, path.end());
                    }
                    return true;
                }
            }
        }

        path.pop_back();
        inStack[node] = false;
        return false;
    };

    for (const auto& [node, _] : resourceTable) {
        if (!visited[node] && dfs(node)) break;
    }

    return cycle;
}

void deadlockRecovery(map<string, Train*>& trains,
    unordered_map<string, Intersection*>& intersections,
    map<string, vector<string>>& resourceTable) {
    
    writeLog logger;
    vector<string> cycle = getDeadlockCycle(resourceTable);

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

    string preemptTrainName = cycle[0];
    Train* preemptTrain = trains[preemptTrainName];
    Intersection* currentIntersection = preemptTrain->current_location;

    if (currentIntersection) {
        string intersectionName = currentIntersection->name;

        logger.logPreemption(preemptTrainName, intersectionName);

        if (currentIntersection->release(preemptTrain)) {
            logger.logRelease(preemptTrainName, intersectionName);
            auto& holders = resourceTable[intersectionName];
            holders.erase(remove(holders.begin(), holders.end(), preemptTrainName), holders.end());
        }
    } else {
        logger.log("SERVER", "Preempted train has no current location. Skipping release.");
    }
}

