#!/bin/bash
gcc scheduler.c -o scheduler.out
gcc process_generator.c -o process_generator.out
gcc clk.c -o clk.out
gcc process.c -o process.out
./process_generator.out

# don't forget to run chmod u+x compileAndRun.sh