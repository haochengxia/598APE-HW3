make clean; make -j ENABLE_BETTER_OPT=1 ENABLE_OPENMP=1 ENBALE_ALLOCAETED_NEXT=1 ENBALE_BARNES_HUT=1;
# echo "Running with 1000 threads and 5000 iterations";
# ./main.exe 1000 5000;
echo "Running with 10000 threads and 500 iterations";
./main.exe 10000 500;