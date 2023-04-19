#include "headers.h"
int remainingTime, previousTime, currentTime, quantum;

int main(int argc, char *argv[])
{
    initClk();
    remainingTime = atoi(argv[1]);

    while (remainingTime > 0)
    {
        currentTime = getClk();
        if (currentTime - previousTime == 1)
            {
                remainingTime--;
                kill(getppid(), SIGUSR2);
                kill(getpid(),SIGSTOP);
                currentTime = getClk();
            }
        previousTime = currentTime;
    }
    
    kill(getppid(), SIGALRM);
    destroyClk(false);
    return 0;
}