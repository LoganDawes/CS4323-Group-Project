/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: This code creates a resource allocation graph for use in deadlock detection and handling requests
*/

#include "resource_allocation.hpp"

using namespace std;

// This should be called from server when ACQUIRE or RELEASE messages come through
class ResourceAllocationGraph{
    unordered_map<string, Intersection*> intersectionMap
    
    // TODO: change from id implementation to actual intersection name
    void acquire(Intersection, Train){
        intersectionMap[id]->acquire(Train)
    }
    
    void release(Intersection, Train){
        intersectionMap[id]->release(Train)
    }
    
    void printGraph(){
        for (intersectionMap){
            cout << id << is_mutex << capacity << lock_state << trains_in_intersection
        }
    }
}