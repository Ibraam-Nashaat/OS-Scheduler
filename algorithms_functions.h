int algorithmFlag = 1;
int childProcessPID;
int selectedAlgorithm;
int quantum;
int currQuantum;
int previous_id;
bool isRunning;
struct msgBuff message;
struct PQueue *priorityQueue;
struct Queue *queue;
struct ProcessStruct * runningProcess = NULL;
struct ProcessStruct * previousProcess = NULL;


void runProcess(struct ProcessStruct *currProcess,int quantum)
{
    runningProcess=currProcess;
    isRunning = true;
    //runningProcess->lastStartedTime = getClk();
    if(runningProcess->pid != -1) // if the process started before , send SIGRTMIN+5 to make it continue its execution
    {
        printf("Process with id %d and pid %d continued, clk %d\n", runningProcess->id,runningProcess->pid, getClk());
        kill(runningProcess->pid,SIGCONT);
        previous_id = runningProcess->id;
        return;
    }
    int pid = fork();
    if (pid == -1) {
        perror("Error in execl");
        exit(-1);
    }
    if(pid==0) // make new process
    {
        childProcessPID=getpid();
        printf("process %d at time %d \n", runningProcess->id, getClk());
        fflush(stdout);
        char remainigTimeChar[13];
        sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
        char quantumChar[13];
        sprintf(quantumChar,"%d",quantum);
        char* argv []={"./process.out",remainigTimeChar,quantumChar,NULL}; // send remaining time as an argument
        int execlResult = execvp(argv[0], argv);
    }    
    runningProcess->pid = pid;
    previous_id = runningProcess->id;
    runningProcess->startTime=getClk();
    runningProcess->lastStopedTime=getClk();
}

/*
This function deletes the process and make isRunning=false to execute another process. 
*/
void terminateProcess(int sigNum)
{
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    runningProcess = NULL;
    isRunning=false;
    fflush(stdout);
}
/*
This function blocks the process, put it at the end of the queue and make isRunning=false to begin another process. 
*/
void blockProcess()
{
    if(selectedAlgorithm == 3)
        {            
            enqueue(queue,runningProcess);
        }
        else
        {
            runningProcess->lastStopedTime=getClk();
            kill(runningProcess->pid,SIGRTMIN+4);
            kill(runningProcess->pid,SIGSTOP);
            push(priorityQueue,runningProcess,runningProcess->remainingTime);
        }
    
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    
    isRunning=false;
    fflush(stdout);
}


