// File: Logic Unit Class - CPU Implementation
// Author: Deyuan Guo
// Date: June 2023

#ifndef LU_BASE_H
#define LU_BASE_H

#include <map>
#include <string>
#include <bit>
#include "lu.h"

class lu_base : public lu
{
public:
    lu_base() {}
    virtual ~lu_base() {}
    virtual std::string getName() { return "base"; }

protected:
    virtual B32 logic_not(B32 a) { _stat["not"]++; return ~a.to_uint(); }
    virtual B32 logic_and(B32 a, B32 b) { _stat["and"]++; return a.to_uint() & b.to_uint(); }
    virtual B32 logic_or(B32 a, B32 b) { _stat["or"]++; return a.to_uint() | b.to_uint(); }
    virtual B32 logic_xor(B32 a, B32 b) { _stat["xor"]++; return a.to_uint() ^ b.to_uint(); }
    virtual B32 logic_nand(B32 a, B32 b) { _stat["nand"]++; return ~(a.to_uint() & b.to_uint()); }
    virtual B32 logic_nor(B32 a, B32 b) { _stat["nor"]++; return ~(a.to_uint() | b.to_uint()); }
    virtual B32 logic_xnor(B32 a, B32 b) { _stat["xnor"]++; return ~(a.to_uint() ^ b.to_uint()); }

    virtual B32 int_add(B32 a, B32 b) { _stat["int-add"]++; return a.to_int() + b.to_int(); }
    virtual B32 int_sub(B32 a, B32 b) { _stat["int-sub"]++; return a.to_int() - b.to_int(); }
    virtual B32 int_abs(B32 a) { _stat["int-abs"]++; return a.to_int() > 0 ? a.to_int() : -a.to_int(); }
    virtual B32 int_min(B32 a, B32 b) { _stat["int-min"]++; return a.to_int() < b.to_int() ? a.to_int() : b.to_int(); }
    virtual B32 int_max(B32 a, B32 b) { _stat["int-max"]++; return a.to_int() > b.to_int() ? a.to_int() : b.to_int(); }
    virtual B64 uint_mul(B32 a, B32 b) { _stat["uint-mul"]++; return (unsigned long long)a.to_uint() * b.to_uint(); }
    virtual B32 uint_div(B32 a, B32 b) { _stat["uint-div"]++; return a.to_uint() / b.to_uint(); }
    virtual B32 uint_rem(B32 a, B32 b) { _stat["uint-rem"]++; return a.to_uint() % b.to_uint(); }

    virtual B1 uint_gt(B32 a, B32 b) { _stat["uint-gt"]++; return a.to_uint() > b.to_uint(); }
    virtual B1 uint_lt(B32 a, B32 b) { _stat["uint-lt"]++; return a.to_uint() < b.to_uint(); }
    virtual B1 int_gt(B32 a, B32 b) { _stat["int-gt"]++; return a.to_int() > b.to_int(); }
    virtual B1 int_lt(B32 a, B32 b) { _stat["int-lt"]++; return a.to_int() < b.to_int(); }
    virtual B1 int_eq(B32 a, B32 b) { _stat["int-eq"]++; return a.to_int() == b.to_int(); }

    virtual B32 float_add(B32 a, B32 b) { _stat["flt-add"]++; return a.to_float() + b.to_float(); }
    virtual B32 float_sub(B32 a, B32 b) { _stat["flt-sub"]++; return a.to_float() - b.to_float(); }
    virtual B32 float_mul(B32 a, B32 b) { _stat["flt-mul"]++; return a.to_float() * b.to_float(); }
    virtual B32 float_div(B32 a, B32 b) { _stat["flt-div"]++; return a.to_float() / b.to_float(); }

    virtual B32 copy(B32 a) { _stat["copy"]++; return a; }
    virtual B1 search(B32 a, B32 p) { _stat["search"]++; return a.to_int() == p.to_int(); }
    virtual B8 pop_count(B32 a) { _stat["popcount"]++; return static_cast<unsigned char>(std::bitset<32>(a.to_uint()).count()); }
    virtual B32 if_else(B1 c, B32 a, B32 b) { _stat["if-else"]++; return c.to_bool() ? a : b; }
    virtual B32 int_ReLU(B32 a) { _stat["ReLU"]++; return a.to_int() > 0 ? a : 0; }
    virtual B32 uint_var_lshift(B32 a, B32 n) { _stat["var-lshift"]++; return a.to_uint() << n.to_uint(); }
    virtual B32 uint_var_rshift(B32 a, B32 n) { _stat["var-rshift"]++; return a.to_uint() >> n.to_uint(); }
};

#endif
