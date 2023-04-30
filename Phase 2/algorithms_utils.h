#include "defines.h"
int algorithmFlag = 1;
int algorithmBlockingFlag = 1; // for handling processes that arrive at the same time
int selectedAlgorithm, quantum, memoryPolicy;
bool isRunning;
struct msgBuff message;
struct PQueue *readyProcessesPriorityQueue;
struct Queue *readyProcessesQueue, *waitingProcessesQueue;
struct ProcessStruct *runningProcess = NULL;

// Run a process with a given quantum time
// currProcess: a pointer to the process structure
// quantum: the quantum time for RR algorithm
void runProcess(struct ProcessStruct *currProcess, int quantum)
{
    runningProcess = currProcess;
    isRunning = true;
    // runningProcess->lastStartedTime = getClk();
    if (runningProcess->pid != -1) // if the process started before, send CONTINUE_PROCESS signal to make it continue its execution
    {
        printf("Process with id %d and pid %d continued, clk %d\n", runningProcess->id, runningProcess->pid, getClk());
        kill(runningProcess->pid, CONTINUE_PROCESS);
        return;
    }
    int pid = fork();
    if (pid == -1)
    {
        perror("Error in fork");
        exit(-1);
    }
    if (pid == 0) // make new process
    {
        printf("process %d started at time %d \n", runningProcess->id, getClk());
        fflush(stdout);
        char remainingTimeChar[13];
        sprintf(remainingTimeChar, "%d", currProcess->remainingTime);
        char quantumChar[13];
        sprintf(quantumChar, "%d", quantum);
        char *argv[] = {"./process.out", remainingTimeChar, quantumChar, NULL}; // send remaining time and quantum as arguments
        int execlResult = execvp(argv[0], argv);
        if (execlResult == -1)
        {
            perror("Error in execvp");
            exit(-1);
        }
    }
    runningProcess->pid = pid;
    runningProcess->startTime = getClk();
    runningProcess->lastStopedTime = getClk();
}

// This function terminates the process and frees the memory
// sigNum: the signal number for termination
void terminateProcess(int sigNum)
{
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    switch(memoryPolicy){
        case FIRST_FIT_POLICY:
            deallocateProcessMemoryFirstFit(runningProcess);
            break;

        case BUDDY_POLICY:
            deallocateProcessMemoryBuddy(runningProcess);
            break;
    }

    free(runningProcess);
    runningProcess = NULL;
    isRunning = false;
    fflush(stdout);

    struct ProcessStruct *waitingProcess = peekQueue(waitingProcessesQueue);
    //printf("PROCESS ID IS %d AND MEMSIZE IS %d\n", waitingProcess->id, waitingProcess->memSize);

    if (waitingProcess != NULL)
    {
        int firstFitAllocation = 0, buddyAllocation = 0;
        switch(memoryPolicy){
            case FIRST_FIT_POLICY:
                printf("Trying to allocate %d with first fit\n", waitingProcess->id);
                firstFitAllocation = allocateProcessMemoryFirstFit(waitingProcess);
                break;

            case BUDDY_POLICY:
                printf("Trying to allocate %d with buddy\n", waitingProcess->id);
                buddyAllocation = allocateProcessMemoryBuddy(waitingProcess);
                break;
        }
        if(firstFitAllocation == 1 || buddyAllocation == 1){
            waitingProcess = dequeue(waitingProcessesQueue);
            printf("Process %d is dequeued from the waiting queue\n", waitingProcess->id);
            if (selectedAlgorithm == 1)
                push(readyProcessesPriorityQueue, waitingProcess, waitingProcess->priority);
            else if (selectedAlgorithm == 2)
                push(readyProcessesPriorityQueue, waitingProcess, waitingProcess->remainingTime);
            else if (selectedAlgorithm == 3)
                enqueue(readyProcessesQueue, waitingProcess);
        }
    }
}

// This function blocks the process, puts it at the end of the queue or the priority queue depending on the algorithm, and makes isRunning=false to begin another process.
void blockProcess()
{
    if (selectedAlgorithm == 3)
    {
        // For round robin algorithm, enqueue the process at the end of the queue
        // queue: a pointer to the queue structure
        // runningProcess: a pointer to the process structure
        enqueue(readyProcessesQueue, runningProcess);
    }
    else
    {
        // For shortest remaining time next algorithm, update the last stopped time and the remaining time of the process, and push it to the priority queue
        // priorityQueue: a pointer to the priority queue structure
        // runningProcess: a pointer to the process structure
        runningProcess->lastStopedTime = getClk();
        kill(runningProcess->pid, INTERRUPT_SIGNAL);
        kill(runningProcess->pid, SIGSTOP);
        push(readyProcessesPriorityQueue, runningProcess, runningProcess->remainingTime);
    }
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    isRunning = false;
    fflush(stdout);
}