#!/bin/bash

for n in 1000000 2000000 3000000 4000000 5000000 6000000 7000000 8000000
do
    g++ main.cpp -DARRAYSIZE=$n -o main -lm -fopenmp
    ./main
done
