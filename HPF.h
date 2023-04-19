void HPF(struct PQueue *pqueue)
{
    struct PQueue *processQueue; // pointer for priorityQueue
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    processQueue = pqueue;
    while (algorithmFlag||!isEmptyPQ(processQueue) || isRunning) { // while queue isn't empty or runung
        if (isEmptyPQ(processQueue)) // queue is empty doesn't make thing
            continue;
        if (!isRunning) {          // isn't running and in Pqueue --> run it
            readyProcess = peek(processQueue);
            pop(processQueue);
            runningProcess=readyProcess;
            quantum = currQuantum = readyProcess->runningTime;
            runProcess(readyProcess);
        }
    }
};
