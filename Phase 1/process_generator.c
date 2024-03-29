#include "headers.h"
int schedulerPID;

int totalRunningTime = 0;

void clearResources(int signum)
{
    msgctl(messageQueueID, IPC_RMID, (struct msqid_ds *)0);
    semctl(processGeneratorAndSchedulerSemID, 0, IPC_RMID, (struct semid_ds *)0);
}

void readFile(struct Queue *processQueue, char *fileName)
{
    FILE *file;
    file = fopen(fileName, "r");
    char firstLine[40];
    int id, arrivalTime, priority, runningTime;

    if (fgets(firstLine, 40, file) == NULL)
    {
        perror("Unable to read the first line from file");
    }

    while (fscanf(file, "%d\t%d\t%d\t%d", &id, &arrivalTime, &runningTime, &priority) != EOF)
    {
        totalRunningTime += runningTime;
        struct ProcessStruct *readProcess = create_process(id, arrivalTime, priority, runningTime);
        enqueue(processQueue, readProcess);
    }

    // processQueue=testReadFile(processQueue);

    fclose(file);
}
bool readFromCmd(int argc, char *argv[], int *algo, int *quantum, char **fileName)
{
    if (argc < 4 || argc >6)
    {
        printf("The CMD arguments should be 4 or 6\n");
        return false;
    }
    *fileName = argv[1];

    if (strcmp(argv[2], "-sch") == 0)
    {
        *algo = atoi(argv[3]);
        if (*algo > 3 || *algo < 1)
        {
            printf("The scheduling algorithm number that you entered is invalid\n");
            return false;
        }
    }
    else
    {
        printf("The CMD arguments doesn't match the required format\n");
        return false;
    }
    /***
     * -----------------------------------------------------------------
     */
    if (argc == 6)
    {
        if (strcmp(argv[4], "-q") == 0)
        {
            *quantum = atoi(argv[5]);
            if (*quantum < 0)
            {
                printf("The quantum should be greater than zero\n");
                return false;
            }
        }
        else
        {
            printf("The CMD arguments doesn't match the required format\n");
            return false;
        }
    }
    else if(*algo==3 && argc<6)
    {
        printf("The quantum of RR is not specified\n");
        return false;
    }
    return true;
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
    int algo, quantum, memoryPolicy;
    char *fileName;
    if (!readFromCmd(argc, argv, &algo, &quantum, &fileName))
        return 0;
    struct Queue *processQueue = createQueue();
    readFile(processQueue,fileName);

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
        char algoAsChar[13], quantumAsChar[13], totalRunningTimeAsChar[13];
        sprintf(algoAsChar, "%d", algo);
        sprintf(quantumAsChar, "%d", quantum);
        sprintf(totalRunningTimeAsChar, "%d", totalRunningTime);
        char *args[] = {"./scheduler.out", algoAsChar, quantumAsChar, totalRunningTimeAsChar, NULL};
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