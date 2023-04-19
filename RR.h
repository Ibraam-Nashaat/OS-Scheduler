void RR(struct Queue *q, int quant)
{
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    queue = q;
    quantum = quant;
    while (algorithmFlag||!isEmptyQueue(queue) || isRunning) { // while queue isn't empty or runung

        if (isEmptyQueue(queue)) // queue is empty doesn't make thing
            {
                continue;
            }
        if (!isRunning) {          // isn't running and in Pqueue --> run it
            readyProcess = dequeue(queue);
            //pop(queue);
            currQuantum = quantum;
            runningProcess=readyProcess;
            runProcess(readyProcess);
        }
    }
};
