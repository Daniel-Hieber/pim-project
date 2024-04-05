// File: Logic Unit Class - MAJ3 + NOT
// Author: Deyuan Guo
// Date: July 2023

#ifndef LU_MAJ3_H
#define LU_MAJ3_H

#include <map>
#include <string>
#include "lu.h"

class lu_maj3 : public lu_base
{
public:
    lu_maj3() : _sa(0), _t1(0), _t2(0) {}
    virtual ~lu_maj3() {}
    virtual std::string getName() { return "maj3"; }

protected:
    virtual B32 logic_not(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Not(a[i]));
        }
        return res;
    }
    virtual B32 logic_and(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, And(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_or(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Or(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_xor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Xor(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_nand(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Nand(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_nor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Nor(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_xnor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Xnor(a[i], b[i]));
        }
        return res;
    }

    virtual B32 int_add(B32 a, B32 b) {
        B32 res;
        B1 cr;
        cr.set(0, Set(0));
        for (int i = 0; i < 32; i++) {
            res.set(i, FullAdd(a[i], b[i], cr));
        }
        return res;
    }
    virtual B32 int_sub(B32 a, B32 b) {
        B32 res;
        B1 cr;
        cr.set(0, Set(1));
        for (int i = 0; i < 32; i++) {
            res.set(i, FullSub(a[i], b[i], cr));
        }
        return res;
    }
    virtual B32 int_abs(B32 a) {
        B32 res;
        B1 sign;
        sign.set(0, Copy(a[31]));
        for (int i = 0; i < 32; i++) {
            res.set(i, Xor(a[i], sign[0]));
        }
        B1 cr;
        cr.set(0, Copy(sign[0]));
        for (int i = 0; i < 32; i++) {
            res.set(i, HalfAdd(res[i], cr));
        }
        return res;
    }
    virtual B32 int_min(B32 a, B32 b) {
        B1 cmp = int_lt(a, b);
        B32 res;
        res = if_else(cmp, a, b);
        return res;
    }
    virtual B32 int_max(B32 a, B32 b) { return a.to_int() > b.to_int() ? a.to_int() : b.to_int(); }
    virtual B64 uint_mul(B32 a, B32 b) {
        return karatsuba_mul(a, b, 32);
        //return uint_mul_helper(a, b, 32);
    }
    virtual B32 uint_div(B32 a, B32 b) { return a.to_uint() / b.to_uint(); }
    virtual B32 uint_rem(B32 a, B32 b) { return a.to_uint() % b.to_uint(); }

    virtual B1 uint_gt(B32 a, B32 b) { return a.to_uint() > b.to_uint(); }
    virtual B1 uint_lt(B32 a, B32 b) { return a.to_uint() < b.to_uint(); }
    virtual B1 int_gt(B32 a, B32 b) { return a.to_int() > b.to_int(); }
    virtual B1 int_lt(B32 a, B32 b) {
        B1 res;
        B1 cr;
        int_sub_helper(a, b, 31, cr);
        B1 cond = Xor(a[31], b[31]);
        cr.set(0, Not(cr[0]));
        res = Sel(cond[0], a[31], cr[0]);
        return res;
    }
    virtual B1 int_eq(B32 a, B32 b) { return a.to_int() == b.to_int(); }
    virtual B1 search(B32 a, B32 p) { return a.to_int() == p.to_int(); }

    virtual B32 copy(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Copy(a[i]));
        }
        return a;
    }
    virtual B32 if_else(B1 c, B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Sel(c[0], a[i], b[i]));
        }
        return res;
    }
    virtual B8 pop_count(B32 a) {
        B32 tmp;
        // 2 bits -> 2-bit count
        for (int i = 0; i < 32; i += 2) {
            tmp.set(i, Xor(a[i], a[i + 1]));
            tmp.set(i + 1, And(a[i], a[i + 1]));
        }
        // aggregate from 2-bit count to 6-bit count
        for (int iter = 2; iter <= 5; iter++) {
            for (int i = 0; i < 32; i += (1 << iter)) {
                B1 cr;
                cr.set(0, Set(0));
                for (int j = 0; j < iter; j++) {
                    tmp.set(i + j, FullAdd(tmp[i + j], tmp[i + (1 << (iter - 1)) + j], cr));
                }
                _sa = op_read(cr[0]);
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

private:
    // pseudo
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
        _t1 = op_move(_sa);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2);
        return op_write(_sa);
    }
    bool Or(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _t1 = op_move(_sa);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _sa = op_set(1);
        _sa = op_maj(_sa, _t1, _t2);
        return op_write(_sa);
    }
    bool Nand(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _t1 = op_move(_sa);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2);
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool Nor(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _t1 = op_move(_sa);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _sa = op_set(1);
        _sa = op_maj(_sa, _t1, _t2);
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool Xor(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _t1 = op_move(_sa);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _t1 = op_not(_t1);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2); // x1 = maj(~a, b, 0)
        bool tmp = op_write(_sa);
        _t1 = op_not(_t1);
        _t2 = op_not(_t2);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2); // x2 = maj(a, ~b, 0)
        _t1 = op_move(_sa);
        _t2 = op_set(1);
        _sa = op_read(tmp);
        _sa = op_maj(_sa, _t1, _t2); // maj(x1, x2, 1)
        return op_write(_sa);
    }
    bool Xnor(bool rs1, bool rs2) {
        _sa = op_read(rs1);
        _t1 = op_move(_sa);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _t1 = op_not(_t1);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2); // x1 = maj(~a, b, 0)
        bool tmp = op_write(_sa);
        _t1 = op_not(_t1);
        _t2 = op_not(_t2);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2); // x2 = maj(a, ~b, 0)
        _t1 = op_move(_sa);
        _t2 = op_set(1);
        _sa = op_read(tmp);
        _sa = op_maj(_sa, _t1, _t2); // maj(x1, x2, 1)
        _sa = op_not(_sa);
        return op_write(_sa);
    }
    bool Set(bool rs1) {
        _sa = op_set(rs1);
        return op_write(_sa);
    }
    bool FullAdd(bool a, bool b, B1& carry) {
        _sa = op_read(carry[0]);
        _t1 = op_move(_sa);
        _sa = op_read(b);
        _t2 = op_move(_sa);
        _sa = op_read(a);
        _t1 = op_not(_t1);
        _sa = op_maj(_sa, _t1, _t2); // x1 = maj(a, b, ~cr)
        bool tmp1 = op_write(_sa);

        _sa = op_read(a);
        _t1 = op_not(_t1);
        _sa = op_maj(_sa, _t1, _t2); // x2 = maj(a, b, cr)
        carry.set(0, op_write(_sa));
        _t2 = op_move(_sa);
        _t2 = op_not(_t2);
        _sa = op_read(tmp1);
        _sa = op_maj(_sa, _t1, _t2); // x3 = maj(a, b, cr)
        return op_write(_sa);
    }
    bool FullSub(bool a, bool b, B1& carry) {
        _sa = op_read(carry[0]);
        _t1 = op_move(_sa);
        _sa = op_read(b);
        _sa = op_not(_sa); // invert b
        _t2 = op_move(_sa);
        _sa = op_read(a);
        _t1 = op_not(_t1);
        _sa = op_maj(_sa, _t1, _t2); // x1 = maj(a, b, ~cr)
        bool tmp1 = op_write(_sa);

        _sa = op_read(a);
        _t1 = op_not(_t1);
        _sa = op_maj(_sa, _t1, _t2); // x2 = maj(a, b, cr)
        carry.set(0, op_write(_sa));
        _t2 = op_move(_sa);
        _t2 = op_not(_t2);
        _sa = op_read(tmp1);
        _sa = op_maj(_sa, _t1, _t2); // x3 = maj(a, b, cr)
        return op_write(_sa);
    }
    bool HalfAdd(bool a, B1& carry) {
        _sa = op_read(carry[0]);
        _t1 = op_move(_sa);
        _sa = op_read(a);
        _t2 = op_move(_sa);
        _sa = op_set(0);
        _t1 = op_not(_t1);
        _sa = op_maj(_sa, _t1, _t2); // x1 = maj(a, 0, ~cr)
        bool tmp1 = op_write(_sa);

        _sa = op_set(0);
        _t1 = op_not(_t1);
        _sa = op_maj(_sa, _t1, _t2); // x2 = maj(a, 0, cr)
        carry.set(0, op_write(_sa));
        _t2 = op_move(_sa);
        _t2 = op_not(_t2);
        _sa = op_read(tmp1);
        _sa = op_maj(_sa, _t1, _t2); // x2 = maj(a, b, cr)
        return op_write(_sa);
    }
    virtual bool Sel(bool cond, bool rs1, bool rs2) {
        _sa = op_read(cond);
        _t1 = op_move(_sa);
        _sa = op_read(rs1);
        _t2 = op_move(_sa);
        _sa = op_set(0);
        _sa = op_maj(_sa, _t1, _t2); // cond & rs1
        bool tmp1 = op_write(_sa);
        _t1 = op_not(_t1);
        _sa = op_read(rs2);
        _t2 = op_move(_sa);
        _sa = op_set(0);
        _t2 = op_maj(_sa, _t1, _t2); // !cond & rs2
        _sa = op_read(tmp1);
        _t1 = op_set(1);
        _sa = op_maj(_sa, _t1, _t2); // cond & rs1 | !cond & rs2
        return op_write(_sa);
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
        B1 zero;
        zero.set(0, Set(0));
        for (int i = 0; i < nBit; i++) {
            res.set(i, Sel(a[0], b[i], zero[0]));
        }
        // add and cond write
        for (int i = 1; i < nBit; i++) {
            B32 tmp;
            B1 cr;
            cr.set(0, Set(0));
            for (int j = 0; j < nBit; j++) {
                tmp.set(j, FullAdd(res[i + j], b[j], cr));
            }
            res.set(i + nBit, And(a[i], cr[0]));
            // store sum
            for (int j = 0; j < nBit; j++) {
                res.set(i + j, Sel(a[i], tmp[j], res[i + j]));
            }
        }
        return res;
    }
    // uint add: n-bit + n-bit = n+1-bit
    virtual B64 karatsuba_add(B64 a, B64 b, int nBit) {
        B64 res;
        B1 cr;
        cr.set(0, Set(0));
        for (int i = 0; i < nBit; i++) {
            res.set(i, FullAdd(a[i], b[i], cr));
        }
        if (nBit < 64) {
            _sa = op_read(cr[0]);
            res.set(nBit, op_write(_sa));
        }
        return res;
    }
    virtual B64 int_sub_helper(B64 a, B64 b, int nBit, B1& cr) {
        B64 res;
        cr.set(0, Set(1));
        for (int i = 0; i < nBit; i++) {
            res.set(i, FullSub(a[i], b[i], cr));
        }
        return res;
    }
    // uint sub: n-bit - n-bit = n-bit
    virtual B64 karatsuba_sub(B64 a, B64 b, int nBit) {
        assert(a.to_ullong() >= b.to_ullong());
        B1 cr;
        return int_sub_helper(a, b, nBit, cr);
    }
    // uint mul: n-bit * n-bit = 2n-bit
    virtual B64 karatsuba_mul(B64 a, B64 b, int nBit) {
        int debug = 0;
        std::string indent;
        for (int i = 0; i < 32 - nBit; i++) {
            indent += " ";
        }
        // base cases
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



    // ops
    bool op_set(bool val) { _stat["set"]++; return val; }
    bool op_move(bool val) { _stat["move"]++; return val; }
    bool op_maj(bool val1, bool val2, bool val3) { _stat["maj3"]++; return (val1 && val2) || (val1 && val3) || (val2 && val3); }
    bool op_not(bool val) { _stat["not"]++; return !val; }
    bool op_read(bool val) { _stat["R"]++; return val; }
    bool op_write(bool val) { _stat["W"]++; assert(val == _sa); return _sa; }

    bool _sa;
    bool _t1;
    bool _t2;
};

#endif
