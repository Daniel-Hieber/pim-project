// File: Logic Unit Base Class
// Author: Deyuan Guo
// Date: June 2023

#ifndef LU_H
#define LU_H

#include <map>
#include <string>
#include "types.h"

class lu
{
public:
    lu() {}
    virtual ~lu() {}
    virtual std::string getName() { return "interface"; }

    // Public APIs with real data types for testing
    int logic_not(int a) { return logic_not(B32(a)).to_int(); }
    int logic_and(int a, int b) { return logic_and(B32(a), B32(b)).to_int(); }
    int logic_or(int a, int b) { return logic_or(B32(a), B32(b)).to_int(); }
    int logic_xor(int a, int b) { return logic_xor(B32(a), B32(b)).to_int(); }
    int logic_nand(int a, int b) { return logic_nand(B32(a), B32(b)).to_int(); }
    int logic_nor(int a, int b) { return logic_nor(B32(a), B32(b)).to_int(); }
    int logic_xnor(int a, int b) { return logic_xnor(B32(a), B32(b)).to_int(); }

    int int_add(int a, int b) { return int_add(B32(a), B32(b)).to_int(); }
    int int_sub(int a, int b) { return int_sub(B32(a), B32(b)).to_int(); }
    int int_abs(int a) { return int_abs(B32(a)).to_int(); }
    int int_min(int a, int b) { return int_min(B32(a), B32(b)).to_int(); }
    int int_max(int a, int b) { return int_max(B32(a), B32(b)).to_int(); }
    unsigned long long uint_mul(unsigned a, unsigned b) { return uint_mul(B32(a), B32(b)).to_ullong(); }
    unsigned uint_div(unsigned a, unsigned b) { return uint_div(B32(a), B32(b)).to_uint(); }
    unsigned uint_rem(unsigned a, unsigned b) { return uint_rem(B32(a), B32(b)).to_uint(); }

    bool uint_gt(unsigned a, unsigned b) { return uint_gt(B32(a), B32(b)).to_bool(); }
    bool uint_lt(unsigned a, unsigned b) { return uint_lt(B32(a), B32(b)).to_bool(); }
    bool int_gt(int a, int b) { return int_gt(B32(a), B32(b)).to_bool(); }
    bool int_lt(int a, int b) { return int_lt(B32(a), B32(b)).to_bool(); }
    bool int_eq(int a, int b) { return int_eq(B32(a), B32(b)).to_bool(); }

    float float_add(float a, float b) { return float_add(B32(a), B32(b)).to_float(); }
    float float_sub(float a, float b) { return float_sub(B32(a), B32(b)).to_float(); }
    float float_mul(float a, float b) { return float_mul(B32(a), B32(b)).to_float(); }
    float float_div(float a, float b) { return float_div(B32(a), B32(b)).to_float(); }

    int copy(int a) { return copy(B32(a)).to_int(); }
    bool search(int a, int p) { return search(B32(a), B32(p)).to_bool(); }
    int if_else(bool c, int a, int b) { return if_else(B1(c), B32(a), B32(b)).to_int(); }
    int ReLU(int a) { return int_ReLU(B32(a)).to_int(); }
    unsigned char pop_count(int a) { return pop_count(B32(a)).to_uchar(); }
    unsigned var_lshift(unsigned a, unsigned n) { return uint_var_lshift(B32(a), B32(n)).to_uint(); }
    unsigned var_rshift(unsigned a, unsigned n) { return uint_var_rshift(B32(a), B32(n)).to_uint(); }

    void print_stat(int repeat = 1) {
        int logicCnt = 0;
        std::string stat = "- STAT (" + getName() + "):";
        for (auto it = _stat.begin(); it != _stat.end(); ++it) {
            stat += " " + it->first + "=" + std::to_string(it->second / repeat);
            if (it->first != "R" && it->first != "W") {
                logicCnt += it->second / repeat;
            }
        }
        if (_stat.empty()) {
            std::cout << " ----" << std::endl;
        } else {
            std::cout << stat << " (#L=" << logicCnt << ")" << std::endl;
        }
    }
    void clear_stat() {
        _stat.clear();
    }

protected:
    // Internal APIs with bitset types for PIM
    virtual B32 logic_not(B32 a) { return ~a.to_uint(); }
    virtual B32 logic_and(B32 a, B32 b) { return a.to_uint() & b.to_uint(); }
    virtual B32 logic_or(B32 a, B32 b) { return a.to_uint() | b.to_uint(); }
    virtual B32 logic_xor(B32 a, B32 b) { return a.to_uint() ^ b.to_uint(); }
    virtual B32 logic_nand(B32 a, B32 b) { return ~(a.to_uint() & b.to_uint()); }
    virtual B32 logic_nor(B32 a, B32 b) { return ~(a.to_uint() | b.to_uint()); }
    virtual B32 logic_xnor(B32 a, B32 b) { return ~(a.to_uint() ^ b.to_uint()); }

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

    virtual B32 float_add(B32 a, B32 b) { return a.to_float() + b.to_float(); }
    virtual B32 float_sub(B32 a, B32 b) { return a.to_float() - b.to_float(); }
    virtual B32 float_mul(B32 a, B32 b) { return a.to_float() * b.to_float(); }
    virtual B32 float_div(B32 a, B32 b) { return a.to_float() / b.to_float(); }

    virtual B32 copy(B32 a) { return a; }
    virtual B1 search(B32 a, B32 p) { return a.to_int() == p.to_int(); }
    virtual B8 pop_count(B32 a) { return static_cast<unsigned char>(std::bitset<32>(a.to_uint()).count()); }
    virtual B32 if_else(B1 c, B32 a, B32 b) { return c.to_bool() ? a : b; }
    virtual B32 int_ReLU(B32 a) { return a.to_int() > 0 ? a : 0; }
    virtual B32 uint_var_lshift(B32 a, B32 n) { return (a.to_uint() << n.to_uint()); }
    virtual B32 uint_var_rshift(B32 a, B32 n) { return (a.to_uint() >> n.to_uint()); }
protected:
    std::map<std::string, int> _stat;
};

#endif
