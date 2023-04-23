#include "headers.h"
#include "defines.h"

// Flag to indicate if an interruption occurred
bool interruptionFlag = 0;

// Handler function for interruption signal
void interruptionHandler(int signum){
    interruptionFlag = 1;
}

int main(int argc, char *argv[])
{
    int remainingTime, previousTime, currentTime, quantum, currentQuantum;
    initClk();
    // Parse arguments for remaining time and quantum
    remainingTime = atoi(argv[1]);
    quantum = currentQuantum = atoi(argv[2]);
    previousTime = getClk();
    // Register handler for interruption signal
    signal(INTERRUPT_SIGNAL, interruptionHandler);

    while (remainingTime > 0)
    {
        currentTime = getClk();
        if(interruptionFlag){
            // Reset flag and time if interrupted
            interruptionFlag = 0;
            previousTime = currentTime = getClk();
        }
        if (currentTime != previousTime )
            {
                // Decrease remaining time and current quantum
                remainingTime--;
                if(currentQuantum != -1) currentQuantum--;
                if(currentQuantum == 0) {
                    // Send signal to parent process that quantum expired
                    kill(getppid(), QUANTUM_FINISHED);
                    // Stop the process until resumed
                    kill(getpid(), SIGSTOP);
                    // Reset current quantum
                    currentQuantum = quantum;
                }
                currentTime = getClk();
            }
        previousTime = currentTime;
    }
    
    // Send signal to parent process that process finished
    kill(getppid(), PROCESS_FINISHED_SIGNAL);
    destroyClk(false);
    return 0;
}