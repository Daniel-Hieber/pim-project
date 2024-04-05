// File: Logic Unit Class - Compute-SRAM
// Author: Deyuan Guo
// Date: June 2023

#ifndef LU_CRAM_H
#define LU_CRAM_H

#include <map>
#include <string>
#include "lu.h"

class lu_cram : public lu
{
public:
    lu_cram() : _carry(0), _tag(0) {}
    virtual ~lu_cram() {}
    virtual std::string getName() { return "cram"; }

protected:
    virtual B32 logic_not(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_inv(a[i]));
        }
        return res;
    }
    virtual B32 logic_and(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_and(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_or(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_or(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_xor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_xor(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_nand(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_nand(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_nor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_nor(a[i], b[i]));
        }
        return res;
    }
    virtual B32 logic_xnor(B32 a, B32 b) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_xnor(a[i], b[i]));
        }
        return res;
    }
    virtual B8 pop_count(B32 a) {
        return static_cast<unsigned char>(std::bitset<32>(a.to_uint()).count());
    }

    virtual B32 int_add(B32 a, B32 b) {
        op_reset_c();
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_add(a[i], b[i]));
        }
        return res;
    }
    virtual B32 int_sub(B32 a, B32 b) {
        b = logic_not(b);
        op_set_c();
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_add(a[i], b[i]));
        }
        return res;
    }
    virtual B32 int_abs(B32 a) {
        B32 res;
        for (int i = 0; i < 32; i++) {
            res.set(i, op_xor(a[i], a[31]));
        }
        op_reset_c();
        op_add(a[31], a[31]);
        for (int i = 0; i < 32; i++) {
            res.set(i, op_add(res[i], res[31]));
        }
        return res;
    }
    virtual B32 int_min(B32 a, B32 b) { return a.to_int() < b.to_int() ? a.to_int() : b.to_int(); }
    virtual B32 int_max(B32 a, B32 b) { return a.to_int() > b.to_int() ? a.to_int() : b.to_int(); }
    virtual B64 uint_mul(B32 a, B32 b) {
        B64 res;
        op_reset_c();
        for (int i = 0; i < 64; i++) {
            res.set(i, op_store_c());
        }
        op_load_t(a[0]);
        for (int j = 0; j < 32; j++) {
            bool b1 = op_copy(b[j]);
            if (_tag) {
                res.set(j, b1);
            }
        }
        for (int i = 1; i < 32; i++) {
            op_load_t(a[i]);
            for (int j = 0; j < 32; j++) {
                bool b1 = op_add(res[i + j], b[j]);
                if (_tag) {
                    res.set(i + j, b1);
                }
            }
            bool cr = op_store_c();
            if (_tag) {
                res.set(i + 32, cr);
            }
            if (i < 31) {
                op_reset_c();
            }
        }
        return res;
    }
    virtual B32 uint_div(B32 a, B32 b) { return a.to_uint() / b.to_uint(); }
    virtual B32 uint_rem(B32 a, B32 b) { return a.to_uint() % b.to_uint(); }

    virtual B1 uint_gt(B32 a, B32 b) {
        a = logic_not(a);
        op_set_c();
        B1 res;
        for (int i = 0; i < 32; i++) {
            op_add(a[i], b[i]);
        }
        res.set(0, op_store_c());
        res.set(0, op_inv(res[0]));
        return res;
    }
    virtual B1 uint_lt(B32 a, B32 b) {
        b = logic_not(b);
        op_set_c();
        B1 res;
        for (int i = 0; i < 32; i++) {
            op_add(a[i], b[i]);
        }
        res.set(0, op_store_c());
        res.set(0, op_inv(res[0]));
        return res;
    }
    virtual B1 int_gt(B32 a, B32 b) {
        // tag = sign(a) == sign(b)
        B1 cond;
        cond.set(0, op_xor(a[31], b[31]));
        op_load_t(cond[0]);

        // 31-bit uint gt cmp
        a = logic_not(a);
        op_set_c();
        B1 res;
        for (int i = 0; i < 31; i++) {
            op_add(a[i], b[i]);
        }
        res.set(0, op_store_c());
        res.set(0, op_inv(res[0]));

        // cond ops
        bool b1 = op_copy(b[31]);
        if (_tag) {
            res.set(0, b1);
        }
        return res;
    }
    virtual B1 int_lt(B32 a, B32 b) {
        // tag = sign(a) == sign(b)
        B1 cond;
        cond.set(0, op_xor(a[31], b[31]));
        op_load_t(cond[0]);

        // 31-bit uint lt cmp
        b = logic_not(b);
        op_set_c();
        B1 res;
        for (int i = 0; i < 31; i++) {
            op_add(a[i], b[i]);
        }
        res.set(0, op_store_c());
        res.set(0, op_inv(res[0]));

        // cond ops
        bool b1 = op_copy(a[31]);
        if (_tag) {
            res.set(0, b1);
        }
        return res;
    }
    virtual B1 int_eq(B32 a, B32 b) {
        B1 tmp;
        B1 res;
        res.set(0, op_xnor(a[0], b[0]));
        for (int i = 1; i < 32; i++) {
            tmp.set(0, op_xnor(a[i], b[i]));
            res.set(0, op_and(res[0], tmp[0]));
        }
        return res;
    }
    virtual B1 search(B32 a, B32 p) {
        for (int i = 0; i < 32; i++) {
            op_equal(a[i], p[i], i > 0);
        }
        return _tag;
    }

    virtual B32 float_add(B32 a, B32 b) { return a.to_float() + b.to_float(); }
    virtual B32 float_sub(B32 a, B32 b) { return a.to_float() - b.to_float(); }
    virtual B32 float_mul(B32 a, B32 b) { return a.to_float() * b.to_float(); }
    virtual B32 float_div(B32 a, B32 b) { return a.to_float() / b.to_float(); }

private:
    bool op_and(bool b1, bool b2) { _stat["and"]++; return b1 & b2; }
    bool op_or(bool b1, bool b2) { _stat["or"]++; return b1 | b2; }
    bool op_xor(bool b1, bool b2) { _stat["xor"]++; return b1 ^ b2; }
    bool op_nand(bool b1, bool b2) { _stat["nand"]++; return !(b1 & b2); }
    bool op_nor(bool b1, bool b2) { _stat["nor"]++; return !(b1 | b2); }
    bool op_xnor(bool b1, bool b2) { _stat["xnor"]++; return !(b1 ^ b2); }
    bool op_add(bool b1, bool b2) {
        _stat["add"]++;
        bool sum = _carry ^ b1 ^ b2;
        _carry = (b1 & b2) | (_carry & (b1 ^ b2));
        return sum;
    }
    bool op_copy(bool b1) { _stat["copy"]++; return b1; }
    bool op_inv(bool b1) { _stat["inv"]++; return !b1; }
    void op_equal(bool b1, bool b2, bool tag_en) { _stat["equal"]++; _tag = tag_en ? (_tag & (b1 == b2)) : (b1 == b2); }
    void op_load_t(bool b1) { _stat["load-t"]++; _tag = b1; }
    bool op_store_c() { _stat["store-c"]++; return _carry; }
    bool op_store_t() { _stat["store-t"]++; return _tag; }
    void op_set_c() { _stat["set-c"]++; _carry = 1; }
    void op_reset_c() { _stat["reset-c"]++; _carry = 0; }
    void op_c_to_t() { _stat["c-to-t"]++; _tag = _carry; }

    bool _carry;
    bool _tag;
};

#endif
