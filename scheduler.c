#include "headers.h"
#include "algorithms_functions.h"
#include "HPF.h"
#include "RR.h"
#include "SRTN.h"


void pushProcessToSRTN(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        push(priorityQueue, newProcess, newProcess->remainingTime);
    }
}

void pushProcessToHPF(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        push(priorityQueue, newProcess, newProcess->priority);
    }
}

void pushProcessToRR(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        enqueue(queue, newProcess);
    }
}

void getProcess(int signum)
{
    int rec_val = msgrcv(messageQueueID, &message, sizeof(message.process), 5, !IPC_NOWAIT);

    printf("Scheduler Received Process with ID=%d\n", message.process.id);
    fflush(stdout);

    if (rec_val == -1)
    {
        perror("Error in receiving from msg queue\n");
    }

    switch (selectedAlgorithm)
    {
    case 1:
        pushProcessToHPF(message.process);
        break;
    case 2:
        pushProcessToSRTN(message.process);
        if(isRunning){
            int tempRunningRime=runningProcess->remainingTime;
            runningProcess->remainingTime=runningProcess->remainingTime-(getClk()-runningProcess->lastStopedTime);
            if(message.process.runningTime < runningProcess->remainingTime)
                 blockProcess();
            else runningProcess->remainingTime=tempRunningRime;
        }
        break;
    case 3:
        pushProcessToRR(message.process);
        break;
    }

    // Process has been pushed to the queue
    // Up the semaphore to allow process generator to continue
    up(processGeneratorAndSchedulerSemID);

    // check if that process was the terminating one (id = -1)
    if (message.process.id == -1)
    {
        algorithmFlag = 0;
    }
}
void changeAlgorithmFlag(int sigNum){
    algorithmFlag=0;
}
void changeBlockingFlag(int signum){
    algorithmBlockingFlag=!algorithmBlockingFlag;
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, getProcess);
    signal(SIGRTMIN,changeAlgorithmFlag);
    signal(SIGUSR2,terminateProcess);
    signal(SIGRTMIN+1, quantumFinished);
    signal(SIGRTMIN+2,changeBlockingFlag);

    initClk();
    createSemaphore();
    createMessageQueue();

    selectedAlgorithm = atoi(argv[1]);
    printf("Selected Algorithm: %d\n", selectedAlgorithm);
    fflush(stdout);

    switch (selectedAlgorithm)
    {
    case 1:
        priorityQueue = createPriorityQueue();
        // Enter an infinite loop to process the priority queue
        HPF(priorityQueue);
        break;
    case 2:
        // Allocate the priority queue for algorithms 1 and 2
        priorityQueue = createPriorityQueue();
        // Enter an infinite loop to process the priority queue
        SRTN(priorityQueue);
        break;
    case 3:
        // Allocate the queue for algorithm 3
        queue = createQueue();
        quantum = atoi(argv[2]);
        // Enter an infinite loop to process the queue
        RR(queue,quantum);
        break;
    default:
        // Handle invalid algorithm selection
        printf("Invalid algorithm selected.\n");
        break;
    }

    destroyClk(false);
    return 0;
}
