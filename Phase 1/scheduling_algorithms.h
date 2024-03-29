#include "algorithms_utils.h"
#include "scheduler_utils.h"

// This function implements the highest priority first algorithm
// pqueue: a pointer to the priority queue structure
void HPF(struct PQueue *pqueue)
{
    struct PQueue *processQueue;        // pointer for priorityQueue
    struct ProcessStruct *readyProcess; // pointer for processes in Pqueue
    processQueue = pqueue;
    while (algorithmFlag || !isEmptyPQ(processQueue) || isRunning)
    {                                // while queue isn't empty or running
        if (isEmptyPQ(processQueue)) // queue is empty doesn't make thing
            continue;
        if (!isRunning && algorithmBlockingFlag)
        {                                      // isn't running and in Pqueue --> run it
            readyProcess = peek(processQueue); // get the process with the highest priority
            pop(processQueue);                 // remove it from the queue
            runningProcess = readyProcess;     // assign it to the running process
            runProcess(readyProcess, -1);      // run it with no quantum time
        }
    }
};

// This function implements the shortest remaining time next algorithm
// pQueue: a pointer to the priority queue structure
void SRTN(struct PQueue *pQueue)
{

    struct ProcessStruct *readyProcess; // pointer for processes in Pqueue
    priorityQueue = pQueue;
    while (algorithmFlag || !isEmptyPQ(priorityQueue) || isRunning)
    {                                 // while queue isn't empty or running
        if (isEmptyPQ(priorityQueue)) // queue is empty doesn't do anything
            continue;
        if (!isRunning && algorithmBlockingFlag)
        {                                       // isn't running and in Pqueue --> run it
            readyProcess = peek(priorityQueue); // get the process with the shortest remaining time
            pop(priorityQueue);                 // remove it from the queue
            runningProcess = readyProcess;      // assign it to the running process
            runProcess(readyProcess, -1);       // run it with no quantum time
        }
    }
};

// Handle the signal to indicate that a quantum has finished
// signum: the signal number
void quantumFinished(int signum)
{
    blockProcess();
}

// Implement the RR algorithm
// q: a pointer to the queue of processes
// quant: the quantum time for RR algorithm
void RR(struct Queue *q, int quant)
{
    struct ProcessStruct *readyProcess; // pointer for processes in queue
    queue = q;                          // set the global queue variable to q
    quantum = quant;                    // set the global quantum variable to quant
    while (algorithmFlag || !isEmptyQueue(queue) || isRunning)
    { // while the algorithm flag is true or the queue is not empty or a process is running

        if (isEmptyQueue(queue)) // if the queue is empty, do nothing
        {
            continue;
        }
        if (!isRunning)
        {                                      // if no process is running and there are processes in queue, run the first one
            readyProcess = dequeue(queue);     // get the first process from the queue
            runningProcess = readyProcess;     // set the global running process variable to readyProcess
            runProcess(readyProcess, quantum); // run the ready process with quantum time
        }
    }
};