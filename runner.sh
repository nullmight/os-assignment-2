#!/usr/bin/env bash
rm TA_P1_1.csv
rm TA_P1_2.csv
rm TA_P2_1.csv
rm TA_P2_2.csv
rm WT_1.csv
rm WT_2.csv
rm CS_1.csv
rm CS_2.csv

gcc P1.c -o P1.out -lpthread
gcc P2.c -o P2.out -lpthread
gcc S.c -o S.out
gcc transpose.c -o transpose.out

array=(5 10 15 20 25 30 35 40 45 50)
for n in "${array[@]}"; do
    q=$n
    m=$n
    python3 MatrixGen.py $n $q $m
    wait $!
    ./S.out $n $m $q inp1.txt inp2.txt out.txt
    wait $!
    python3 Plot.py $n $q $m
    wait $!
    if cmp --silent -- "out.txt" "res.txt"; then
        echo "Output matrix is correct"
    else
        echo "Incorrect output matrix"
        break
    fi
    echo $n $q $m done

done
python3 TotPlot.py
