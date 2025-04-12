/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: This code reads the intersections and trains from their respective text files into objects.
It also defines the basic methods
*/

#include "parsing.hpp"

using namespace std;

// Define intersection class constructor
Intersection::Intersection(string name, unsigned int capacity) : name(name), capacity(capacity), is_mutex(capacity==1), train_count(0) {
    if(is_mutex){ // Create mutex
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&mtx, &attr);
    } else { // Create semaphore
        sem_init(&semaphore, 1, capacity);
    }
}
    
// Acquire train, called in travel function. Returns whether it was successfully acquired.
bool Intersection::acquire(Train* train) {
    if(is_mutex){ // Mutex method
        if(train_count == 0) { // If intersection is empty
            pthread_mutex_lock(&mtx);
            trains_in_intersection.push_back(train);
            train_count++;
            return true; // Train was acquired
        } else {
            return false; // Train was not acquired
        }
    } else { // Semaphore method
        if (train_count < capacity) { // If there is room for another train
            sem_wait(&semaphore); // Unsure about this
            trains_in_intersection.push_back(train);
            train_count++;
            return true; // Train was acquired
        } else {
            return false; // Train was not acquired
        }
        
    }
}
    
// Release train, called by the travel function. Returns whether it was successfully released.
bool Intersection::release(Train* train) {
    // Find the specific train in the intersection
    auto found_train = find(trains_in_intersection.begin(), trains_in_intersection.end(), train);
    if(found_train != trains_in_intersection.end()) { // Unlock mutex and update semaphore for intersection
        if(is_mutex){
            pthread_mutex_unlock(&mtx);
            return true;
        } else {
            sem_post(&semaphore);
            return true;
        }
    } else {
        cout << "Error: Train not found";
        return false;
    }
}

bool Intersection::isOpen() { // Returns whether the intersection has an availability or not.
    return is_mutex ? (train_count == 0) : (train_count < capacity);
}


// Define train class constructor
Train::Train(string name, vector<Intersection*> route): name(name), route(route) {}

// Parse intersections.txt into objects of type Intersection
unordered_map<string, Intersection*> parseIntersections(const string& filename){
    // Create intersections unordered map so trains can access intersections by name
    unordered_map<string, Intersection*> intersections;
    ifstream file(filename);
    string line;
    
    // Works for any number of intersections
    while(getline(file, line)) { // While there is a next line
        stringstream ss(line);
        string name;
        unsigned int capacity;

        getline(ss, name, ':');
        ss >> capacity;

        // Debugging parsing, remove in submission
        cout << "Name : " << name << " , Capacity: " << capacity << endl;
        
        intersections[name] = new Intersection(name, capacity);
    }
    
    return intersections;
}

// Parse trains.txt into objects of type Train
unordered_map<string, Train*> parseTrains(const string& filename, unordered_map<string, Intersection*>& intersections){
    ifstream file(filename);
    string line;
    unordered_map<string, Train*> trains;
 
    while(getline(file, line)) { // While there is a next line
        stringstream ss(line);
        string name;
        getline(ss, name, ':');

        string intersection;
        vector<Intersection*> route;

        while(getline(ss, intersection, ',')){
            if(intersections.find(intersection) != intersections.end()){
                route.push_back(intersections[intersection]);
            } else {
                cout <<"ERROR: intersection not found";
            }
        }
        
        // Debugging parsing, remove in submission
        cout << "Train: " << name << " | Route: ";
        for (auto *intersection : route)
        {
            cout << intersection->name << " ";
        }
        cout << endl;

        trains[name] = new Train(name, route);
    }

    return trains;
}