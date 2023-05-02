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


void checkIfProcessArrivedAtSameTimeOfQuantumFininshed(struct ProcessStruct incomingProcess){
    
    struct ProcessStruct *process=peekQueueTail(queue);
    if(process!=NULL && process->lastStopedTime==getClk()){
        int size=queueSize(queue);
        while(size--){
            process=dequeue(queue);
            if(size==0){
                pushProcessToRR(incomingProcess);
            }
            enqueue(queue,process);
        }
    }
    else pushProcessToRR(incomingProcess);
}