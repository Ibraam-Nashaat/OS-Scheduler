struct Queue *testReadFile(struct Queue *queue)
{
    struct Queue *tempQueue = createQueue();
    while (!isEmptyQueue(queue))
    {
        struct ProcessStruct *process = dequeue(queue);
        ;
        printf("%d\t%d\t%d\t%d\t%d\n", process->id,
               process->arrivalTime,
               process->runningTime,
               process->priority,
               process->memSize);
        enqueue(tempQueue, process);
    }
    return tempQueue;
}
void testLinkedList(struct Queue *queue)
{
    struct sortedLinkedList *listNode = createSortedLinkedList();
    struct Queue *tempQueue = createQueue();
    int i = 0;
    struct sortedLinkedListNode *nodeToDelete;
    while (!isEmptyQueue(queue))
    {
        struct ProcessStruct *process = dequeue(queue);
        struct memoryNode *mem = createMemoryNode(0, process->memSize, process->pid);
        insert(listNode, mem, process->priority);
        enqueue(tempQueue, process);
        // if(i==1) nodeToDelete=listNode->tail;
        i++;
    }

    struct sortedLinkedListNode *ptr = listNode->head;
    printf("Before removing:\n");
    while (ptr != NULL)
    {
        printf("start %d, end %d, size %d, pid %d\n",
               ptr->data->startLocation, ptr->data->endLocation,
               ptr->data->size, ptr->data->pid);
        ptr = ptr->next;
    }
    removeLinkedListNode(listNode->head, listNode);
    removeLinkedListNode(listNode->tail, listNode);

    ptr = listNode->head;
    printf("After removing two elements: Is empty %d\n", isEmptyLL(listNode));
    while (ptr != NULL)
    {
        printf("start %d, end %d, size %d, pid %d\n",
               ptr->data->startLocation, ptr->data->endLocation,
               ptr->data->size, ptr->data->pid);
        ptr = ptr->next;
    }
    removeLinkedListNode(listNode->tail, listNode);
    ptr = listNode->head;

    printf("After removing another elements: Is empty %d\n", isEmptyLL(listNode));
    while (ptr != NULL)
    {
        printf("start %d, end %d, size %d, pid %d\n",
               ptr->data->startLocation, ptr->data->endLocation,
               ptr->data->size, ptr->data->pid);
        ptr = ptr->next;
    }
}

void printMemory(struct sortedLinkedList* list,char memoryType[]){
    struct sortedLinkedListNode* listNode=list->head;
    printf("%s\n",memoryType);
    while (listNode != NULL)
    {
        printf("size: %d ,start location: %d, end location: %d \n", 
                listNode->data->size,listNode->data->startLocation, listNode->data->endLocation);
        listNode = listNode->next;
    }
}
