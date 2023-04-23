#include "headers.h"
int main(int argc, char *argv[])
{
    int remainingTime, previousTime, currentTime, quantum,currQuantum;
    initClk();
    remainingTime = atoi(argv[1]);
    quantum=currQuantum=atoi(argv[2]);
    previousTime=getClk();

    while (remainingTime > 0)
    {
        currentTime = getClk();
        if (currentTime != previousTime )
            {
                remainingTime--;
                if(currQuantum!=-1)currQuantum--;
                if(currQuantum==0) {
                    kill(getppid(),SIGRTMIN+1);
                    kill(getpid(),SIGSTOP);
                    currQuantum=quantum;
                }
                currentTime = getClk();
            }
        previousTime = currentTime;
    }
    
    kill(getppid(), SIGUSR2);
    destroyClk(false);
    return 0;
}