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
    for b in 32 64 128 256
    do
        g++ second.cpp -lOpenCL -lm -fopenmp -std=c++11 -DNMB=$t -DLOCAL_SIZE=$b  -o second
        ./second
    done
done
