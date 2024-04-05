// File: DRAM-BitSIMD Functional Simulator
// Author: Deyuan Guo
// Date: June 2023

#include <iostream>
#include <cstdlib>
#include <string>
#include <random>
#include <limits>
#include <vector>
#include <iomanip>

#include "lu.h"
#include "lu_base.h"
#include "lu_bitwise.h"
#include "lu_bs2r.h"
#include "lu_bs3r.h"
#include "lu_nand.h"
#include "lu_maj3.h"
#include "lu_simdram.h"
#include "lu_cram.h"
#include "lu_xnor.h"

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist6(INT_MIN, INT_MAX);
std::uniform_real_distribution<float> distF(-1e30, 1e30);

bool show_float_error = false;

int get_rand_int() {
    return dist6(rng);
}

float get_rand_float() {
    return distF(rng);
}

void append_rand_int(std::vector<int>& vec, int n) {
    for (int i = 0; i < n; i++) {
        vec.push_back(get_rand_int());
    }
}

void append_rand_uint(std::vector<unsigned>& vec, int n) {
    for (int i = 0; i < n; i++) {
        vec.push_back(static_cast<unsigned>(get_rand_int()));
    }
}

void append_rand_float(std::vector<float>& vec, int n) {
    for (int i = 0; i < n; i++) {
        vec.push_back(get_rand_float());
    }
}

void print_summary(const std::string& inst, int passed, int total, lu* base, lu* test) {
    std::string mark = (passed == total ? " passed" : " <---------------");
    std::cout << inst << ": " << passed << " / " << total << mark << std::endl;
    base->print_stat(total);
    test->print_stat(total);
    base->clear_stat();
    test->clear_stat();
}

///////////////////////////////////////////////////////////////////////////////
// Test Functions
///////////////////////////////////////////////////////////////////////////////

void test_logic_not(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        int x = base->logic_not(i);
        int y = test->logic_not(i);
        total++;
        if (x == y) { passed++; }
        else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
    }
    print_summary("NOT", passed, total, base, test);
}

