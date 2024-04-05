// File: Logic Unit Class - DRAM-BitSIMD V 2-reg
// Author: Deyuan Guo
// Date: June 2023

#ifndef LU_BS2R_H
#define LU_BS2R_H

#include <map>
#include <string>
#include "lu.h"

class lu_bs2r : public lu
{
public:
    lu_bs2r() : _sa(0), _cr(0), _pr(0) {}
    virtual ~lu_bs2r() {}
    virtual std::string getName() { return "bs2r"; }

protected:
    virtual B32 logic_not(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _sa = op_not(_sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 logic_and(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_and(_sa, _cr);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 logic_or(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_or(_sa, _cr);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 logic_xor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_xor(_sa, _cr);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 logic_nand(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_and(_sa, _cr);
            _sa = op_not(_sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 logic_nor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_or(_sa, _cr);
            _sa = op_not(_sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 logic_xnor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_xor(_sa, _cr);
            _sa = op_not(_sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B8 pop_count(B32 a) {
        B32 tmp;
        // 2 bits -> 2-bit count
        for (int i = 0; i < 32; i += 2) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(a[i + 1]);
            _pr = op_move(_sa);
            _sa = op_xor(_cr, _pr);
            tmp.set(i, op_write(_sa));
            _sa = op_and(_cr, _pr);
            tmp.set(i + 1, op_write(_sa));
        }
        // aggregate from 2-bit count to 6-bit count
        for (int iter = 2; iter <= 5; iter++) {
            for (int i = 0; i < 32; i += (1 << iter)) {
                _cr = op_set(0);
                for (int j = 0; j < iter; j++) {
                    _sa = op_read(tmp[i + j]);
                    _pr = op_xor(_sa, _cr);
                    _sa = op_read(tmp[i + (1 << (iter - 1)) + j]);
                    _cr = op_sel(_pr, _sa, _cr);
                    _sa = op_xor(_pr, _sa);
                    tmp.set(i + j, op_write(_sa));
                }
                _sa = op_move(_cr);
                tmp.set(i + iter, op_write(_sa));
            }
        }
        _sa = op_set(0);
        tmp.set(6, op_write(_sa));
        tmp.set(7, op_write(_sa));

        // return first 8 bits
        B8 res(static_cast<unsigned char>(tmp.to_uint()));
        return res;
    }

    virtual B32 int_add(B32 a, B32 b) {
        B32 res;
        _cr = op_set(0);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 int_sub(B32 a, B32 b) {
        B32 res;
        _cr = op_set(0);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(_pr, _cr, _sa);
            _sa = op_xor(_pr, _sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 int_abs(B32 a) {
        B32 res;
        _sa = op_read(a[31]);
        _cr = op_move(_sa);
        _pr = op_move(_sa);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _sa = op_xor(_sa, _pr);
            _sa = op_xor(_sa, _cr);
            res.set(i, op_write(_sa));
            _sa = op_not(_sa);
            _cr = op_and(_sa, _cr);
        }
        return res;
    }
    virtual B32 int_min(B32 a, B32 b) {
        B1 cmp = int_lt(a, b);
        B32 res;
        res = if_else(cmp, a, b);
        return res;
    }
    virtual B32 int_max(B32 a, B32 b) {
        B1 cmp = int_gt(a, b);
        B32 res;
        res = if_else(cmp, a, b);
        return res;
    }
    virtual B64 uint_mul(B32 a, B32 b) {
        if (1) {
            B64 res;
            //std::cout << " =========================================================== " << std::endl;
            res = karatsuba_mul(a, b, 32);
            //std::cout << "FINAL: " << a.to_uint() << " * " << b.to_uint() << " = " << res.to_ullong() << std::endl;
            return res;
        }
        return uint_mul_helper(a, b, 32);
    }

    virtual B32 uint_div(B32 a, B32 b) {
        B64 qr = uint_div_rem(a, b);
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(qr[i + 32]);
            res.set(i, op_write(_sa));
        }
        return res;
    }
    virtual B32 uint_rem(B32 a, B32 b) {
        B64 qr = uint_div_rem(a, b);
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(qr[i]);
            res.set(i, op_write(_sa));
        }
        return res;
    }

    virtual B1 uint_gt(B32 a, B32 b) {
        B1 res;
        _cr = op_set(0);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(b[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(a[i]);
            _cr = op_sel(!_pr, _sa, _cr);
        }
        _sa = op_move(_cr);
        res.set(0, op_write(_sa));
        return res;
    }
    virtual B1 uint_lt(B32 a, B32 b) {
        B1 res;
        _cr = op_set(0);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(!_pr, _sa, _cr);
        }
        _sa = op_move(_cr);
        res.set(0, op_write(_sa));
        return res;
    }
    virtual B1 int_gt(B32 a, B32 b) {
        B1 res;
        _cr = op_set(0);
        for (int i = 0; i < 31; i++) {
            _sa = op_read(b[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(a[i]);
            _cr = op_sel(!_pr, _sa, _cr);
        }
        // 31-bit uint gt cmp result in _cr
        _sa = op_read(a[31]);
        _pr = op_move(_sa);
        _sa = op_read(b[31]);
        _pr = op_xor(_sa, _pr);
        _sa = op_sel(_pr, _sa, _cr);
        res.set(0, op_write(_sa));
        return res;
    }
    virtual B1 int_lt(B32 a, B32 b) {
        B1 res;
        _cr = op_set(0);
        for (int i = 0; i < 31; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(!_pr, _sa, _cr);
        }
        // 31-bit uint lt cmp result in _cr
        _sa = op_read(b[31]);
        _pr = op_move(_sa);
        _sa = op_read(a[31]);
        _pr = op_xor(_sa, _pr);
        _sa = op_sel(_pr, _sa, _cr);
        res.set(0, op_write(_sa));
        return res;
    }
    virtual B1 int_eq(B32 a, B32 b) {
        B1 res;
        _pr = op_set(0);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _cr = op_xor(_sa, _cr);
            _pr = op_or(_pr, _cr);
        }
        _sa = op_not(_pr);
        res.set(0, op_write(_sa));
        return res;
    }

    virtual B32 float_add(B32 a, B32 b) { return a.to_float() + b.to_float(); }
    virtual B32 float_sub(B32 a, B32 b) { return a.to_float() - b.to_float(); }
    virtual B32 float_mul(B32 a, B32 b) { return a.to_float() * b.to_float(); }
    virtual B32 float_div(B32 a, B32 b) { return a.to_float() / b.to_float(); }

    virtual B32 copy(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            res.set(i, op_write(_sa));
        }
        return res;
    }

    virtual B1 search(B32 a, B32 p) {
        B1 res;
        _pr = op_set(0);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_set(p[i]); // encode pattern in micro-ops sequence
            _cr = op_xor(_sa, _cr);
            _pr = op_or(_pr, _cr);
        }
        _sa = op_not(_pr);
        res.set(0, op_write(_sa));
        return res;
    }

    virtual B32 if_else(B1 c, B32 a, B32 b) {
        B32 res;
        _sa = op_read(c[0]);
        _pr = op_move(_sa);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _cr = op_move(_sa);
            _sa = op_read(b[i]);
            _sa = op_sel(_pr, _cr, _sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }

    virtual B32 int_ReLU(B32 a) {
        B32 res;
        _pr = op_read(a[31]);
        _pr = op_not(_pr);
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[i]);
            _sa = op_and(_sa, _pr);
            res.set(i, op_write(_sa));
        }
        return res;
    }

    virtual B32 uint_var_lshift(B32 a, B32 n) {
        // copy a to res
        B32 res = copy(a);
        // only use the lowest 5 bits of n for conditional shift
        for (int i = 0; i < 5; i++) {
            _sa = op_read(n[i]);
            _pr = op_move(_sa);
            int step = (1 << i);
            for (int j = 31; j >= step; j--) {
                _sa = op_read(res[j - step]);
                _cr = op_move(_sa);
                _sa = op_read(res[j]);
                _sa = op_sel(_pr, _cr, _sa);
                res.set(j, op_write(_sa));
            }
            _cr = op_set(0);
            for (int j = 0; j < step; j++) {
                _sa = op_read(res[j]);
                _sa = op_sel(_pr, _cr, _sa);
                res.set(j, op_write(_sa));
            }
        }
        return res;
    }
    virtual B32 uint_var_rshift(B32 a, B32 n) {
        // copy a to res
        B32 res = copy(a);
        // only use the lowest 5 bits of n for conditional shift
        for (int i = 0; i < 5; i++) {
            _sa = op_read(n[i]);
            _pr = op_move(_sa);
            int step = (1 << i);
            for (int j = 0; j < 32 - step; j++) {
                _sa = op_read(res[j + step]);
                _cr = op_move(_sa);
                _sa = op_read(res[j]);
                _sa = op_sel(_pr, _cr, _sa);
                res.set(j, op_write(_sa));
            }
            _cr = op_set(0);
            for (int j = 0; j < step; j++) {
                _sa = op_read(res[31 - j]);
                _sa = op_sel(_pr, _cr, _sa);
                res.set(31 - j, op_write(_sa));
            }
        }
        return res;
    }
protected:
    // mem-based pseudo ops
    bool Copy(bool rs1) {
        _sa = op_read(rs1);
        return op_write(_sa);
    }
    bool Not(bool rs1) {
        _sa = op_read(rs1);
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool And(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _cr = op_move(_sa);
        _sa = op_read(rs2);
        _sa = op_and(_sa, _cr);
        return op_write(_sa);
    }
    bool Or(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _cr = op_move(_sa);
        _sa = op_read(rs2);
        _sa = op_or(_sa, _cr);
        return op_write(_sa);
    }
    bool Nand(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _cr = op_move(_sa);
        _sa = op_read(rs2);
        _sa = op_and(_sa, _cr);
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool Nor(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _cr = op_move(_sa);
        _sa = op_read(rs2);
        _sa = op_or(_sa, _cr);
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool Xor(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _cr = op_move(_sa);
        _sa = op_read(rs2);
        _sa = op_xor(_sa, _cr);
        return op_write(_sa);
    }
    bool Xnor(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _cr = op_move(_sa);
        _sa = op_read(rs2);
        _sa = op_xor(_sa, _cr);
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool Set(bool val) {
        _sa = op_set(val);
        return op_write(_sa);
    }

    virtual B64 uint_div_rem(B32 a, B32 b) {
        B64 qr; // quotient and remainder
        B32 tmp;
        // init 64-bit space
        _sa = op_set(0);
        for (int i = 0; i < 64; i++) {
            qr.set(i, op_write(_sa));
        }
        // compute
        for (int i = 0; i < 32; i++) {
            _sa = op_read(a[31 - i]);
            qr.set(31 - i, op_write(_sa));

            // qr[62-i:31-i] - b
            _cr = op_set(0);
            for (int j = 0; j < 32; j++) {
                _sa = op_read(qr[31 - i + j]);
                _pr = op_xor(_sa, _cr);
                _sa = op_read(b[j]);
                _cr = op_sel(_pr, _cr, _sa);
                _sa = op_xor(_pr, _sa);
                tmp.set(j, op_write(_sa));
            }

            // update quotient
            _cr = op_not(_cr);
            _sa = op_move(_cr);
            qr.set(63 - i, op_write(_sa));

            // update remainder
            _pr = op_move(_cr);
            for (int j = 0; j <= i; j++) {
                _sa = op_read(tmp[j]);
                _cr = op_move(_sa);
                _sa = op_read(qr[31 - i + j]);
                _sa = op_sel(_pr, _cr, _sa);
                qr.set(31 - i + j, op_write(_sa));
            }
        }
        return qr;
    }

    // uint add: n-bit + n-bit = n+1-bit
    virtual B64 karatsuba_add(B64 a, B64 b, int nBit) {
        B64 res;
        _cr = op_set(0);
        for (int i = 0; i < nBit; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            res.set(i, op_write(_sa));
        }
        if (nBit < 64) {
            _sa = op_move(_cr);
            res.set(nBit, op_write(_sa));
        }
        return res;
    }
    // uint sub: n-bit - n-bit = n-bit
    virtual B64 karatsuba_sub(B64 a, B64 b, int nBit) {
        assert(a.to_ullong() >= b.to_ullong());
        B64 res;
        _cr = op_set(0);
        for (int i = 0; i < nBit; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(_pr, _cr, _sa);
            _sa = op_xor(_pr, _sa);
            res.set(i, op_write(_sa));
        }
        assert(_cr == 0);
        return res;
    }
    virtual B64 uint_mul_helper(B64 a, B64 b, int nBit) {
        B64 res;
        // initialize to 0
        _sa = op_set(0);
        for (int i = 0; i < nBit * 2; i++) {
            res.set(i, op_write(_sa));
        }
        // cond copy the first
        _sa = op_read(a[0]);
        _pr = op_move(_sa);
        _cr = op_set(0);
        for (int i = 0; i < nBit; i++) {
            _sa = op_read(b[i]);
            _sa = op_sel(_pr, _sa, _cr);
            res.set(i, op_write(_sa));
        }
        // add and cond write
        for (int i = 1; i < nBit; i++) {
            B32 tmp;
            _cr = op_set(0);
            for (int j = 0; j < nBit; j++) {
                _sa = op_read(res[i + j]);
                _pr = op_xor(_sa, _cr);
                _sa = op_read(b[j]);
                _cr = op_sel(_pr, _sa, _cr);
                _sa = op_xor(_pr, _sa);
                tmp.set(j, op_write(_sa));
            }
            _sa = op_read(a[i]);
            _pr = op_move(_sa);
            // store cr
            //_sa = op_read(res[i + nBit]);
            //_cr = op_sel(!_pr, _sa, _cr);
            _sa = op_and(_cr, _pr);
            res.set(i + nBit, op_write(_sa));
            // store sum
            for (int j = 0; j < nBit; j++) {
                _sa = op_read(res[i + j]);
                _cr = op_move(_sa);
                _sa = op_read(tmp[j]);
                _sa = op_sel(_pr, _sa, _cr);
                res.set(i + j, op_write(_sa));
            }
        }
        return res;
    }
    // uint mul: n-bit * n-bit = 2n-bit
    virtual B64 karatsuba_mul(B64 a, B64 b, int nBit) {
        int debug = 0;
        std::string indent;
        for (int i = 0; i < 32 - nBit; i++) {
            indent += " ";
        }
        // base case
        assert(nBit > 2);
        if (nBit <= 20) { // threshold switch to base mul
            return uint_mul_helper(a, b, nBit);
        }
        // recursion
        int m = (nBit + 1) / 2;
        B64 t0, t1, t2, t3;
        B64 ah, al, bh, bl;
        for (int i = 0; i < m; i++) { // no-copy redirection
            ah.set(i, a[m + i]);
            al.set(i, a[i]);
            bh.set(i, b[m + i]);
            bl.set(i, b[i]);
        }
        if (debug) {
            std::cout << indent << "ah = " << ah.to_ullong()
                      << ", al = " << al.to_ullong()
                      << ", bh = " << bh.to_ullong()
                      << ", bl = " << bl.to_ullong() << std::endl;
            std::cout << indent << "nBit = " << nBit << " m = " << m << std::endl;
        }
        t3 = karatsuba_mul(karatsuba_add(ah, al, m), karatsuba_add(bh, bl, m), m + 1); // 2m+2 bit
        t0 = karatsuba_mul(al, bl, m); // 2m bit
        t2 = karatsuba_mul(ah, bh, m); // 2m bit
        t1 = karatsuba_sub(t3, karatsuba_add(t0, t2, m*2), m*2 + 2); // n+2 bit
        if (debug) {
            std::cout << indent << "t0 = " << t0.to_ullong()
                      << ", t1 = " << t1.to_ullong()
                      << ", t2 = " << t2.to_ullong()
                      << ", t3 = " << t3.to_ullong()
                      << std::endl;
        }
        _sa = op_set(0);

        B64 res;
        for (int i = 0; i < m * 2; i++) { // redir
            res.set(i, t0[i]);
            res.set(m * 2 + i, t2[i]);
        }
        B64 t4, t5;
        for (int i = 0; i < m * 3; i++) { // no-copy redirection
            t4.set(i, res[m + i]);
        }
        t5 = karatsuba_add(t4, t1, m * 3);
        for (int i = 0; i < m * 3; i++) { // no-copy redirection
            res.set(m + i, t5[i]);
        }
        if (debug) {
            std::cout << indent << a.to_ullong() << " * " << b.to_ullong() << " = " << res.to_ullong() << std::endl;
            //std::cout << a.str() << " * " << b.str() << " = " << res.str() << std::endl;
        }
        return res;
    }

    // reg-based bit-serial ops
    bool op_not(bool val) { _stat["not"]++; return !val; }
    bool op_and(bool rs1, bool rs2) { _stat["and"]++; return rs1 & rs2; }
    bool op_or(bool rs1, bool rs2) { _stat["or"]++; return rs1 | rs2; }
    bool op_xor(bool rs1, bool rs2) { _stat["xor"]++; return rs1 ^ rs2; }
    bool op_sel(bool cond, bool rs1, bool rs2) { _stat["sel"]++; return cond ? rs1 : rs2; }

    bool op_move(bool val) { _stat["move"]++; return val; }
    bool op_set(bool val) { _stat["set"]++; return val; }
    bool op_read(bool val) { _stat["R"]++; return val; }
    bool op_write(bool val) { _stat["W"]++; assert(val == _sa); return _sa; }

    bool _sa;
    bool _cr;
    bool _pr;
};

#endif
