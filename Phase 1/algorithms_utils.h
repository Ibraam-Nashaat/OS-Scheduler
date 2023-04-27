#include "defines.h"

int algorithmFlag = 1;
int algorithmBlockingFlag = 1; // for handling processes that arrive at the same time
int selectedAlgorithm, quantum;
int totalWaitingTime = 0;
float sumWeightedTAT = 0;
bool isRunning;
struct msgBuff message;
struct PQueue *priorityQueue;
struct Queue *queue;
struct ProcessStruct *runningProcess = NULL;
FILE *logFile;

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
        printf("\033[1;33mProcess with id %d and pid %d continued at time = %d\033[0m\n", runningProcess->id, runningProcess->pid, getClk(), runningProcess->lastStopedTime);
        fprintf(logFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), runningProcess->id, runningProcess->arrivalTime, runningProcess->runningTime, runningProcess->remainingTime, runningProcess->waitingTime);
        runningProcess->waitingTime += (getClk() - runningProcess->lastStopedTime);
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
        printf("\033[1;34mProcess %d started at time %d\033[0m\n", runningProcess->id, getClk());

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
    
    fprintf(logFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), runningProcess->id, runningProcess->arrivalTime, runningProcess->runningTime, runningProcess->remainingTime, runningProcess->waitingTime);

    runningProcess->pid = pid;
    runningProcess->startTime = getClk();
    runningProcess->waitingTime = runningProcess->startTime - runningProcess->arrivalTime;
    runningProcess->lastStopedTime = getClk();
}

// This function terminates the process and frees the memory
// sigNum: the signal number for termination
void terminateProcess(int sigNum)
{
    totalWaitingTime += runningProcess->waitingTime;
    printf("\033[1;32mProcess %d finished at time = %d\033[0m\n", runningProcess->id, getClk(), runningProcess->waitingTime);
    int turnaroundTime = getClk() - runningProcess->arrivalTime;
    float weightedTAT = 0;
    if(runningProcess->runningTime){
        weightedTAT = turnaroundTime / (float) runningProcess->runningTime;
    }

    sumWeightedTAT += weightedTAT;

    fprintf(logFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), runningProcess->id, runningProcess->arrivalTime, runningProcess->runningTime, runningProcess->remainingTime, runningProcess->waitingTime, turnaroundTime, weightedTAT);

    free(runningProcess);
    runningProcess = NULL;
    isRunning = false;
    fflush(stdout);
}

// This function blocks the process, puts it at the end of the queue or the priority queue depending on the algorithm, and makes isRunning=false to begin another process.
void blockProcess()
{
    if (selectedAlgorithm == 3)
    {
        // For round robin algorithm, enqueue the process at the end of the queue
        // queue: a pointer to the queue structure
        // runningProcess: a pointer to the process structure
        enqueue(queue, runningProcess);
    }
    else
    {
        // For shortest remaining time next algorithm, update the last stopped time and the remaining time of the process, and push it to the priority queue
        // priorityQueue: a pointer to the priority queue structure
        // runningProcess: a pointer to the process structure
        kill(runningProcess->pid, INTERRUPT_SIGNAL);
        kill(runningProcess->pid, SIGSTOP);
        push(priorityQueue, runningProcess, runningProcess->remainingTime);
    }
    runningProcess->lastStopedTime = getClk();
    printf("\033[1;33mid = %d Blocked pid = %d remaining time = %d current time = %d\033[0m\n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    fprintf(logFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), runningProcess->id, runningProcess->arrivalTime, runningProcess->runningTime, runningProcess->remainingTime, runningProcess->waitingTime);
    isRunning = false;
    fflush(stdout);
}