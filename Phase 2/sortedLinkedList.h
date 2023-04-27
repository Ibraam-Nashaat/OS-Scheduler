#define NULL ((void *)0)

struct sortedLinkedListNode *newSortedLLNode(struct memoryNode *memoryNode, int priority)
{
    struct sortedLinkedListNode *temp = (struct sortedLinkedListNode *)malloc(sizeof(struct sortedLinkedListNode));
    temp->data = memoryNode;
    temp->next = NULL;
    temp->priority = priority;
    return temp;
}

struct sortedLinkedList *createSortedLinkedList()
{
    struct sortedLinkedList *LL = (struct sortedLinkedList *)malloc(sizeof(struct sortedLinkedList));
    LL->head = NULL;
    LL->tail = NULL;
    return LL;
};
bool isEmptyLL(struct sortedLinkedList *LL)
{
    return (LL->head == NULL);
}

// enQueue method-> add node to Queue
void insert(struct sortedLinkedList *LL, struct memoryNode *memoryNode, int priority)
{
    struct sortedLinkedListNode *LLnodeHead = LL->head;
    struct sortedLinkedListNode *nNode = newSortedLLNode(memoryNode, priority);
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

struct memoryNode *removeLinkedListNode(struct sortedLinkedListNode *node, struct sortedLinkedList *LL)
{

    struct sortedLinkedListNode *current = LL->head;

    struct sortedLinkedListNode *previous = NULL;
    struct memoryNode *mem;
    if (current != NULL && current->next == NULL)
    {
        mem = current->data;
        free(current);
        LL->head = NULL;
        LL->tail == NULL;
        return mem;
    }
    if (current != NULL && current == node)
    {

        LL->head = current->next;
        mem = current->data;
        free(current);
        return mem;
    }

    while (current != NULL && current != node)
    {

        previous = current;

        current = current->next;
    }

    if (current == NULL)
        return false;

    previous->next = current->next;
    mem = current->data;
    free(current);
    return mem;
}
// find node by its pid
struct sortedLinkedListNode *find(struct sortedLinkedListNode *headList, int id)
{
    struct sortedLinkedListNode *curr = headList;
    while (curr != NULL && curr->data->id != id)
    {
        curr = curr->next;
    }

    return curr;
}
