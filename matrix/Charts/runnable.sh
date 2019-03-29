# 10000x10000 2000x2000 8000x8000

echo "Serial:"

for i in `seq 0 9`
do
    ./main 2000 2000 0 2 >> "mul.txt"
done
printf "\n" >> "mul.txt"


echo "Pralelo:"

for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./main 2000 2000 ${i} 3 >> "mul.txt"
    done
    printf "\n" >> "mul.txt"

done
printf "\n"
