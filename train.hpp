#ifndef TRAIN_FORKING_HPP
#define TRAIN_FORKING_HPP
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "parsing.hpp"

void train_forking(unordered_map<std::string, Intersection*>& intersections, unordered_map<std::string, Train*>& trains);

void train_behavior(Train* train);
#endif