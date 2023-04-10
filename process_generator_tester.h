struct Queue* testReadFile(struct Queue* queue){
struct Queue* tempQueue=createQueue();
while(!isEmptyN(queue)){
    struct ProcessStruct* process=peekN(queue);
    printf("%d\t%d\t%d\t%d\n",process->id,
                            process->arrivalTime,
                            process->runningTime,
                            process->priority);
    enQueue(tempQueue,process);
    deQueue(queue);
    
}
return tempQueue;
}

