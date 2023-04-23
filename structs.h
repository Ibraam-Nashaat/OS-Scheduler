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
    pid;
};

struct msgBuff
{
    long mtype;
    struct ProcessStruct process;
};
struct PQNode
{
    struct ProcessStruct* data; // data of Node
    struct PQNode * next;  // pointer for next Node
    int priority;
};
//Queue Struct
struct PQueue
{
    struct PQNode * head ,* tail;
};

struct Node
{
    struct ProcessStruct*  data; // data of Node
    struct Node * next;  // pointer for next Node
};
//Queue Struct
struct Queue
{
    struct Node * head ,* tail;
};