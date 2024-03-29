struct ProcessStruct
{
    int id,
        arrivalTime,
        priority,
        runningTime,
        remainingTime,
        startTime,
        waitingTime,
        lastStopedTime,
        lastStartingTime,
        pid,
        memSize;
};

struct memoryNode{
    int startLocation,
        endLocation,
        size,
        id;
};


struct msgBuff
{
    long mtype;
    struct ProcessStruct process;
};
struct PQNode
{
    struct ProcessStruct *data; // data of Node
    struct PQNode *next;        // pointer for next Node
    int priority;
};
// Queue Struct
struct PQueue
{
    struct PQNode *head, *tail;
};

struct Node
{
    struct ProcessStruct *data; // data of Node
    struct Node *next;          // pointer for next Node
};
// Queue Struct
struct Queue
{
    struct Node *head, *tail;
};

struct sortedLinkedListNode
{
    struct memoryNode* data; // data of Node
    struct sortedLinkedListNode *next;        // pointer for next Node
    int priority;
};

struct sortedLinkedList
{
    struct sortedLinkedListNode *head, *tail;
};