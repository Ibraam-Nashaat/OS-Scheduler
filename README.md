# OS-Scheduler
This project aims to implement a CPU scheduler with its complementary components in an operating system environment. The scheduler determines the order of execution for scheduled processes based on specific algorithms.

## Requirements
1. Linux
2. GCC Compiler

## How to run using the terminal
- Phase 1 Folder:
  1. `cd Phase 1`
  2. `make build`
  3. `./process_generator.out processes.txt -sch x -q y`,  where `x` is either 1,2 or 3 (where 1 represents HPF, 2 represents SRTN and 3 represents RR). Note that you provide `-q y` argument only in case you selected RR algorithm, where `y` is the quantum.
- Phase 2 Folder:
  1. `cd Phase 2`
  2. `make build`
  3. `./process_generator.out processes.txt -sch x -q y -mem z`,  where `x` is either 1,2 or 3 (where 1 represents HPF, 2 represents SRTN and 3 represents RR) and `z` is the memory policy and it is either 1 or 2 (where 1 represents first fit and 2 represents buddy memory allocation). Note that you provide `-q y` argument only in case you selected RR algorithm, where `y` is the quantum.

## System Description

The system is based on a computer with a single CPU and infinite memory. The following components are required for the scheduler:

1. Process Generator: This module is responsible for generating processes, reading input files, creating the scheduler and clock processes, and providing the necessary parameters to the scheduler.

2. Clock: The clock module emulates an integer time clock and is used for simulation and interprocess communication (IPC).

3. Scheduler: The scheduler is the core component that keeps track of processes and their states. It implements various scheduling algorithms and determines which process will run and for how long.

4. Process: Each process acts as a CPU-bound entity and notifies the scheduler upon termination.

## Scheduling Algorithms

The project requires the implementation of the following three scheduling algorithms:

1. Non-preemptive Highest Priority First (HPF): The scheduler prioritizes processes based on their priority and selects the process with the highest priority to run first.

2. Shortest Remaining Time Next (SRTN): The scheduler selects the process with the shortest remaining execution time to run next.

3. Round Robin (RR): The scheduler allocates a fixed time slice to each process in a cyclic manner, allowing each process to run for a predefined time quantum before switching to the next process.

## Input/Output

The project utilizes input and output files to simulate and evaluate the operating system design. The following files are used:

- **Input File**: `processes.txt`
  - Each non-comment line represents a process with fields separated by tab characters.
  - The input file specifies process information such as ID, arrival time, runtime, priority, and memory size.

- **Output Files**:
  1. `scheduler.log`
     - This file logs the scheduler's activity and the state of processes at different times.
     - It includes information such as process start, stop, resume, and finish times, along with process-specific details.
  
  2. `scheduler.perf`
     - This file reports performance metrics of the scheduler, including CPU utilization, average weighted turnaround time, average waiting time, and standard deviation for average weighted turnaround time.

  3. `memory.log`
     - This file logs memory allocation and deallocation information.
     - It includes details such as the allocated bytes for a process, the time of allocation and deallocation, and the corresponding process ID.

## Memory Allocation Policies

The project requires extending the Scheduler to include memory allocation capabilities. The following memory allocation policies are implemented:

1. First Fit: The scheduler allocates memory space for processes using the first available block that can accommodate the process size.

2. Buddy Memory Allocation: Buddy memory allocation technique is implemented as an alternative policy. 

## Assumptions

The project is based on the following assumptions:

- The system has a single uni-core CPU.
- The total memory size is fixed at 1024 bytes.
- Each process size is less than or equal to 256 bytes.
- The memory allocated for a process remains constant during its time in the system.
