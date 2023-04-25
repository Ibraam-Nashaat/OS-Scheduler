#define NULL ((void *)0)

struct sortedLinkedListNode*newSortedLLNode(struct memoryNode * memoryNode, int priority)
{
    struct sortedLinkedListNode*temp = (struct sortedLinkedListNode*)malloc(sizeof(struct sortedLinkedListNode));
    temp->data = memoryNode;
    temp->next = NULL;
    temp->priority = priority;
    return temp;
}

struct sortedLinkedList *createSortedLinkedList()
{
    struct sortedLinkedList *LL = (struct sortedLinkedList *)malloc(sizeof(struct sortedLinkedList));
    // Intialize queue
    LL->head = NULL;
    LL->tail = NULL;
    return LL;
};
bool isEmptyLL(struct sortedLinkedList *LL)
{
    return (LL->head == NULL);
}

// enQueue method-> add node to Queue
void insert(struct sortedLinkedList *LL, struct memoryNode * memoryNode, int priority)
{
    struct sortedLinkedListNode*LLnodeHead = LL->head;
    struct sortedLinkedListNode*nNode = newSortedLLNode(memoryNode, priority);
    // codition if queue is empty
    if (LL->head == NULL)
    {
        LL->tail = LL->head = nNode;
        return;
    }
    else
    {
        if (priority <= LLnodeHead->priority) // LOW priority first
        {
            nNode->next = LLnodeHead;
            LL->head = nNode;
        }
        else if (priority > LLnodeHead->priority) // if not try to rearrange itself
        {
            while ((LLnodeHead->next != NULL) && (LLnodeHead->next->priority <= priority))
                LLnodeHead = LLnodeHead->next;
            nNode->next = LLnodeHead->next;
            LLnodeHead->next = nNode;
        }
    }
}

struct sortedLinkedListNode* removeLinkedListNode(struct sortedLinkedListNode* node,struct sortedLinkedList* LL)
{
    struct sortedLinkedListNode* current=LL->head;
    struct sortedLinkedListNode* previous=NULL;
    if(current!=NULL && current->next==NULL){
        free(current);
        LL->head=NULL;
        LL->tail==NULL;
        return current;
    }
    if(current!=NULL && current==node){
        LL->head=current->next;
        free(current);
        return current;
    }
    while(current!=NULL && current!=node){
        previous=current;
        current=current->next;
    }
    if(current==NULL) return NULL;
    previous->next=current->next;
    free(current);
    return current;
}
struct sortedLinkedListNode* find(struct sortedLinkedListNode* headList, int pid) {
    struct sortedLinkedListNode* curr = headList;
    while (curr != NULL && curr->data->pid != pid) 
    {
        curr = curr->next;
    }
   
    return curr;
}
struct sortedLinkedListNode* splitNode(struct sortedLinkedListNode* nodeToSplit,int memSize) {
    // Create a new node with half the data of the original node
    struct sortedLinkedListNode* newNode = (struct sortedLinkedListNode*) malloc(sizeof(struct sortedLinkedListNode));
    newNode->data = (struct memoryNode*) malloc(sizeof(struct memoryNode));
    newNode->data->startLocation = nodeToSplit->data->startLocation;
    newNode->data->pid=nodeToSplit->data->pid;
    newNode->priority=nodeToSplit->priority;
    newNode->data->size=memSize;
    newNode->data->endLocation=newNode->data->startLocation+memSize;
    nodeToSplit->data->size=nodeToSplit->data->size-memSize;
    nodeToSplit->data->startLocation=newNode->data->endLocation;

    // Adjust the pointers to insert the new node between the original node and the next node
    newNode->next = nodeToSplit;
    // Return a pointer to the new node created by the split
    return newNode;
}