export OMP_NUM_THREADS=1;
make clean; make -j ENABLE_BETTER_OPT=1 ENABLE_OPENMP=1 ENBALE_ALLOCAETED_NEXT=1 ENBALE_BARNES_HUT=0;
echo "NUMBER OF THREADS: $OMP_NUM_THREADS";
# echo "Running with 1000 threads and 5000 iterations";
# ./main.exe 1000 5000;
echo "Running with 10000 threads and 500 iterations";
./main.exe 10000 500;

export OMP_NUM_THREADS=2;
echo "NUMBER OF THREADS: $OMP_NUM_THREADS";
# echo "Running with 1000 threads and 5000 iterations";
# ./main.exe 1000 5000;
echo "Running with 10000 threads and 500 iterations";
./main.exe 10000 500;

export OMP_NUM_THREADS=3;
echo "NUMBER OF THREADS: $OMP_NUM_THREADS";
# echo "Running with 1000 threads and 5000 iterations";
# ./main.exe 1000 5000;
echo "Running with 10000 threads and 500 iterations";
./main.exe 10000 500;

export OMP_NUM_THREADS=4;
echo "NUMBER OF THREADS: $OMP_NUM_THREADS";
# echo "Running with 1000 threads and 5000 iterations";
# ./main.exe 1000 5000;
echo "Running with 10000 threads and 500 iterations";
./main.exe 10000 500;