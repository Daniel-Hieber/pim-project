// File: Logic Unit Class - Pseudo Bitwise Implementation
// Author: Deyuan Guo
// Date: July 2023

#ifndef LU_BITWISE_H
#define LU_BITWISE_H

#include <map>
#include <string>
#include <bit>
#include "lu.h"

class lu_bitwise : public lu
{
public:
    lu_bitwise() {}
    virtual ~lu_bitwise() {}
    virtual std::string getName() { return "bitwise"; }

protected:
    virtual B32 logic_not(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(Not(Read(a[i]))));
        }
        return res;
    }
    virtual B32 logic_and(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(And(Read(a[i]), Read(b[i]))));
        }
        return res;
    }
    virtual B32 logic_or(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(Or(Read(a[i]), Read(b[i]))));
        }
        return res;
    }
    virtual B32 logic_xor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(Xor(Read(a[i]), Read(b[i]))));
        }
        return res;
    }
    virtual B32 logic_nand(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(Nand(Read(a[i]), Read(b[i]))));
        }
        return res;
    }
    virtual B32 logic_nor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(Nor(Read(a[i]), Read(b[i]))));
        }
        return res;
    }
    virtual B32 logic_xnor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(Xnor(Read(a[i]), Read(b[i]))));
        }
        return res;
    }

    virtual B32 int_add(B32 a, B32 b) {
        B32 res;
        bool carry = 0;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(ADD(Read(a[i]), Read(b[i]), carry)));
        }
        return res;
    }
    virtual B32 int_sub(B32 a, B32 b) {
        B32 res;
        bool borrow = 0;
        for (int i = 0; i < 32; i++) {
            res.set(i, Write(SUB(Read(a[i]), Read(b[i]), borrow)));
        }
        return res;
    }
    virtual B32 int_abs(B32 a) {
        B32 res;
        bool sign = Read(a[31]);
        bool carry = sign;
        for (int i = 0; i < 32; i++) {
            bool t = Xor(Read(a[i]), sign);
          res.set(i, Write(ADD(t, 0, carry)));
        }
        return res;
    }
    virtual B32 int_min(B32 a, B32 b) { return a.to_int() < b.to_int() ? a.to_int() : b.to_int(); }
    virtual B32 int_max(B32 a, B32 b) { return a.to_int() > b.to_int() ? a.to_int() : b.to_int(); }
    virtual B64 uint_mul(B32 a, B32 b) { return (unsigned long long)a.to_uint() * b.to_uint(); }
    virtual B32 uint_div(B32 a, B32 b) { return a.to_uint() / b.to_uint(); }
    virtual B32 uint_rem(B32 a, B32 b) { return a.to_uint() % b.to_uint(); }

    virtual B1 uint_gt(B32 a, B32 b) { return a.to_uint() > b.to_uint(); }
    virtual B1 uint_lt(B32 a, B32 b) { return a.to_uint() < b.to_uint(); }
    virtual B1 int_gt(B32 a, B32 b) { return a.to_int() > b.to_int(); }
    virtual B1 int_lt(B32 a, B32 b) { return a.to_int() < b.to_int(); }
    virtual B1 int_eq(B32 a, B32 b) { return a.to_int() == b.to_int(); }

    virtual B32 float_add(B32 a, B32 b) { return a.to_float() + b.to_float(); }
    virtual B32 float_sub(B32 a, B32 b) { return a.to_float() - b.to_float(); }
    virtual B32 float_mul(B32 a, B32 b) { return a.to_float() * b.to_float(); }
    virtual B32 float_div(B32 a, B32 b) { return a.to_float() / b.to_float(); }

    virtual B32 copy(B32 a) {
        _stat["COPY"]++;
        return a;
    }
    virtual B1 search(B32 a, B32 p) {
        B1 res;
        for (int i = 0; i < 32; i++) {
            res.set(0, Or(res[0], Xor(a[i], p[i])));
        }
        return Not(res[0]);
    }
    virtual B8 pop_count(B32 a) { return static_cast<unsigned char>(std::bitset<32>(a.to_uint()).count()); }
    virtual B32 if_else(B1 c, B32 a, B32 b) {
        _stat["IF-ELSE"]++;
        B32 res = c[0] ? a : b;
        return res;
    }
    virtual B32 int_ReLU(B32 a) { return a.to_int() > 0 ? a : 0; }
    virtual B32 uint_var_lshift(B32 a, B32 n) { return (a.to_uint() << n.to_uint()); }
    virtual B32 uint_var_rshift(B32 a, B32 n) { return (a.to_uint() >> n.to_uint()); }

protected:
    virtual bool Read(bool val) { _stat["R"]++; return val; }
    virtual bool Write(bool val) { _stat["W"]++; return val; }
    virtual bool Copy(bool val) { return Write(Read(val)); }

    virtual bool Not(bool src) { _stat["not"]++; return !src; }
    virtual bool And(bool src1, bool src2) { _stat["and"]++; return src1 & src2; }
    virtual bool Or(bool src1, bool src2) { _stat["or"]++; return src1 | src2; }
    virtual bool Xor(bool src1, bool src2) { _stat["xor"]++; return src1 ^ src2; }
    virtual bool Nand(bool src1, bool src2) { _stat["nand"]++; return !(src1 & src2); }
    virtual bool Nor(bool src1, bool src2) { _stat["nor"]++; return !(src1 | src2); }
    virtual bool Xnor(bool src1, bool src2) { _stat["xnor"]++; return !(src1 ^ src2); }

    virtual bool ADD(bool src1, bool src2, bool& carry) {
        _stat["ADD"]++;
        bool res = src1 ^ src2 ^ carry;
        carry = (src1 & src2) | (src1 & carry) | (src2 & carry);
        return res;
    }
    virtual bool SUB(bool src1, bool src2, bool& borrow) {
        _stat["SUB"]++;
        bool res = src1 ^ src2 ^ borrow;
        borrow = ((!src1) & src2) | ((!src1) & borrow) | (src2 & borrow);
        return res;
    }
};

#endif
