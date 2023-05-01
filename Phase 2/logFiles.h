FILE *memoryLogFile;

void addMemoryAllocation(int time, int bytes, int processId, int from, int to){
    fprintf(memoryLogFile, "At time %d allocated %d bytes for process %d from %d to %d\n", time, bytes, processId, from, to);
}

void addMemoryDeallocation(int time, int bytes, int processId, int from, int to){
    fprintf(memoryLogFile, "At time %d freed %d bytes for process %d from %d to %d\n", time, bytes, processId, from, to);
}