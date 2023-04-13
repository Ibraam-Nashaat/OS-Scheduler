
struct ProcessStruct * runningProcess;
struct Queue *processQueue; // pointer for Queue
bool isRunning;
int quantum;
void runProcessRR(struct ProcessStruct *currProcess)
{
    runningProcess=currProcess;
    isRunning=true;
    
    int pid = fork();
    if (pid == -1) {
        perror("Error in execl");
        exit(-1);
    }
    if(pid==0)
    {
        childProcessPID=getpid();
         printf("id =  %d started pid = %d current time = %d \n", currProcess->id, getpid(), getClk());
         char remainigTimeChar[13];
         char quantumChar[13];
         sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
         sprintf(quantumChar, "%d", quantum);
         char* argv []={"./process.out",remainigTimeChar, quantumChar,NULL};
         int execlResult = execvp(argv[0], argv);
    }
    runningProcess->remainingTime-=quantum;
}
//terminate process ->make isRunning false and free process
void terminateProcessRR(int sigNum)
{
    isRunning=false;
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    //signal(SIGUSR2, terminateProcess);
    fflush(stdout);
}

void BlockProcessRR(int sigNum)
{
    isRunning=false;
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, getpid(), runningProcess->remainingTime, getClk());
    enqueue(processQueue,runningProcess);
    fflush(stdout);
}

 
void RR(struct Queue *queue, int quant)
{
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    processQueue = queue;
    quantum = quant;
    while (algorithmFlag||!isEmptyQueue(processQueue) || isRunning) { // while queue isn't empty or runung
        if (isEmptyQueue(processQueue)) // queue is empty doesn't make thing
            continue;
        if (!isRunning) {          // isn't running and in Pqueue --> run it
            readyProcess = dequeue(processQueue);
            //pop(processQueue);
            runningProcess=readyProcess;
            runProcessRR(readyProcess);
        }
    }
};
