#!/usr/bin/env bash


rm TA_P1_1.csv
rm TA_P1_2.csv
rm TA_P2_1.csv
rm TA_P2_2.csv


gcc P1.c -o P1.out -lpthread
gcc P2.c -o P2.out -lpthread
gcc S.c -o S.out
gcc transpose.c -o transpose.out

array=(50)
for n in "${array[@]}"; do
    rm P1_1_1.csv
    rm P1_1_2.csv
    rm P1_1_3.csv
    rm P1_2_1.csv
    rm P1_2_2.csv
    rm P1_2_3.csv
    rm P2_1_1.csv
    rm P2_1_2.csv
    rm P2_1_3.csv
    rm P2_2_1.csv
    rm P2_2_2.csv
    rm P2_2_3.csv
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