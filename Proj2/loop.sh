#!/bin/bash

for t in 1 2 4 8 12 16 20
do
    for n in 2 4 6 8 10 20 30 40
    do
        g++ -O3 main.cpp -DNUMT=$t -DNUMNODES=$n -o main -lm -fopenmp
        ./main
    done
done
