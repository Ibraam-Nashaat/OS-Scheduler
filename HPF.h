// This function implements the highest priority first algorithm
// pqueue: a pointer to the priority queue structure
void HPF(struct PQueue *pqueue)
{
    struct PQueue *processQueue; // pointer for priorityQueue
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    processQueue = pqueue;
    while (algorithmFlag||!isEmptyPQ(processQueue) || isRunning) { // while queue isn't empty or running
        if (isEmptyPQ(processQueue)) // queue is empty doesn't make thing
            continue;
        if (!isRunning && algorithmBlockingFlag) {          // isn't running and in Pqueue --> run it
            readyProcess = peek(processQueue); // get the process with the highest priority
            pop(processQueue); // remove it from the queue
            runningProcess=readyProcess; // assign it to the running process
            runProcess(readyProcess,-1); // run it with no quantum time
        }
    }
};