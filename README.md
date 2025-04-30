# Operating Systems Project 2023/2024

## Introduction

This document briefly describes the implementation of a chain reaction simulation project. 
The project demonstrates the use of concurrent processes and resource management in a UNIX-like operating environment, highlighting inter-process communication (IPC) and synchronization techniques.

## Project Architecture

The system is organized into four main types of processes: Master, Atom, Activator, and Feeder. 
These processes cooperate to simulate the chain reaction, aiming to maximize concurrency and efficiently manage events within the simulation. 
The project architecture has been designed to be modular, dividing functionality into specific source and header files for each component. 
This approach not only facilitates code maintenance but also allows for a clearer separation of responsibilities between the various parts of the system.

## Description of Key Components

### Master Process

Coordinator of the simulation, creates atom processes and manages activator and feeder processes:

1. #### Resource Creation:
   - Message Queues: A message queue is created for communication between various processes, mainly to confirm creation and termination.
   - Shared Memory: Two shared memory segments are created, one to maintain the IDs of atom processes and one for simulation statistics.
   - Semaphores: Two semaphores are created to manage access to shared structures: one for atoms and one for statistics.
2. #### Process Management:
   - Begins by creating Atom processes, then subsequently Activator and Feeder processes
   - Upon receiving confirmation of process creation, it sets the simulation duration and notifies all processes to start the simulation through the use of Signals.
   - Once the simulation has started, it listens for any TIMEOUT, EXPLODE, BLACKOUT, and MELTDOWN signals
   - To print statistics every second, the master uses another child process that must handle printing every second and manage energy, possibly triggering EXPLODE and BLACKOUT signals

### Atom Process

Simulates atomic fission through fork, essential for the dynamics of the simulation.

1. #### Process Management:
    - Upon creation, the Atom process retrieves information about shared memories and semaphores through environment variables and subsequently informs the Master process of its creation via the message queue
    - It then saves its pid in the data structure and waits for the fission signal from the Activator, also using Signals.
    - Upon receiving the fission signal, the handler activates and checks if the atomic number allows fission:
    - If it does, it performs a fork creating a new atom, increases the statistics of fissions and energy.
    - Otherwise, it becomes waste, removes its pid from the registry, and increases waste statistics.

### Activator Process

The activator sends fission signals to selected atoms at regular intervals, orchestrating part of the simulation logic through signals and semaphores.

1. #### Process Management:
   - Upon creation, it informs the Master process of its creation via the message queue and retrieves information about memories and semaphores through environment variables
   - It waits for the "Start Simulation" signal via Signal(SIGUSR1).
   - When the simulation starts, it activates the timer and every STEP_ACTIVATOR sends the fission signal (SIGUSR1) by randomly selecting RANDOM_ATOM_SET atoms from the registry.
   - Updates the statistics.

### Feeder Process

The feeder adds new atoms to the simulation, simulating the introduction of new fuel. It uses timers to manage the periodic creation of new atom processes.

1. #### Process Management:
   - Upon creation, it informs the Master process of its creation via the message queue and retrieves information about memories and semaphores through environment variables
   - Every STEP_FEEDER it creates N_NEW_ATOMS by executing a fork with the Atom process code

## Compilation and Execution Guide

### Parameter Setting

Modify the configuration parameters in the utils.h file to adjust the simulation to your needs. Here you can set variables such as simulation duration, number of initial atoms, etc.

### Compilation

To compile the project:

make all


This command builds all the necessary executables for the project.

### Cleaning

To remove executables and clean the project:

make clean


### Execution

To start the simulation:

Make run or ./master
