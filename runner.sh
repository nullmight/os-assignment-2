#!/usr/bin/env bash

gcc P1.c -o P1.out -lpthread
gcc P2.c -o P2.out -lpthread
gcc S.c -o S.out

./S.out 5 5 5 matrix1.txt matrix2.txt matrixres.txt