# 10000x10000 2000x2000 8000x8000

echo "Serial:"

for i in `seq 0 9`
do
    ./a.out 10000 10000 1 0 >> "sum-1.txt"
done
printf "\n" >> "sum-1.txt"


echo "Paralelo Threads:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./a.out 10000 10000 ${i} 1 >> "sum-1.txt"
    done
    printf "\n" >> "sum-1.txt"

done

# printf "\n"  >> "sum.txt"

echo "Paralelo OPENMP:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./a.out 10000 10000 ${i} 2 >> "sum-2.txt"
    done
    printf "\n" >> "sum-2.txt"

done