#!/bin/bash
#SBATCH -J MonteCarlo
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --constraint=v100
#SBATCH --gres=gpu:1
#SBATCH -o montecarlo.out
#SBATCH -e montecarlo.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=leejuic@oregonstate.edu

for t in 1 4 16 64
do
    for b in 8 32 128 512
    do
        g++ first.cpp -lOpenCL -lm -fopenmp -std=c++11 -DNMB=$t -DLOCAL_SIZE=$b  -o first
        ./first
    done
done
