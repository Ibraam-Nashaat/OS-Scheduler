#include "headers.h"
int remainingTime, previousTime, currentTime, quantum;

int main(int argc, char *argv[])
{
    initClk();
    remainingTime = atoi(argv[1]);
    previousTime=getClk();

    while (remainingTime > 0)
    {
        currentTime = getClk();
        if (currentTime != previousTime )
            {
              //  printf("remaining time %d\n",remainingTime);
                remainingTime--;
                kill(getppid(), SIGRTMIN+1);
               // kill(getpid(),SIGSTOP);
                currentTime = getClk();
            }
       // printf("The time is %d and my remaining time is %d\n",getClk(),remainingTime);
        fflush(stdout);
        previousTime = currentTime;
    }
    
    kill(getppid(), SIGUSR2);
    destroyClk(false);
    return 0;
}