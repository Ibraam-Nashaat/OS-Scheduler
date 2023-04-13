
struct ProcessStruct * runningProcess;
bool isRunning;
void runProcess(struct ProcessStruct *currProcess)
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
         sprintf(remainigTimeChar, "%d", currProcess->runningTime);
         char* argv []={"./process.out",remainigTimeChar,NULL};
         int execlResult = execvp(argv[0], argv);
    }
}
//terminate process ->make isRunning false and free process
void terminateProcess(int sigNum)
{
    isRunning=false;
    free(runningProcess);
    //signal(SIGUSR2, terminateProcess);
    printf("process finished At=%d\n",getClk());
    fflush(stdout);


}
void HPF(struct PQueue *pqueue)
{
    struct PQueue *processQueue; // pointer for priorityQueue
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    processQueue = pqueue;
    while (algorithmFlag||!isEmptyPQ(processQueue) || isRunning) { // while queue isn't empty or runung
        if (isEmptyPQ(processQueue)) // queue is empty doesn't make thing
            continue;
        if (!isRunning) {          // isn't running and in Pqueue --> run it
            readyProcess = peek(processQueue);
            pop(processQueue);
            runningProcess=readyProcess;
            runProcess(readyProcess);
        }
    }

};
