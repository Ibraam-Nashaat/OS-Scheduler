FILE *memoryLogFile;
FILE *schedulerLogFile;

void addMemoryAllocation(int time, int bytes, int processId, int from, int to){
    fprintf(memoryLogFile, "At time %d allocated %d bytes for process %d from %d to %d\n", time, bytes, processId, from, to);
}

void addMemoryDeallocation(int time, int bytes, int processId, int from, int to){
    fprintf(memoryLogFile, "At time %d freed %d bytes for process %d from %d to %d\n", time, bytes, processId, from, to);
}

void logProcessStart(int time, int processId, int arrivalTime, int runningTime, int remainingTime, int waitingTime){
    fprintf(schedulerLogFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", time, processId, arrivalTime, runningTime, remainingTime, waitingTime);
}

void logProcessResume(int time, int processId, int arrivalTime, int runningTime, int remainingTime, int waitingTime){
    fprintf(schedulerLogFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", time, processId, arrivalTime, runningTime, remainingTime, waitingTime);
}

void logProcessFinish(int time, int processId, int arrivalTime, int runningTime, int remainingTime, int waitingTime, int turnaround, float weightedTAT){
    fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, processId, arrivalTime, runningTime, remainingTime, waitingTime, turnaround, weightedTAT);
}

void logProcessStop(int time, int processId, int arrivalTime, int runningTime, int remainingTime, int waitingTime){
    fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", time, processId, arrivalTime, runningTime, remainingTime, waitingTime);
}