// File: Logic Unit Class - SIMDRAM Implementation
// Author: Deyuan Guo
// Date: July 2023

#ifndef LU_SIMDRAM_H
#define LU_SIMDRAM_H

#include <map>
#include <string>
#include <bit>
#include "lu.h"

class lu_simdram : public lu
{
public:
    lu_simdram() {}
    virtual ~lu_simdram() {}
    virtual std::string getName() { return "simdram"; }

protected:
    virtual B32 logic_not(B32 a) { return ~a.to_uint(); }
    virtual B32 logic_and(B32 a, B32 b) { return a.to_uint() & b.to_uint(); }
    virtual B32 logic_or(B32 a, B32 b) { return a.to_uint() | b.to_uint(); }
    virtual B32 logic_xor(B32 a, B32 b) { return a.to_uint() ^ b.to_uint(); }
    virtual B32 logic_nand(B32 a, B32 b) { return ~(a.to_uint() & b.to_uint()); }
    virtual B32 logic_nor(B32 a, B32 b) { return ~(a.to_uint() | b.to_uint()); }
    virtual B32 logic_xnor(B32 a, B32 b) { return ~(a.to_uint() ^ b.to_uint()); }
    virtual B8 pop_count(B32 a) { return static_cast<unsigned char>(std::bitset<32>(a.to_uint()).count()); }

    virtual B32 int_add(B32 a, B32 b) { return a.to_int() + b.to_int(); }
    virtual B32 int_sub(B32 a, B32 b) { return a.to_int() - b.to_int(); }
    virtual B32 int_abs(B32 a) { return a.to_int() > 0 ? a.to_int() : -a.to_int(); }
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
    virtual B1 search(B32 a, B32 p) { return a.to_int() == p.to_int(); }

    virtual B32 float_add(B32 a, B32 b) { return a.to_float() + b.to_float(); }
    virtual B32 float_sub(B32 a, B32 b) { return a.to_float() - b.to_float(); }
    virtual B32 float_mul(B32 a, B32 b) { return a.to_float() * b.to_float(); }
    virtual B32 float_div(B32 a, B32 b) { return a.to_float() / b.to_float(); }

protected:

};

#endif
