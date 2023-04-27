#include "headers.h"
#include "testing_functions.h"
int schedulerPID;

void clearResources(int signum)
{
    msgctl(messageQueueID, IPC_RMID, (struct msqid_ds *)0);
    semctl(processGeneratorAndSchedulerSemID, 0, IPC_RMID, (struct semid_ds *)0);
}

void readFile(struct Queue *processQueue)
{
    FILE *file;
    file = fopen("processes.txt", "r");
    char firstLine[40];
    int id, arrivalTime, priority, runningTime,memSize;

    if (fgets(firstLine, 40, file) == NULL)
    {
        perror("Unable to read the first line from file");
    }

    while (fscanf(file, "%d\t%d\t%d\t%d\t%d", &id, &arrivalTime, &runningTime, &priority,&memSize) != EOF)
    {
        struct ProcessStruct *readProcess = create_process(id, arrivalTime, priority, runningTime,memSize);
        enqueue(processQueue, readProcess);
    }

    // processQueue=testReadFile(processQueue);
   // testLinkedList(processQueue);

    fclose(file);
}

int getSchedulingAlgoFromUser(int *algo)
{
    int quantum = -1;
    printf("Choose the scheduling algorithm:\n");
    printf("1- HPF\n");
    printf("2- SRTN\n");
    printf("3- RR\n");
    scanf("%d", &*algo);
    while (*algo < 1 || *algo > 3)
    {
        printf("Enter a number between 1 and 3:\n");
        scanf("%d", &*algo);
    }
    if (*algo == 3)
    {
        printf("Enter the required quantum:\n");
        scanf("%d", &quantum);
    }
    return quantum;
}
int getMemoryAllocationPolicyFromUser(){
    int memoryPolicy;
    printf("Choose the memory allocation policy:\n");
    printf("1- First Fit\n");
    printf("2- Buddy memory allocation\n");
    scanf("%d",&memoryPolicy);
    while(memoryPolicy<1 || memoryPolicy>2){
        printf("Enter 1 or 2:\n");
        scanf("%d", &memoryPolicy);
    }
    return memoryPolicy;
}

void sendProcess(struct ProcessStruct *process)
{
    struct msgBuff message;
    message.mtype = 5;
    message.process = *process;
    int send = msgsnd(messageQueueID, &message, sizeof(message.process), !IPC_NOWAIT);
    if (send == -1)
        perror("Errror in sending message from process generator to scheduler");
}

int main(int argc, char *argv[])
{

    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct Queue *processQueue = createQueue();
    readFile(processQueue);
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int algo;
    int quantum = getSchedulingAlgoFromUser(&algo);
    int memoryPolicy=getMemoryAllocationPolicyFromUser();

    createSemaphore();
    createMessageQueue();

    int clkPID = fork();
    if (clkPID == 0)
    {
        char *args[] = {"./clk.out", NULL};
        execvp(args[0], args);
    }

    schedulerPID = fork();
    if (schedulerPID == 0)
    {
        char algoAsChar[13], quantumAsChar[13],memoryPolicyAsChar[13];
        sprintf(algoAsChar, "%d", algo);
        sprintf(quantumAsChar, "%d", quantum);
        sprintf(memoryPolicyAsChar,"%d",memoryPolicy);
        char *args[] = {"./scheduler.out", algoAsChar, quantumAsChar,memoryPolicyAsChar, NULL};
        execvp(args[0], args);
    }
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // printf("current time is %d\n", x);
    //  TODO Generation Main Loop
    //  5. Create a data structure for processes and provide it with its parameters.
    //  6. Send the information to the scheduler at the appropriate time.
    //  7. Clear clock resources
    int clk;
    int algorithmBlockingFlag = 1;
    while (!isEmptyQueue(processQueue))
    {
        struct ProcessStruct *process = dequeue(processQueue);
        clk = getClk();
        while (process->arrivalTime > clk)
        {
            clk = getClk();
        }
        printf("Process arrived at arrival time %d and clk %d\n", process->arrivalTime, clk);
        fflush(stdout);
        sendProcess(process);
        kill(schedulerPID, SIGUSR1);
        if (algorithmBlockingFlag && peekQueue(processQueue) != NULL && peekQueue(processQueue)->arrivalTime == clk)
        {
            algorithmBlockingFlag = 0;
            kill(schedulerPID, SIGRTMIN + 2);
        }
        else if (!algorithmBlockingFlag &&
                 ((peekQueue(processQueue) != NULL && peekQueue(processQueue)->arrivalTime != clk) ||
                  isEmptyQueue(processQueue)))
        {
            algorithmBlockingFlag = 1;
            kill(schedulerPID, SIGRTMIN + 2);
        }
        down(processGeneratorAndSchedulerSemID);
    }
    kill(schedulerPID, SIGRTMIN);

    waitpid(schedulerPID, NULL, 0);

    destroyClk(true);
    return 0;
}