# CS4323-Group-Project
Repository for CS4323 Spring 2025 group project, the Multi-Train Railway Intersection Control System

Compiling the programs:
1. Log into the CSX server testing environment.
2. Navigate to directory with the included files
3. Run:
./compile.sh
4. Running the program:
./server

For various test cases, run:
./testcompile.sh
./test

Tested on CSX server:
csx1.cs.okstate.edu

## Files:
### intersections.txt
- **Purpose**: Defines intersections and their capacities.
- **Format**: `IntersectionName:Capacity`
- **Example**:
IntersectionA:1 IntersectionB:2 IntersectionC:1 IntersectionD:3 IntersectionE:1

### trains.txt
- **Purpose**: Defines train names and their routes (an ordered list of intersections).
- **Format**: `TrainName:Intersection1,Intersection2,...`
- **Example**:
Train1:IntersectionA,IntersectionB,IntersectionC Train2:IntersectionB,IntersectionD,IntersectionE Train3:IntersectionC,IntersectionD,IntersectionA Train4:IntersectionE,IntersectionB,IntersectionD

### parsing.cpp
Parses intersections.txt and trains.txt into objects with basic methods.

### train.cpp
Forks child processes based on the number of trains, then simulates travel across their defined route. Each train uses ipc communication to server.cpp to request AQUIRE or RELEASE.

### server.cpp
Main entry point to the program, calls parsing and train forking before switching to server role. Sends GRANT, WAIT, or DENY commands to the trains as a response to their requests. Will detect deadlocks if they occur.

### ipc.cpp
Configures shared memory segments that store mutexes and semaphores, then manages message queues that serve as a channel between server and trains.

## resource_allocation.cpp
Defines the resource allocation table class to keep a map of intersections.

### deadlock_recovery.cpp
Called by server if a deadlock is detected, is responsible for resolving the deadlock for the program to restore system flow.

### logging.cpp
Reads requests and responses sent between server and trains to write to a simulation.log file. Keeps track of simulated time and deadlock resolution steps.

### testing.cpp
Various functions to test certain aspects of the program during development. Also used to generate various scenarios for the program.

## Authors
- **Caden Blust**
- **Logan Dawes**
- **Myron Peoples**
- **Richard Powers**
- **Evelyn Wilson**
- **Gavin Zlatar**
