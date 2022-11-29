#!/usr/bin/env bash

gcc P1.c -o P1.out -lpthread
gcc P2.c -o P2.out -lpthread
gcc S.c -o S.out


./S.out 5 5 5 inp1.txt inp2.txt out.txt 200000
# ./P1.out 5 5 5 matrix1.txt matrix2.txt P1_1.csv 1 1
# ./P2.out 5 5 5 matrixres.txt P2_1.csv 1 1