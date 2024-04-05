
# dram_ap_phoenix

* DRAM-BitSIMD benchmark implementations from Lingxi's GitHub: https://github.com/LingxiWu/DRAMAP-Phoenix

* Reorganized/reformated source code
  * phoenix-2.0/tests: Orignial phoenix implementation
  * phoenix-2.0/tests_modified: Orignial phoenix implementation, with extra time measurement code in `*-seq.c` and `*-pthread.c` (Guarded with DRAMAP flag)
    * histogram
    * string_match
    * word_count
  * phoenix-2.0/tests_bitsimd: Implementation for DRAM-AP (BitSIMD) in `*-dramap.c` files (Guarded with DRAMAP flag)
    * histogram
    * kmeans
    * linear_regression
    * matrix_multiply
    * pca
    * string_match

### How to build and run

* Run make in phoenix-2.0 directory
  * Need Linux environment
* Download datasets separately (see phoenix/README.md)
  * wget URL
* Goto tests directory and run each testcase

### Benchmarks available for DRAM-BitSIMD

* histogram
* kmeans
* linear_regression
* matrix_multiply
* pca
* string_match

