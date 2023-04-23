// This function implements the shortest remaining time next algorithm
// pQueue: a pointer to the priority queue structure
void SRTN(struct PQueue *pQueue)
{
    
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    priorityQueue = pQueue;
    while (algorithmFlag||!isEmptyPQ(priorityQueue) || isRunning) { // while queue isn't empty or running
        if (isEmptyPQ(priorityQueue)) // queue is empty doesn't do anything
            continue;
        if (!isRunning && algorithmBlockingFlag) {          // isn't running and in Pqueue --> run it  
            readyProcess = peek(priorityQueue); // get the process with the shortest remaining time
            pop(priorityQueue); // remove it from the queue           
            runningProcess = readyProcess; // assign it to the running process
            runProcess(readyProcess,-1); // run it with no quantum time
        }
    }
};