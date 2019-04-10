# 10000x10000 2000x2000 8000x8000

echo "Serial:"

for i in `seq 0 9`
do
    ./a.out 8000 8000 1 6 >> "sort-1.txt"
done
printf "\n" >> "sort-1.txt"


echo "Paralelo Threads:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./a.out 8000 8000 ${i} 7 >> "sort-1.txt"
    done
    printf "\n" >> "sort-1.txt"

done

# printf "\n"  >> "sort.txt"

echo "Paralelo OPENMP:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./a.out 8000 8000 ${i} 8 >> "sort-2.txt"
    done
    printf "\n" >> "sort-2.txt"

done
