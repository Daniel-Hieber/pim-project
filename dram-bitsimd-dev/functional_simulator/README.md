## dram-bitsimd-dev/functional_simulator

* Contributors
  * Deyuan Guo

* Build
```
make
```

* Usage
```
DRAM-BitSIMD functional simulator
Usage: ./main.out <arch>
Supported arch: base, bitwise, bs2r, bs3r, nand, maj3, xnor, simdram, cram
```

* Example outputs
  * Performance stats of 32-bit signed integer addition shown below
    * 64 memory row reads
    * 32 memory row writes
    * 97 bit-serial logical operations
```
INT-ADD: 100 / 100 passed
- STAT (base): int-add=1 (#L=1)
- STAT (bs3r): R=64 W=32 sel=32 set=1 xor=64 (#L=97)
```
