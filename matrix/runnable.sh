# 10000x10000 2000x2000 10000x10000



echo "Serial:"

for i in `seq 0 9`
do
    ./main 10000 10000 0 0 >> "sum-2.txt"
done
printf "\n" >> "sum-2.txt"


echo "Pralelo:"

for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./main 10000 10000 ${i} 1 >> "sum-2.txt"
    done
    printf "\n" >> "sum-2.txt"

done
printf "\n"
