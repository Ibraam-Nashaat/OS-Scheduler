#include "defines.h"
#include"structs.h"
int algorithmFlag = 1;
int algorithmBlockingFlag = 1; // for handling processes that arrive at the same time
int selectedAlgorithm, quantum,memoryPolicy;
bool isRunning;
struct msgBuff message;
struct PQueue *priorityQueue;
struct Queue *queue,*processWaitingQueue;
struct ProcessStruct *runningProcess = NULL;
struct sortedLinkedList* memoryHoles, *memoryUsed;

// Run a process with a given quantum time
// currProcess: a pointer to the process structure
// quantum: the quantum time for RR algorithm
void runProcess(struct ProcessStruct *currProcess, int quantum)
{
    runningProcess = currProcess;
    isRunning = true;
    // runningProcess->lastStartedTime = getClk();
    if (runningProcess->pid != -1) // if the process started before, send CONTINUE_PROCESS signal to make it continue its execution
    {
        printf("Process with id %d and pid %d continued, clk %d\n", runningProcess->id, runningProcess->pid, getClk());
        kill(runningProcess->pid, CONTINUE_PROCESS);
        return;
    }
    int pid = fork();
    if (pid == -1)
    {
        perror("Error in fork");
        exit(-1);
    }
    if (pid == 0) // make new process
    {
        printf("process %d started at time %d \n", runningProcess->id, getClk());
        fflush(stdout);
        char remainigTimeChar[13];
        sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
        char quantumChar[13];
        sprintf(quantumChar, "%d", quantum);
        char *argv[] = {"./process.out", remainigTimeChar, quantumChar, NULL}; // send remaining time and quantum as arguments
        int execlResult = execvp(argv[0], argv);
        if (execlResult == -1)
        {
            perror("Error in execvp");
            exit(-1);
        }
    }
    runningProcess->pid = pid;
    runningProcess->startTime = getClk();
    runningProcess->lastStopedTime = getClk();
}
void deAllocateProcessMemory(struct ProcessStruct *process )
{
  struct sortedLinkedListNode* llNode= find(memoryUsed->head,process->pid);
  struct sortedLinkedListNode* rmNode=removeLinkedListNode(llNode,memoryUsed);
  rmNode->data->pid=-1;
 // memoryHoles->head->data->size=memoryHoles->head->data->size -rmNode->data->size;
  //memoryHoles->head->data->startLocation=memoryHoles->head->data->startLocation+rmNode->data->size;
  insert(memoryHoles,rmNode->data,rmNode->data->startLocation);
} 
void reAllocateProcessMemory(struct ProcessStruct *Process){
struct memoryNode * memNode=createMemoryNode(memoryHoles->head->data->startLocation,Process->memSize,Process->pid);
struct sortedLinkedListNode* currNode=memoryHoles->head;
while(memNode->size >currNode->data->size && currNode!=NULL )
{
currNode=currNode->next;
}
if(currNode ==NULL)
{
    printf("NO memory available\n");
    return ;
}
if(memNode->size==currNode->data->size)
{
   //struct sortedLinkedListNode* holedNode=removeLinkedListNode(currNode,memoryHoles);
   insert (memoryUsed,memNode,Process->priority);
}
else{
  struct sortedLinkedListNode* secCurrNode=splitNode(currNode,Process->memSize);
 // struct sortedLinkedListNode* holedNode=removeLinkedListNode(secCurrNode,memoryHoles);

    insert (memoryUsed,memNode,Process->priority);
}

}
// This function terminates the process and frees the memory
// sigNum: the signal number for termination
void terminateProcess(int sigNum)
{
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    runningProcess = NULL;
    isRunning = false;
    fflush(stdout);
    deAllocateProcessMemory(runningProcess);
    reAllocateProcessMemory(runningProcess);
}

// This function blocks the process, puts it at the end of the queue or the priority queue depending on the algorithm, and makes isRunning=false to begin another process.
void blockProcess()
{
    if (selectedAlgorithm == 3)
    {
        // For round robin algorithm, enqueue the process at the end of the queue
        // queue: a pointer to the queue structure
        // runningProcess: a pointer to the process structure
        enqueue(queue, runningProcess);
    }
    else
    {
        // For shortest remaining time next algorithm, update the last stopped time and the remaining time of the process, and push it to the priority queue
        // priorityQueue: a pointer to the priority queue structure
        // runningProcess: a pointer to the process structure
        runningProcess->lastStopedTime = getClk();
        kill(runningProcess->pid, INTERRUPT_SIGNAL);
        kill(runningProcess->pid, SIGSTOP);
        push(priorityQueue, runningProcess, runningProcess->remainingTime);
    }
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    isRunning = false;
    fflush(stdout);
}