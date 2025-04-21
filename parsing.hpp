#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <fstream>
#include <unordered_map>
#include <semaphore.h>
#include <pthread.h>
#include <condition_variable>
#include <algorithm>

class Train;

class Intersection {
public:
    std::string name;
    unsigned int capacity;
    bool is_mutex;
    bool lock_state;
    unsigned int train_count;

    pthread_mutex_t mtx;
    sem_t semaphore;
    std::condition_variable cv;

    std::vector<Train*> trains_in_intersection;

    Intersection(std::string name, unsigned int capacity);

    bool acquire(Train* train);
    bool release(Train* train);
    bool isOpen();
};

class Train {
public:
    std::string name;
    std::vector<Intersection*> route;
    Intersection* current_location;
    int mtype;

    Train(std::string name, std::vector<Intersection*> route);
};

std::unordered_map<std::string, Intersection*> parseIntersections(const std::string& filename);
std::unordered_map<std::string, Train*> parseTrains(const std::string& filename, std::unordered_map<std::string, Intersection*>& intersections);

#endif
