
# dram_ap_prim

* DRAM-BitSIMD benchmark implementations from Lingxi's GitHub: https://github.com/LingxiWu/DRAMAP-Prim

* Reorganized/reformated source code
  * BS/baselines/cpu/bs_omp.c
  * MLP/baselines/cpu/mlp_openmp.c
  * RED/baselines/cpu/app_baseline.cpp
  * SEL/baselines/cpu/app_baseline.c
  * VA/baselines/cpu/app_baseline.c

### How to build and run

* For BS / MLP / SEL / VA
  * Goto baselines/cpu/
  * make
* For RED
  * Goto RED/baselines/cpu/
  * Config Makefile with cuda toolkit include directory `-I<path-to-cuda>/include` for thrust header files
  * Run `TYPE=UINT64 make`

### Benchmarks available for DRAM-BitSIMD

* BS
* MLP
* RED
* SEL
* VA

