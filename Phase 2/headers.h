#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef short bool;

#include "defines.h"
#include "structs.h"
#include "PQueue.h"
#include "Queue.h"
#include "sortedLinkedList.h"

int messageQueueID;
int processGeneratorAndSchedulerSemID; // The ID of the semaphore between process generator and scheduler
///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    unsigned short *array; /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHMKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHMKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}

// This function creates a message queue using the System V IPC mechanism
// MSQKEY is a key that identifies the message queue
// 0666 is the permission mode for the message queue
// IPC_CREAT is a flag that creates the message queue if it does not exist
void createMessageQueue()
{
    // msgget returns the ID of the message queue associated with MSQKEY
    messageQueueID = msgget(MSQKEY, 0666 | IPC_CREAT);

    // If msgget returns -1, it means an error occurred
    if (messageQueueID == -1)
    {
        // perror prints a descriptive error message to stderr
        perror("Error in creating message queue\n");
        // exit terminates the program with a non-zero status
        exit(-1);
    }
}

// This function creates a semaphore using the System V IPC mechanism
// SEMKEY is a key that identifies the semaphore set
// 1 is the number of semaphores in the set
// 0666 is the permission mode for the semaphore set
// IPC_CREAT is a flag that creates the semaphore set if it does not exist
void createSemaphore()
{
    // semget returns the ID of the semaphore set associated with SEMKEY
    processGeneratorAndSchedulerSemID = semget(SEMKEY, 1, 0666 | IPC_CREAT);
    // If semget returns -1, it means an error occurred
    if (processGeneratorAndSchedulerSemID == -1)
    {
        // perror prints a descriptive error message to stderr
        perror("Error in creating semaphore\n");
        // exit terminates the program with a non-zero status
        exit(-1);
    }

    // semun is a union type that holds the value or operation for the semaphore
    union Semun semun;
    // semun.val is the initial value of the semaphore
    // 0 means the semaphore is locked by default
    semun.val = 0;
    // semctl performs a control operation on the semaphore set
    // processGeneratorAndSchedulerSemID is the ID of the semaphore set
    // 0 is the index of the semaphore in the set (only one in this case)
    // SETVAL is the operation that sets the value of the semaphore to semun.val
    // semun is the argument for the operation
    if (semctl(processGeneratorAndSchedulerSemID, 0, SETVAL, semun) == -1)
    {
        // perror prints a descriptive error message to stderr
        perror("Error in semctl\n");
        // exit terminates the program with a non-zero status
        exit(-1);
    }
}

struct ProcessStruct *create_process(int id, int arrivalTime, int priority, int runningTime,int memSize)
{
    struct ProcessStruct *process = (struct ProcessStruct *)malloc(sizeof(struct ProcessStruct));
    process->id = id;
    process->arrivalTime = arrivalTime;
    process->priority = priority;
    process->runningTime = runningTime;
    process->remainingTime = runningTime;
    process->pid = -1;
    process->startTime = -1;
    process->waitingTime = 0;
    process->lastStopedTime = -1;
    process->memSize=memSize;
    return process;
}

struct memoryNode* createMemoryNode(int startLocation,int size,int pid){
    struct memoryNode *memoryNode=(struct memoryNode *)malloc(sizeof(struct memoryNode));
    memoryNode->startLocation=startLocation;
    memoryNode->endLocation=startLocation+size-1;
    memoryNode->size=size;
    memoryNode->pid=pid;
};