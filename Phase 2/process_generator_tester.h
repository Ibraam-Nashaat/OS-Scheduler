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
void testLinkedList(struct Queue* queue){
    struct sortedLinkedList* list=createSortedLinkedList();
    struct Queue* tempQueue=createQueue();
    int i=0;
    struct sortedLinkedListNode* nodeToDelete;
    while(!isEmptyQueue(queue)){
        struct ProcessStruct* process=dequeue(queue);
        struct memoryNode* mem=createMemoryNode(0,process->memSize,process->pid);
        insert(list,mem,process->priority);
        enqueue(tempQueue,process);
       // if(i==1) nodeToDelete=list->tail;
        i++;
    }
    
    struct sortedLinkedListNode* ptr=list->head;
    printf("Before removing:\n");
     while(ptr!=NULL){
        printf("start %d, end %d, size %d, pid %d\n",
                ptr->data->startLocation,ptr->data->endLocation,
                ptr->data->size,ptr->data->pid);
        ptr=ptr->next;
    }
    removeLinkedListNode(list->head,list);
    removeLinkedListNode(list->tail,list);
    
    ptr=list->head;
    printf("After removing two elements: Is empty %d\n",isEmptyLL(list));
    while(ptr!=NULL){
        printf("start %d, end %d, size %d, pid %d\n",
                ptr->data->startLocation,ptr->data->endLocation,
                ptr->data->size,ptr->data->pid);
        ptr=ptr->next;
    }
    removeLinkedListNode(list->tail,list);
    ptr=list->head;
    
    printf("After removing another elements: Is empty %d\n",isEmptyLL(list));
     while(ptr!=NULL){
        printf("start %d, end %d, size %d, pid %d\n",
                ptr->data->startLocation,ptr->data->endLocation,
                ptr->data->size,ptr->data->pid);
        ptr=ptr->next;
    }
}