void test_logic_and(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->logic_and(i, j);
            int y = test->logic_and(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("AND", passed, total, base, test);
}

void test_logic_or(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->logic_or(i, j);
            int y = test->logic_or(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("OR", passed, total, base, test);
}

void test_logic_xor(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->logic_xor(i, j);
            int y = test->logic_xor(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("XOR", passed, total, base, test);
}

void test_logic_nand(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->logic_nand(i, j);
            int y = test->logic_nand(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("NAND", passed, total, base, test);
}

void test_logic_nor(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->logic_nor(i, j);
            int y = test->logic_nor(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("NOR", passed, total, base, test);
}

void test_logic_xnor(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->logic_xnor(i, j);
            int y = test->logic_xnor(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("XNOR", passed, total, base, test);
}

void test_int_add(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->int_add(i, j);
            int y = test->int_add(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-ADD", passed, total, base, test);
}

void test_int_sub(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->int_sub(i, j);
            int y = test->int_sub(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-SUB", passed, total, base, test);
}

void test_int_abs(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        int x = base->int_abs(i);
        int y = test->int_abs(i);
        total++;
        if (x == y) { passed++; }
        else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
    }
    print_summary("INT-ABS", passed, total, base, test);
}

void test_int_min(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->int_min(i, j);
            int y = test->int_min(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-MIN", passed, total, base, test);
}

void test_int_max(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->int_max(i, j);
            int y = test->int_max(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-MAX", passed, total, base, test);
}

void test_int_gt(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            bool x = base->int_gt(i, j);
            bool y = test->int_gt(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-GT", passed, total, base, test);
}

void test_int_lt(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            bool x = base->int_lt(i, j);
            bool y = test->int_lt(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-LT", passed, total, base, test);
}

void test_int_eq(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->int_eq(i, j);
            int y = test->int_eq(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-EQ", passed, total, base, test);
}

void test_uint_mul(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 2, 3, 255};
    append_rand_uint(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            unsigned long long x = base->uint_mul(i, j);
            unsigned long long y = test->uint_mul(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-MUL", passed, total, base, test);
}

void test_uint_div(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {1, 2, 3, 5, 255};
    append_rand_uint(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            unsigned x = base->uint_div(i, j);
            unsigned y = test->uint_div(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-DIV", passed, total, base, test);
}

void test_uint_rem(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 2, 3, 255};
    append_rand_uint(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            unsigned x = base->uint_rem(i, j);
            unsigned y = test->uint_rem(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-REM", passed, total, base, test);
}

void test_uint_gt(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 2, 3, 255};
    append_rand_uint(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            bool x = base->uint_gt(i, j);
            bool y = test->uint_gt(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-GT", passed, total, base, test);
}

void test_uint_lt(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 2, 3, 255};
    append_rand_uint(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            bool x = base->uint_lt(i, j);
            bool y = test->uint_lt(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-LT", passed, total, base, test);
}

void test_float_add(lu* base, lu* test)
{
    std::vector<float> vec1 = {0.0f, 1.0f, -1.0f, 10.3f, -10.3f};
    append_rand_float(vec1, 3);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            float x = base->float_add(i, j);
            float y = test->float_add(i, j);
            total++;
            if (x == y) { passed++; }
            if (show_float_error) {
                float resMax = std::abs(std::max(x, y));
                float err = std::abs(x - y);
                float errRatio = (resMax == 0 ? 0 : err / resMax);
                printf("Result: %15.8g + %15.8g = %15.8g (ref %15.8g , err %15.8g, ratio %1.1g)\n", i, j, y, x, x - y, errRatio);
            }
        }
    }
    print_summary("FLT-ADD", passed, total, base, test);
}

void test_float_sub(lu* base, lu* test)
{
    std::vector<float> vec1 = {0.0f, 1.0f, -1.0f, 10.3f, -10.3f};
    append_rand_float(vec1, 3);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            float x = base->float_sub(i, j);
            float y = test->float_sub(i, j);
            total++;
            if (x == y) { passed++; }
            if (show_float_error) {
                float resMax = std::abs(std::max(x, y));
                float err = std::abs(x - y);
                float errRatio = (resMax == 0 ? 0 : err / resMax);
                printf("Result: %15.8g - %15.8g = %15.8g (ref %15.8g , err %15.8g, ratio %1.1g)\n", i, j, y, x, x - y, errRatio);
            }
        }
    }
    print_summary("FLT-SUB", passed, total, base, test);
}

void test_float_mul(lu* base, lu* test)
{
    std::vector<float> vec1 = {0.0f, 1.0f, -1.0f, 10.3f, -10.3f, 1e-30};
    append_rand_float(vec1, 2);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            float x = base->float_mul(i, j);
            float y = test->float_mul(i, j);
            total++;
            if (x == y) { passed++; }
            if (show_float_error) {
                float resMax = std::abs(std::max(x, y));
                float err = std::abs(x - y);
                float errRatio = (resMax == 0 ? 0 : err / resMax);
                printf("Result: %15.8g * %15.8g = %15.8g (ref %15.8g , err %15.8g, ratio %1.1g)\n", i, j, y, x, x - y, errRatio);
            }
        }
    }
    print_summary("FLT-MUL", passed, total, base, test);
}

void test_float_div(lu* base, lu* test)
{
    //std::vector<float> vec1 = {0.0f, 1.0f, -1.0f, 10.3f, -10.3f};
    //std::vector<float> vec2 = {1.0f, -1.0f, 10.3f, -10.3f};
    std::vector<float> vec1 = {0, 1, 5, 15, -7, -1011, 1e-30};
    append_rand_float(vec1, 3);
    std::vector<float> vec2 = {1, 6, -13, 1e-30};
    append_rand_float(vec2, 2);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec2) {
            float x = base->float_div(i, j);
            float y = test->float_div(i, j);
            total++;
            if (x == y) { passed++; }
            if (show_float_error) {
                float resMax = std::abs(std::max(x, y));
                float err = std::abs(x - y);
                float errRatio = (resMax == 0 ? 0 : err / resMax);
                printf("Result: %15.8g / %15.8g = %15.8g (ref %15.8g , err %15.8g, ratio %1.1g)\n", i, j, y, x, x - y, errRatio);
            }
        }
    }
    print_summary("FLT-DIV", passed, total, base, test);
}

void test_int_copy(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        int x = base->copy(i);
        int y = test->copy(i);
        total++;
        if (x == y) { passed++; }
        else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
    }
    print_summary("INT-COPY", passed, total, base, test);
}

void test_int_search(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : vec1) {
            int x = base->search(i, j);
            int y = test->search(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-SEARCH", passed, total, base, test);
}

void test_int_if_else(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);
    std::vector<bool> cond = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

    int total = 0;
    int passed = 0;
    for (size_t i = 0; i < vec1.size(); i++) {
        int a = vec1[i];
        bool c = cond[i];
        for (auto j : vec1) {
            int x = base->if_else(c, a, j);
            int y = test->if_else(c, a, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("INT-IF-ELSE", passed, total, base, test);
}

void test_int_ReLU(lu* base, lu* test)
{
    std::vector<int> vec1 = {0, 1, -1, 2, -2};
    append_rand_int(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        int x = base->ReLU(i);
        int y = test->ReLU(i);
        total++;
        if (x == y) { passed++; }
        else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
    }
    print_summary("INT-ReLU", passed, total, base, test);
}

void test_pop_count(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 255, 1024, 0xFFFFFFFF};
    append_rand_uint(vec1, 5);

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        unsigned char x = base->pop_count(i);
        unsigned char y = test->pop_count(i);
        total++;
        if (x == y) { passed++; }
        else std::cout << "Error: " << i << " " << x << " " << y << std::endl;
    }
    print_summary("POPCOUNT", passed, total, base, test);
}

void test_uint_var_lshift(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 2, 3, 255};
    append_rand_uint(vec1, 5);
    // >5-bit shift step causes UB
    std::vector<unsigned> step = {0, 1, 2, 3, 4, 8, 11, 15, 16, 31};

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : step) {
            auto x = base->var_lshift(i, j);
            auto y = test->var_lshift(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-VAR-LSHIFT", passed, total, base, test);
}

void test_uint_var_rshift(lu* base, lu* test)
{
    std::vector<unsigned> vec1 = {0, 1, 2, 3, 255};
    append_rand_uint(vec1, 5);
    // >5-bit shift step causes UB
    std::vector<unsigned> step = {0, 1, 2, 3, 4, 8, 11, 15, 16, 31};

    int total = 0;
    int passed = 0;
    for (auto i : vec1) {
        for (auto j : step) {
            auto x = base->var_rshift(i, j);
            auto y = test->var_rshift(i, j);
            total++;
            if (x == y) { passed++; }
            else std::cout << "Error: " << i << " " << j << " " << x << " " << y << std::endl;
        }
    }
    print_summary("UINT-VAR-RSHIFT", passed, total, base, test);
}

void test_all_ops(lu* base, lu* test)
{
    std::cout << "\nTest bit-wise logical operations:" << std::endl;
    test_logic_not(base, test);
    test_logic_and(base, test);
    test_logic_or(base, test);
    test_logic_xor(base, test);
    test_logic_nand(base, test);
    test_logic_nor(base, test);
    test_logic_xnor(base, test);

    std::cout << "\nTest integer arithmetic operations:" << std::endl;
    test_int_add(base, test);
    test_int_sub(base, test);
    test_int_abs(base, test);
    test_int_min(base, test);
    test_int_max(base, test);

    std::cout << "\nTest unsigned integer arithmetic operations:" << std::endl;
    test_uint_mul(base, test);
    test_uint_div(base, test);
    test_uint_rem(base, test);

    std::cout << "\nTest integer relational operations:" << std::endl;
    test_uint_gt(base, test);
    test_uint_lt(base, test);
    test_int_gt(base, test);
    test_int_lt(base, test);
    test_int_eq(base, test);

    std::cout << "\nTest floating-point operations:" << std::endl;
    test_float_add(base, test);
    test_float_sub(base, test);
    test_float_mul(base, test);
    test_float_div(base, test);

    std::cout << "\nTest other misc operations:" << std::endl;
    test_int_copy(base, test);
    test_int_search(base, test);
    test_int_if_else(base, test);
    test_int_ReLU(base, test);
    test_pop_count(base, test);
    test_uint_var_lshift(base, test);
    test_uint_var_rshift(base, test);
}

///////////////////////////////////////////////////////////////////////////////
// Main Function
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    std::cout << "DRAM-BitSIMD functional similator" << std::endl;

    if (argc != 2) {
        std::cout << "Usage: ./main.out <arch>" << std::endl;
        std::cout << "Supported arch: base, bitwise, bs2r, bs3r, nand, maj3, xnor, simdram, cram" << std::endl;
        return 0;
    }
    std::string arch = argv[1];

    lu* base = new lu_base();
    lu* test = nullptr;
    if (arch == "base") { test = new lu_base(); }
    else if (arch == "bitwise") { test = new lu_bitwise(); }
    else if (arch == "bs2r") { test = new lu_bs2r(); }
    else if (arch == "bs3r") { test = new lu_bs3r(); }
    else if (arch == "nand") { test = new lu_nand(); }
    else if (arch == "maj3") { test = new lu_maj3(); }
    else if (arch == "xnor") { test = new lu_xnor(); }
    else if (arch == "simdram") { test = new lu_simdram(); }
    else if (arch == "cram") { test = new lu_cram(); }
    else {
        std::cout << "Unknown arch: " << arch << std::endl;
        return 0;
    }
    assert(base && test);

    std::cout << "Compare design " << base->getName() << " vs. " << test->getName() << std::endl;

    test_all_ops(base, test);

    delete base;
    delete test;
    return 0;
}
