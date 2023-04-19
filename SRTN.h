void SRTN(struct PQueue *pQueue)
{
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    priorityQueue = pQueue;
    quantum = 1;
    while (algorithmFlag||!isEmptyPQ(priorityQueue) || isRunning) { // while queue isn't empty or runung
        if (isEmptyPQ(priorityQueue)) // queue is empty doesn't make thing
            continue;
        if (!isRunning) {          // isn't running and in Pqueue --> run it  
            readyProcess = peek(priorityQueue);
            pop(priorityQueue);            
            runningProcess = readyProcess;
            runProcess(readyProcess);
        }
    }
};