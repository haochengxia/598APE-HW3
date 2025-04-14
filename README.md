# 598APE-HW3
This repository contains optimized code for homework 3 of 598APE.

You can select features by editing the Makefile, all optimizations are enabled by default.

## Artifacts evaluation instructions

We use  `./main.exe 10000 500` for evaluation, where `nplanets` is 10000 and `timesteps` is 500.

### How to run

For the results of Figure 1,

```
bash ae/3_openmp.sh
```

For the results of Table 1,

```
bash ae/0_original.sh
bash ae/1_opt.sh
bash ae/2_mem.sh
bash ae/3_openmp.sh
bash ae/4_final.sh  # Barnes-Hut Approximation
```

For the results of Figure 2 (in Appendix),

```
bash ae/analysis.sh
cd analysis
python plot.py  # It can take you more than twenty minutes; skip it if you don't want
```

---
# Upstream

# 598APE-HW3

This repository contains code for homework 3 of 598APE.

This assignment is relatively simple in comparison to HW1 and HW2 to ensure you have enough time to work on the course project.

In particular, this repository is an implementation of an n-body simulator.

To compile the program run:
```bash
make -j
```

To clean existing build artifacts run:
```bash
make clean
```

This program assumes the following are installed on your machine:
* A working C compiler (g++ is assumed in the Makefile)
* make

The nbody program is a classic physics simulation whose exact results are unable to be solved for exactly through integration.

Here we implement a simple time evolution where each iteration advances the simulation one unit of time, according to Newton's law of gravitation.

Once compiled, one can call the nbody program as follows, where nplanets is the number of randomly generated planets for the simulation, and timesteps denotes how long to run the simulation for:
```bash
./main.exe <nplanets> <timesteps>
```

In particular, consider speeding up simple run like the following (which runs ~6 seconds on my local laptop under the default setup):
```bash
./main.exe 1000 5000
```

Exact bitwise reproducibility is not required, but approximate correctness (within a reasonable region of the final location).