make clean; make -j ENABLE_BETTER_OPT=0 ENABLE_OPENMP=0 ENBALE_ALLOCAETED_NEXT=0 ENBALE_BARNES_HUT=0;
cp main.exe ./analysis/0_original.exe;

make clean; make -j ENABLE_BETTER_OPT=1 ENABLE_OPENMP=0 ENBALE_ALLOCAETED_NEXT=0 ENBALE_BARNES_HUT=0;
cp main.exe ./analysis/1_opt.exe;

make clean; make -j ENABLE_BETTER_OPT=1 ENABLE_OPENMP=0 ENBALE_ALLOCAETED_NEXT=1 ENBALE_BARNES_HUT=0;
cp main.exe ./analysis/2_mem.exe;

make clean; make -j ENABLE_BETTER_OPT=1 ENABLE_OPENMP=1 ENBALE_ALLOCAETED_NEXT=1 ENBALE_BARNES_HUT=0;
cp main.exe ./analysis/3_openmp.exe;

make clean; make -j ENABLE_BETTER_OPT=1 ENABLE_OPENMP=1 ENBALE_ALLOCAETED_NEXT=1 ENBALE_BARNES_HUT=1;
cp main.exe ./analysis/4_final.exe;