// File: Logic Unit Class - XNOR+AND+SEL - Associative Processing
// Author: Deyuan Guo
// Date: Aug 2023

#ifndef LU_XNOR_H
#define LU_XNOR_H

#include <map>
#include <string>
#include "lu.h"

class lu_xnor : public lu_base
{
public:
    lu_xnor() : _sa(0), _cr(0), _pr(0) {}
    virtual ~lu_xnor() {}
    virtual std::string getName() { return "xnor"; }

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
        B1 br;
        br.set(0, Set(0));
        for (int i = 0; i < 32; i++) {
            res.set(i, FullSub(a[i], b[i], br));
        }
        return res;
    }
    virtual B32 int_abs(B32 a) {
        B32 res;
        B1 cr;
        cr.set(0, Copy(a[31]));
        for (int i = 0; i < 32; i++) {
            op_read(cr[0]);
            _cr = op_move(_sa);
            op_read(a[i]);
            _pr = op_set(0);
            _pr = op_xnor(_pr, _sa);
            _sa = op_and(_cr, _pr);
            cr.set(0, _sa);
            _sa = op_xnor(_cr, _pr);
            _cr = op_set(0);
            _cr = op_xnor(_sa, _cr);
            // cond copy
            op_read(a[31]);
            _pr = op_move(_sa);
            op_read(a[i]);
            _sa = op_sel(_cr, _sa);
            res.set(i, op_write());
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
        _cr = op_set(0);
        for (int i = 0; i < 31; i++) {
            op_read(a[i]);
            _pr = op_xnor(_sa, _cr);
            op_read(b[i]);
            _cr = op_sel(_sa, _cr);
        }
        // 31-bit uint lt cmp result in _cr
        op_read(b[31]);
        _pr = op_move(_sa);
        op_read(a[31]);
        _pr = op_xnor(_sa, _pr);
        _sa = op_sel(_cr, _sa);
        res.set(0, op_write());
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
        op_read(c[0]);
        _pr = op_move(_sa);
        for (int i = 0; i < 32; i++) {
            op_read(a[i]);
            _cr = op_move(_sa);
            op_read(b[i]);
            _sa = op_sel(_cr, _sa);
            res.set(i, op_write());
        }
        return res;
    }

    virtual B8 pop_count(B32 a) {
        B32 tmp;
        // 2 bits -> 2-bit count
        for (int i = 0; i < 32; i += 2) {
            op_read(a[i]);
            _cr = op_move(_sa);
            op_read(a[i + 1]);
            _pr = op_move(_sa);
            _sa = op_and(_cr, _pr);
            tmp.set(i + 1, op_write());
            _sa = op_xnor(_cr, _pr);
            _cr = op_set(0);
            _sa = op_xnor(_sa, _cr);
            tmp.set(i, op_write());
        }
        // aggregate from 2-bit count to 6-bit count
        for (int iter = 2; iter <= 5; iter++) {
            for (int i = 0; i < 32; i += (1 << iter)) {
                B1 cr;
                cr.set(0, Set(0));
                for (int j = 0; j < iter; j++) {
                    tmp.set(i + j, FullAdd(tmp[i + j], tmp[i + (1 << (iter - 1)) + j], cr));
                }
                op_read(cr[0]);
                tmp.set(i + iter, op_write());
            }
        }
        _sa = op_set(0);
        tmp.set(6, op_write());
        tmp.set(7, op_write());

        // return first 8 bits
        B8 res(static_cast<unsigned char>(tmp.to_uint()));
        return res;
    }

private:
    // mem-based pseudo ops
    bool Copy(bool rs1) {
        op_read(rs1);
        return op_write();
    }
    bool Nand(bool rs1, bool rs2) {
        op_read(rs1);
        _cr = op_move(_sa);
        op_read(rs2);
        _sa = op_and(_sa, _cr);
        _cr = op_set(0);
        _sa = op_xnor(_sa, _cr);
        return op_write();
    }
    bool Not(bool rs1) {
        op_read(rs1);
        _cr = op_set(0);
        _sa = op_xnor(_sa, _cr);
        return op_write();
    }
    bool And(bool rs1, bool rs2) {
        op_read(rs1);
        _cr = op_move(_sa);
        op_read(rs2);
        _sa = op_and(_sa, _cr);
        return op_write();
    }
    bool Or(bool rs1, bool rs2) {
        op_read(rs1);
        _pr = op_move(_sa);
        _cr = op_set(1);
        op_read(rs2);
        _sa = op_sel(_cr, _sa);
        return op_write();
    }
    bool Nor(bool rs1, bool rs2) {
        op_read(rs1);
        _pr = op_move(_sa);
        _cr = op_set(1);
        op_read(rs2);
        _sa = op_sel(_cr, _sa);
        _cr = op_set(0);
        _sa = op_xnor(_sa, _cr);
        return op_write();
    }
    bool Xor(bool rs1, bool rs2) {
        op_read(rs1);
        _cr = op_move(_sa);
        op_read(rs2);
        _sa = op_xnor(_sa, _cr);
        _cr = op_set(0);
        _sa = op_xnor(_sa, _cr);
        return op_write();
    }
    bool Xnor(bool rs1, bool rs2) {
        op_read(rs1);
        _cr = op_move(_sa);
        op_read(rs2);
        _sa = op_xnor(_sa, _cr);
        return op_write();
    }
    bool Set(bool rs1) {
        _sa = op_set(rs1);
        return op_write();
    }
    bool FullAdd(bool a, bool b, B1& carry) {
        // compute _pr = (a == carry) as condition
        op_read(carry[0]);
        _cr = op_move(_sa);
        op_read(a);
        _pr = op_xnor(_sa, _cr);
        // compute borrow' = _pr ? carry : b
        op_read(b);
        _sa = op_sel(_cr, _sa);
        carry.set(0, op_write());
        // compute sum = _pr == b
        op_read(b);
        _sa = op_xnor(_pr, _sa);
        return op_write();
    }
    bool FullSub(bool a, bool b, B1& borrow) {
        // compute _pr = (a == borrow) as condition
        op_read(borrow[0]);
        _cr = op_move(_sa);
        op_read(a);
        _pr = op_xnor(_sa, _cr);
        // compute borrow' = _pr ? borrow : b
        op_read(b);
        _sa = op_sel(_sa, _cr);
        borrow.set(0, op_write());
        // compute sum = _pr == b
        op_read(b);
        _sa = op_xnor(_pr, _sa);
        return op_write();
    }
    bool HalfAdd(bool a, B1& carry) {
        op_read(carry[0]);
        _cr = op_move(_sa);
        op_read(a);
        _pr = op_move(_sa);
        _sa = op_and(_cr, _pr);
        carry.set(0, _sa);
        _sa = op_xnor(_cr, _pr);
        _cr = op_set(0);
        _sa = op_xnor(_sa, _cr);
        return op_write();
    }

    virtual B64 uint_mul_helper(B64 a, B64 b, int nBit) {
        B64 res;
        // initialize to 0
        _sa = op_set(0);
        for (int i = 0; i < nBit * 2; i++) {
            res.set(i, op_write());
        }
        // cond copy the first
        op_read(a[0]);
        _pr = op_move(_sa);
        _cr = op_set(0);
        for (int i = 0; i < nBit; i++) {
            op_read(b[i]);
            _sa = op_sel(_sa, _cr);
            res.set(i, op_write());
        }
        // add and cond write
        for (int i = 1; i < nBit; i++) {
            B32 tmp;
            B1 cr;
            cr.set(0, Set(0));
            for (int j = 0; j < nBit; j++) {
                tmp.set(j, FullAdd(res[i + j], b[j], cr));
            }
            op_read(a[i]);
            _pr = op_move(_sa); // cond
            // store cr
            op_read(cr[0]);
            _sa = op_and(_pr, _sa);
            res.set(i + nBit, op_write());
            // store sum
            for (int j = 0; j < nBit; j++) {
                op_read(res[i + j]);
                _cr = op_move(_sa);
                op_read(tmp[j]);
                _sa = op_sel(_sa, _cr);
                res.set(i + j, op_write());
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
            op_read(cr[0]);
            res.set(nBit, op_write());
        }
        return res;
    }
    // uint sub: n-bit - n-bit = n-bit
    virtual B64 karatsuba_sub(B64 a, B64 b, int nBit) {
        assert(a.to_ullong() >= b.to_ullong());
        B64 res;
        B1 cr;
        cr.set(0, Set(0));
        for (int i = 0; i < nBit; i++) {
            res.set(i, FullSub(a[i], b[i], cr));
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


    // reg-based bit-serial ops
    bool op_xnor(bool rs1, bool rs2) { _stat["xnor"]++; return rs1 == rs2; }
    bool op_and(bool rs1, bool rs2) { _stat["and"]++; return rs1 && rs2; }
    bool op_sel(bool rs1, bool rs2) { _stat["sel"]++; return _pr ? rs1 : rs2; }

    bool op_move(bool rs) { _stat["move"]++; return rs; }
    bool op_set(bool rs) { _stat["set"]++; return rs; }
    void op_read(bool val) { _stat["R"]++; _sa = val; }
    bool op_write() { _stat["W"]++; return _sa; }

    bool _sa;
    bool _cr;
    bool _pr;
};

#endif
