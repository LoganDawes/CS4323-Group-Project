#ifndef TRAIN_FORKING_HPP
#define TRAIN_FORKING_HPP
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "parsing.hpp"

void train_forking();

void train_behavior();
#endif