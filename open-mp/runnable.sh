# 10000x10000 2000x2000 8000x8000

echo "Serial:"

for i in `seq 0 9`
do
    ./a.out 2000 2000 1 3 >> "mul-1.txt"
done
printf "\n" >> "mul-1.txt"


echo "Paralelo Threads:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./a.out 2000 2000 ${i} 4 >> "mul-1.txt"
    done
    printf "\n" >> "mul-1.txt"

done

# printf "\n"  >> "mul.txt"

echo "Paralelo OPENMP:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./a.out 2000 2000 ${i} 5 >> "mul-2.txt"
    done
    printf "\n" >> "mul-2.txt"

done
