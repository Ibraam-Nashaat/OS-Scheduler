#include "headers.h"

void HPF_process(char* arg)
{
    int remainingTime,previousTime,currentTime;
    previousTime=getClk();
    remainingTime=atoi(arg);

    while (remainingTime > 0)
    {
        currentTime=getClk();
        if(previousTime!=currentTime){
            remainingTime--;
        }
        previousTime=currentTime;
    }
    kill(getppid(),SIGUSR2);
}

void RR_process(char* arg1, char* arg2)
{
    int remainingTime,previousTime,currentTime, quantum;
    previousTime=getClk();
    remainingTime=atoi(arg1);
    quantum = atoi(arg2);

    while (quantum > 0 && remainingTime > 0)
    {
        currentTime=getClk();
        if(previousTime != currentTime){
            quantum--;
            remainingTime--;
        }
        previousTime=currentTime;
    }

    if(!remainingTime)
        kill(getppid(), SIGBUS);
    else
    {
        // send stop process;
        kill(getppid(), SIGURG);
    }    

}

void SRTN_process(char* arg1)
{
    int remainingTime,previousTime,currentTime, quantum;
    previousTime=getClk();
    remainingTime=atoi(arg1);
    quantum = 1;

    while (quantum > 0 && remainingTime > 0)
    {
        currentTime=getClk();
        if(previousTime != currentTime){
            quantum--;
            remainingTime--;
        }
        previousTime=currentTime;
    }

    if(!remainingTime)
        kill(getppid(), SIGIO);
    else
    {
        // send stop process;
        kill(getppid(), SIGIOT);
    }    
}

int main(int argc, char * argv[])
{
    initClk();
    if(argc < 3)
        HPF_process(argv[1]);
    else if (atoi(argv[2]) == -1)
        SRTN_process(argv[1]);
    else
        RR_process(argv[1], argv[2]);

    destroyClk(false);
    
    return 0;
}