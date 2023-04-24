struct Queue* testReadFile(struct Queue* queue){
struct Queue* tempQueue=createQueue();
while(!isEmptyQueue(queue)){
    struct ProcessStruct* process=dequeue(queue);;
    printf("%d\t%d\t%d\t%d\t%d\n",process->id,
                            process->arrivalTime,
                            process->runningTime,
                            process->priority,
                            process->memSize);
    enqueue(tempQueue,process);
    
    
}
return tempQueue;
}

