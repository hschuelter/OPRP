# 10000x10000 2000x2000 8000x8000

echo "Serial:"

for i in `seq 0 9`
do
    ./main 8000 8000 1 0 >> "sum-2.txt"
done
printf "\n" >> "sum-2.txt"


echo "Pralelo:"
for i in `seq 1 8`
do
    for j in `seq 0 9`
    do
        ./main 8000 8000 ${i} 1 >> "sum-2.txt"
    done
    printf "\n" >> "sum-2.txt"

done
# printf "\n"
