void SRTN(struct PQueue *pQueue)
{
    
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    priorityQueue = pQueue;
    while (algorithmFlag||!isEmptyPQ(priorityQueue) || isRunning) { // while queue isn't empty or runung
        if (isEmptyPQ(priorityQueue)) // queue is empty doesn't do anything
            continue;
        if (!isRunning) {          // isn't running and in Pqueue --> run it  
            readyProcess = peek(priorityQueue);
            pop(priorityQueue);            
            runningProcess = readyProcess;
            runProcess(readyProcess,-1);
        }
        //printf("hello %d\n",isEmptyPQ(priorityQueue));
    }
};
