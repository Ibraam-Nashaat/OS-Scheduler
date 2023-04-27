struct sortedLinkedList *memoryHoles, *memoryUsed;
// split node to two nodes sequential
struct sortedLinkedListNode *splitNode(struct sortedLinkedListNode *nodeToSplit, int memSize)
{
    // Create a new node with part the data of the original node
    if (nodeToSplit == NULL)
        return NULL;
    struct sortedLinkedListNode *newNode = (struct sortedLinkedListNode *)malloc(sizeof(struct sortedLinkedListNode));
    // alocate memoryNode -->data of LLNode
    newNode->data = (struct memoryNode *)malloc(sizeof(struct memoryNode));
    newNode->data->startLocation = nodeToSplit->data->startLocation;
    newNode->data->id = nodeToSplit->data->id;
    newNode->priority = nodeToSplit->priority;
    newNode->data->size = memSize;
    newNode->data->endLocation = newNode->data->startLocation + memSize;
    // Modify nodeToSplit data
    nodeToSplit->data->size = nodeToSplit->data->size - memSize;
    nodeToSplit->data->startLocation = newNode->data->endLocation;
    nodeToSplit->priority = nodeToSplit->data->startLocation;
    // Adjust the pointers to insert the new node between the original node and the next node
    newNode->next = nodeToSplit;
    // Return a pointer to the new node created by the split
    return newNode;
}
bool mergeTwoNodes(struct sortedLinkedListNode *node1, struct sortedLinkedListNode *node2)
{
    // If either node is NULL, return
    if (node1 == NULL || node2 == NULL)
    {
        return false;
    }
    if (node1->data->endLocation == node2->data->startLocation - 1)
    {
        node1->data->size = node1->data->size + node2->data->size;
        node1->data->endLocation = node2->data->endLocation;
        node1->next = node2->next;
        free(node2->data);
        free(node2);
        return true;
    }
    return false;
}
void mergeHoles(struct sortedLinkedList *LL)
{
    struct sortedLinkedListNode *firstNodePtr = LL->head;
    struct sortedLinkedListNode *secNodePtr = firstNodePtr->next;
    while (secNodePtr != NULL)
    {
        if (!mergeTwoNodes(firstNodePtr, secNodePtr))
        {
            firstNodePtr = firstNodePtr->next;
            secNodePtr = secNodePtr->next;
        }
        else
            secNodePtr = firstNodePtr->next;
    }
}
void deallocateProcessMemoryFirstFit(struct ProcessStruct *process)
{
    struct memoryNode *memoryRemovedNode;
    struct sortedLinkedListNode *llNode = find(memoryUsed->head, process->id);
    if (!isEmptyLL(memoryUsed))
    {
        memoryRemovedNode = removeLinkedListNode(llNode, memoryUsed);
    }
    if (memoryRemovedNode == NULL)
    {
        printf("null\n");
        return;
    }
    memoryRemovedNode->id = -1;

    insert(memoryHoles, memoryRemovedNode, memoryRemovedNode->startLocation);
    mergeHoles(memoryHoles);
    printMemory(memoryHoles, "Memory Holes after Deallocation");
    printMemory(memoryUsed, "Memory Used after Deallocation");
}

bool allocateProcessMemoryFirstFit(struct ProcessStruct *Process)
{
    struct sortedLinkedListNode *currNode = memoryHoles->head;
    while (currNode != NULL && Process->memSize > currNode->data->size)
    {
        currNode = currNode->next;
    }
    if (currNode == NULL)
    {
        printf("NO memory available\n");
        printMemory(memoryHoles, "Memory Holes after allocation");
        printMemory(memoryUsed, "Memory Used after allocation");
        return false;
    }
    if (Process->memSize == currNode->data->size)
    {
        
        struct memoryNode *removedNode = removeLinkedListNode(currNode, memoryHoles);
        struct memoryNode *memNode = createMemoryNode(removedNode->startLocation, Process->memSize, Process->id);
        insert(memoryUsed, memNode,removedNode->startLocation);
    }
    else
    {
        
        struct sortedLinkedListNode *secCurrNode = splitNode(currNode, Process->memSize);
        struct memoryNode *memNode = createMemoryNode(secCurrNode->data->startLocation, Process->memSize, Process->id);
        insert(memoryUsed, memNode, secCurrNode->data->startLocation);
    }

    printMemory(memoryHoles, "Memory Holes after allocation");
    printMemory(memoryUsed, "Memory Used after allocation");
    return true;
}