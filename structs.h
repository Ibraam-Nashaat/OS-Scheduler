struct ProcessStruct
{
    int id,
        arrivalTime,
        priority,
        runningTime;
};

struct msgBuff
{
    long mtype;
    struct ProcessStruct process;
};

struct QNode
{
    struct ProcessStruct *data;
    struct QNode *next;
};

struct Queue
{
    struct QNode *front, *rear;
};

struct PQNode
{
    struct ProcessStruct *data;
    // Lower value indicate higher priority
    int priority;
    struct PQNode *next;
};

struct PQueue
{
    struct PQNode *head;
};