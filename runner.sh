#!/usr/bin/env bash

gcc P1.c -o P1.out -lpthread
gcc P2.c -o P2.out -lpthread
gcc S.c -o S.out
gcc transpose.c -o transpose.out

array=(10 50 100)
for n in "${array[@]}"; do
    q=$n
    m=$n
    python3 MatrixGen.py $n $q $m
    wait $!
    ./S.out $n $m $q inp1.txt inp2.txt out.txt
    wait $!
    python3 plot.py $n $q $m
    wait $!
    echo $n $q $m done
done
# ./S.out 5 5 5 inp1.txt inp2.txt out.txt
# ./P1.out 5 5 5 matrix1.txt matrix2.txt P1_1.csv 1 1
# ./P2.out 5 5 5 matrixres.txt P2_1.csv 1 1