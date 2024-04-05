// File: Logic Unit Class - DRAM-BitSIMD V 3-reg
// Author: Deyuan Guo
// Date: June 2023

#ifndef LU_BS3R_H
#define LU_BS3R_H

#include <map>
#include <string>
#include "lu.h"

class lu_bs3r : public lu_bs2r
{
public:
    lu_bs3r() : lu_bs2r(), _r1(0) {}
    virtual ~lu_bs3r() {}
    virtual std::string getName() { return "bs3r"; }

protected:
    virtual B64 uint_mul(B32 a, B32 b) {
        return karatsuba_mul(a, b, 32);
        //return uint_mul_helper(a, b, 32);
    }

    virtual B32 float_add(B32 a, B32 b) {
        B32 res = float32_add_sub(a, b, true/*add*/);
        return res;
    }

    virtual B32 float_sub(B32 a, B32 b) {
        B32 res = float32_add_sub(a, b, false/*sub*/);
        return res;
    }

    virtual B32 float_mul(B32 a, B32 b) {
        bool debug = 0;
        if (debug) std::cout << "flt-mul: " << a.str() << " * " << b.str() << std::endl;
        // step 1: extract significand
        B32 sigA = float32_extract_significand(a);
        B32 sigB = float32_extract_significand(b);
        if (debug) std::cout << "  sig-a: " << sigA.str() << " sig-b: " << sigB.str() << " extract"<< std::endl;
        // step 2: multiply significand
        B64 prod = uint_mul_helper(sigA, sigB, 24);
        if (debug) std::cout << "  prod : " << prod.str() << std::endl;
        // step 3: normalize 48-bit prod and generate exponent
        B1 hasOne = has_one(prod, 47, 0);
        B8 expDelta = lshift_and_get_exponent(prod, 48);
        if (debug) std::cout << "  prod : " << prod.str() << " shifted" << std::endl;
        if (debug) std::cout << "  exp-d: " << expDelta.str() << " has-one: " << hasOne.str() << std::endl;
        B1 isInf, isUf;
        B8 exp = float32_get_exponent_for_mul(a, b, expDelta, hasOne, isInf, isUf, debug);
        if (debug) std::cout << "  exp  : " << exp.str() << " is-inf: " << isInf.str() << " is-uf: " << isUf.str() << std::endl;

        B32 res;
        // sign
        _sa = op_read(a[31]);
        _cr = op_move(_sa);
        _sa = op_read(b[31]);
        _sa = op_xor(_cr, _sa);
        res.set(31, op_write(_sa));
        // exponent
        _sa = op_read(isInf[0]);
        _cr = op_move(_sa);
        _sa = op_read(isUf[0]);
        _pr = op_move(_sa);
        _r1 = op_set(0);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(exp[i]);
            _sa = op_or(_cr, _sa);
            _sa = op_sel(_pr, _r1, _sa);
            res.set(i + 23, op_write(_sa));
        }
        // mantissa
        _cr = op_not(_cr);
        for (int i = 0; i < 23; i++) {
            _sa = op_read(prod[i + 24]);
            _sa = op_and(_cr, _sa);
            _sa = op_sel(_pr, _r1, _sa);
            res.set(i, op_write(_sa));
        }
        if (debug) std::cout << "  res  : " << res.str() << std::endl;

        if (debug) {
            float x = a.to_float() * b.to_float();
            B32 t(x);
            if (debug) std::cout << "  gold : " << t.str() << " " << a.to_float() << " * " << b.to_float() << " = " << x << std::endl;
        }
        return res;
    }

    virtual B32 float_div(B32 a, B32 b) {
        int debug = 0;
        if (debug) std::cout << "flt-div: " << a.str() << " / " << b.str() << std::endl;
        // step 1: extract significand
        B32 sigA = float32_extract_significand(a);
        B32 sigB = float32_extract_significand(b);
        if (debug) std::cout << "  sig-a: " << sigA.str() << " sig-b: " << sigB.str() << " extract"<< std::endl;
        // step 2: divide significand
        B32 div = uint_div_helper_for_float32(sigA, sigB, debug > 1);
        if (debug) std::cout << "  div,r: " << div.str() << std::endl;
        // step 3: normalize 24-bit div and generate exponent
        B1 hasOne = has_one(div, 24, 0);
        B8 expDelta = lshift_and_get_exponent(div, 24);
        if (debug) std::cout << "  div  : " << div.str() << " shifted" << std::endl;
        if (debug) std::cout << "  exp-d: " << expDelta.str() << " has-one: " << hasOne.str() << std::endl;
        B1 isInf, isUf;
        B8 exp = float32_get_exponent_for_div(a, b, expDelta, hasOne, isInf, isUf, debug);
        if (debug) std::cout << "  exp  : " << exp.str() << " is-inf: " << isInf.str() << " is-uf: " << isUf.str() << std::endl;

        B32 res;
        // sign
        _sa = op_read(a[31]);
        _cr = op_move(_sa);
        _sa = op_read(b[31]);
        _sa = op_xor(_cr, _sa);
        res.set(31, op_write(_sa));
        // exponent
        _sa = op_read(isInf[0]);
        _cr = op_move(_sa);
        _sa = op_read(isUf[0]);
        _pr = op_move(_sa);
        _r1 = op_set(0);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(exp[i]);
            _sa = op_or(_cr, _sa);
            _sa = op_sel(_pr, _r1, _sa);
            res.set(i + 23, op_write(_sa));
        }
        // mantissa
        _cr = op_not(_cr);
        for (int i = 0; i < 23; i++) {
            _sa = op_read(div[i]);
            _sa = op_and(_cr, _sa);
            _sa = op_sel(_pr, _r1, _sa);
            res.set(i, op_write(_sa));
        }
        if (debug) std::cout << "  res  : " << res.str() << std::endl;

        if (debug) {
            float x = a.to_float() / b.to_float();
            B32 t(x);
            if (debug) std::cout << "  gold : " << t.str() << " " << a.to_float() << " / " << b.to_float() << " = " << x << std::endl;
        }
        return res;
    }

protected:

    B32 float32_add_sub(const B32 a, const B32 b, bool add) {
        bool debug = 0;
        B32 res;
        if (debug) std::cout << (add ? "flt-add: " : "flt-sub") << a.str() << (add ? " + " : " - ") << b.str() << std::endl;
        // step 1: diff exponents
        B1 expCmpLt; // exp(a) < exp(b)
        B1 expCmpGe; // exp(a) >= exp(b)
        B8 expDiff = float32_exponent_absdiff(a, b, expCmpLt, expCmpGe);
        // copy larger exp to result
        _pr = op_read(expCmpLt[0]);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(a[i + 23]);
            _cr = op_move(_sa);
            _sa = op_read(b[i + 23]);
            _sa = op_sel(_pr, _sa, _cr);
            res.set(i + 23, op_write(_sa));
        }
        if (debug) std::cout << "  exp-diff: " << expDiff.str() << " exp-lt: " << expCmpLt.str() << " exp-ge: " << expCmpGe.str() << std::endl;
        if (debug) std::cout << "  res  : " << res.str() << std::endl;

        // step 2: align significands
        B32 sigA = float32_extract_significand(a);
        B32 sigB = float32_extract_significand(b);
        if (debug) std::cout << "  sig-a: " << sigA.str() << " sig-b: " << sigB.str() << " extract"<< std::endl;
        // conditional variable right shift
        sigA = float32_significand_cond_var_rshift(sigA, expCmpLt, expDiff);
        sigB = float32_significand_cond_var_rshift(sigB, expCmpGe, expDiff);
        if (debug) std::cout << "  sig-a: " << sigA.str() << " sig-b: " << sigB.str() << " var-shift" << std::endl;
        // 26-bit sign extension
        sigA = sign_ext(sigA, a[31], 26);
        sigB = sign_ext(sigB, b[31], 26);
        if (debug) std::cout << "  sig-a: " << sigA.str() << " sig-b: " << sigB.str() << " sign-ext" << std::endl;

        // step 3: add significands
        B32 sig;
        if (add) {
            sig = int_add_helper(sigA, sigB, 26);
        } else {
            sig = int_sub_helper(sigA, sigB, 26);
        }
        if (debug) std::cout << "  sig-s: " << sig.str() << std::endl;

        // step 4: normalize result
        float32_normalization_for_add(res, sig, debug);

        if (debug) {
            float x = a.to_float() + b.to_float();
            B32 t(x);
            if (debug) std::cout << "  gold : " << t.str() << " " << a.to_float() << (add ? " + " : " - ") << b.to_float() << " = " << x << std::endl;
        }
        return res;
    }


    B8 float32_exponent_absdiff(const B32 a, const B32 b, B1& expCmpLt, B1& expCmpGe) {
        B8 expDiff; // abs(exp(a) - exp(b))
        _cr = op_set(0);
        for (int i = 0; i < 8; i++) { // sub
            _sa = op_read(a[i + 23]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i + 23]);
            _cr = op_sel(_pr, _cr, _sa);
            _pr = op_xor(_pr, _sa);
            _sa = op_move(_pr);
            expDiff.set(i, op_write(_sa));
        }
        _sa = op_move(_cr);
        expCmpLt.set(0, op_write(_sa)); // exp(a) < exp(b)
        _sa = op_not(_sa);
        expCmpGe.set(0, op_write(_sa)); // exp(a) < exp(b)
        _pr = op_move(_cr);
        for (int i = 0; i < 8; i++) { // abs
            _sa = op_read(expDiff[i]);
            _sa = op_xor(_sa, _pr);
            _sa = op_xor(_sa, _cr);
            expDiff.set(i, op_write(_sa));
            _sa = op_not(_sa);
            _cr = op_and(_sa, _cr);
        }
        return expDiff;
    }

    B32 float32_extract_significand(const B32 a) {
        B32 sig;
        // copy 23-bit unsigned mantissa
        _cr = op_set(0);
        for (int i = 0; i < 23; i++) {
            _sa = op_read(a[i]);
            _cr = op_or(_sa, _cr);
            sig.set(i, op_write(_sa));
        }
        // add leading 1
        // if exponent and mantissa are all zero, don't add leading 1
        for (int i = 23; i < 31; i++) {
            _sa = op_read(a[i]);
            _cr = op_or(_sa, _cr);
        }
        _sa = op_move(_cr);
        sig.set(23, op_write(_sa));
        // return 24-bit unsigned significand
        return sig;
    }

    B32 float32_significand_cond_var_rshift(const B32 sig, const B1 cond, const B8 step) {
        // copy 24-bit sig to res
        B32 res;
        for (int i = 0; i < 24; i++) {
            _sa = op_read(sig[i]);
            res.set(i, op_write(_sa));
        }
        // use the lowest 5 bits of step for conditional r shift
        _sa = op_read(cond[0]);
        _r1 = op_move(_sa);
        for (int i = 0; i < 5; i++) {
            _sa = op_read(step[i]);
            _pr = op_move(_sa);
            int step = (1 << i);
            for (int j = 0; j < 24 - step; j++) {
                _sa = op_read(res[j + step]);
                _cr = op_move(_sa);
                _sa = op_read(res[j]);
                _cr = op_sel(_pr, _cr, _sa); // var shift
                _sa = op_read(sig[j]);
                _sa = op_sel(_r1, _cr, _sa); // if !cond no change
                res.set(j, op_write(_sa));
            }
            for (int j = 0; j < step; j++) {
                _cr = op_set(0);
                _sa = op_read(res[23 - j]);
                _cr = op_sel(_pr, _cr, _sa);
                _sa = op_read(sig[23 - j]);
                _sa = op_sel(_r1, _cr, _sa);
                res.set(23 - j, op_write(_sa));
            }
        }
        // check other bits in step, if has 1, reset all to 0
        _pr = op_set(0);
        for (int i = 5; i < 8; i++) {
            _sa = op_read(step[i]);
            _pr = op_or(_pr, _sa);
        }
        _pr = op_and(_r1, _pr);  // only reset when cond is 1 and see 1s
        _cr = op_set(0);
        for (int i = 0; i < 24; i++) {
            _sa = op_read(res[i]);
            _sa = op_sel(_pr, _cr, _sa);
            res.set(i, op_write(_sa));
        }

        return res;
    }

    B32 sign_ext(const B32 val, const B1 sign, const int nBit) {
        B32 res;
        _sa = op_read(sign[0]);
        _cr = op_move(_sa);
        _pr = op_move(_sa);
        for (int i = 0; i < nBit; i++) {
            _sa = op_read(val[i]);
            _sa = op_xor(_sa, _pr);
            _sa = op_xor(_sa, _cr);
            res.set(i, op_write(_sa));
            _sa = op_not(_sa);
            _cr = op_and(_sa, _cr);
        }
        return res;
    }

    B32 int_add_helper(B32 a, B32 b, int nBit) {
        B32 res;
        _cr = op_set(0);
        for (int i = 0; i < nBit; i++) {
            _sa = op_read(b[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(a[i]);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }

    B32 int_sub_helper(B32 a, B32 b, int nBit) {
        B32 res;
        _cr = op_set(0);
        for (int i = 0; i < nBit; i++) {
            _sa = op_read(a[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(b[i]);
            _cr = op_sel(_pr, _cr, _sa);
            _sa = op_xor(_pr, _sa);
            res.set(i, op_write(_sa));
        }
        return res;
    }

    void float32_normalization_for_add(B32& res, B32 sig, bool debug = false) {
        // get sign bit
        _sa = op_read(sig[25]);
        res.set(31, op_write(_sa));
        if (debug) std::cout << "  res  : " << res.str() << std::endl;
        // abs of 26-bit significand
        _cr = op_move(_sa);
        _pr = op_move(_sa);
        for (int i = 0; i < 26; i++) {
            _sa = op_read(sig[i]);
            _sa = op_xor(_sa, _pr);
            _sa = op_xor(_sa, _cr);
            sig.set(i, op_write(_sa));
            _sa = op_not(_sa);
            _cr = op_and(_sa, _cr);
        }
        if (debug) std::cout << "  sig-u: " << sig.str() << std::endl;
        // left shift unsigned significand leading zero to bit 24
        // if all zero, don't shift
        B1 hasOne = has_one(sig, 24, 0);
        B8 expDelta = lshift_and_get_exponent(sig, 25);
        if (debug) std::cout << "  sig-u: " << sig.str() << " exp-d: " << expDelta.str() << std::endl;
        // subtract exp delta by 1
        _cr = op_set(1);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(expDelta[i]);
            _sa = op_xor(_sa, _cr);
            expDelta.set(i, op_write(_sa));
            _cr = op_and(_sa, _cr);
        }
        if (debug) std::cout << "  sig-u: " << sig.str() << " exp-d: " << expDelta.str() << std::endl;
        // sub exp delta from final delta
        // but if result is 0, reset exp to 0
        _cr = op_set(0);
        _sa = op_read(hasOne[0]);
        _r1 = op_move(_sa);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(res[i + 23]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(expDelta[i]);
            _cr = op_sel(_pr, _cr, _sa);
            _sa = op_xor(_pr, _sa);
            _sa = op_and(_sa, _r1);
            res.set(i + 23, op_write(_sa));
        }
        if (debug) std::cout << "  res  : " << res.str() << std::endl;
        // copy fraction
        for (int i = 0; i < 23; i++) {
            _sa = op_read(sig[i + 1]);
            res.set(i, op_write(_sa));
        }
        if (debug) std::cout << "  res  : " << res.str() << std::endl;
    }

    // Left shift the value till the leading 1 is at msb index.
    // Generate exponent value based on the log shifting.
    template <typename T>
    B8 lshift_and_get_exponent(T& val, int nBit) {
        B8 expDelta;
        int pass = 5; // for nBit in 16 ~ 31 range
        for (int i = pass - 1; i >= 0; i--) {
            // determine shift condition
            // can only shift if all bits are zero in the range
            int range = (1 << i);
            int ub = nBit - 1;
            int lb = nBit - range;
            _pr = op_set(0);
            for (int j = lb; j <= ub; j++) {
                _sa = op_read(val[j]);
                _pr = op_or(_sa, _pr);
            }
            _pr = op_not(_pr); // all zero
            // conditional left shift
            for (int j = ub; j - range >= 0; j--) {
                _sa = op_read(val[j]);
                _cr = op_move(_sa);
                _sa = op_read(val[j - range]);
                _sa = op_sel(_pr, _sa, _cr);
                val.set(j, op_write(_sa));
            }
            _cr = op_set(0);
            for (int j = range - 1; j >= 0; j--) { // fill the rest to 0
                _sa = op_read(val[j]);
                _sa = op_sel(_pr, _cr, _sa);
                val.set(j, op_write(_sa));
            }
            // save to exponent
            _sa = op_move(_pr);
            expDelta.set(i, op_write(_sa));
            //std::cout << " << val " << val.str() << " exp " << expDelta.str() << std::endl;
        }
        return expDelta;
    }

    template <typename T>
    B1 has_one(const T& val, int lb, int rb) {
        B1 res;
        _cr = op_set(0);
        for (int i = rb; i <= lb; i++) {
            _sa = op_read(val[i]);
            _cr = op_or(_cr, _sa);
        }
        _sa = op_move(_cr);
        res.set(0, op_write(_sa));
        return res;
    }

    B64 uint_mul_helper(B64 a, B64 b, int nBit = 32) {
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
            _pr = op_read(a[i]);
            _cr = op_set(0);
            for (int j = 0; j < nBit; j++) {
                // 1-bit adder
                _sa = op_read(b[j]);
                _r1 = op_xor(_sa, _cr);
                _sa = op_read(res[i + j]);
                _cr = op_sel(_r1, _sa, _cr); // carry'
                _r1 = op_xor(_r1, _sa); // sum
                _sa = op_sel(_pr, _r1, _sa); // conditional
                res.set(i + j, op_write(_sa));
            }
            _sa = op_read(res[i + nBit]);
            _sa = op_sel(_pr, _cr, _sa);
            res.set(i + nBit, op_write(_sa));
        }
        return res;
    }

    B8 float32_get_exponent_for_mul(const B32 a, const B32 b, const B8 expDelta, const B1 hasOne, B1& isInf, B1& isUf, bool debug) {
        // use B16 as exponent may exceed 8-bit
        B16 expA;
        B16 expB;
        B16 expD;
        for (int i = 0; i < 8; i++) { // no-cost redirect
            expA.set(i, a[i + 23]);
            expB.set(i, b[i + 23]);
            expD.set(i, expDelta[i]);
        }
        // add expA and expB to a 9-bit number
        B16 exp;
        _cr = op_set(0);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(expA[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(expB[i]);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            exp.set(i, op_write(_sa));
        }
        exp.set(8, _cr);
        if (debug) std::cout << "  exp  : " << exp.str() << " a+b" << std::endl;
        // add expDelta with 127 ofst, and also adjust by 1, so +126
        _cr = op_set(0);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(expD[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(i < 7 && i > 0);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            expD.set(i, op_write(_sa));
        }
        expD.set(8, _cr);
        if (debug) std::cout << "  exp-d: " << expD.str() << " +ofst" << std::endl;
        // subtract exp delta from final delta
        _sa = op_read(hasOne[0]);
        _r1 = op_move(_sa);
        _cr = op_set(0);
        for (int i = 0; i < 9; i++) {
            _sa = op_read(exp[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(expD[i]);
            _cr = op_sel(_pr, _cr, _sa);
            _sa = op_xor(_pr, _sa);
            _sa = op_and(_sa, _r1);
            exp.set(i, op_write(_sa));
        }
        if (debug) std::cout << "  exp  : " << exp.str() << " -delta br=" << _cr << std::endl;
        B8 res;
        for (int i = 0; i < 8; i++) { // no-cost redirect
            res.set(i, exp[i]);
        }
        // check exponent overflow and underflow
        _sa = op_move(_cr);
        isUf.set(0, op_write(_sa));
        _sa = op_read(exp[8]);
        isInf.set(0, op_write(_sa));
        return res;
    }

    B8 float32_get_exponent_for_div(const B32 a, const B32 b, const B8 expDelta, const B1 hasOne, B1& isInf, B1& isUf, bool debug) {
        // use B16 as exponent may exceed 8-bit
        B16 expA;
        B16 expB;
        B16 expD;
        for (int i = 0; i < 8; i++) { // no-cost redirect
            expA.set(i, a[i + 23]);
            expB.set(i, b[i + 23]);
            expD.set(i, expDelta[i]);
        }
        // compute expA + 127 ofst
        B16 exp;
        _cr = op_set(0);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(expA[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(i < 7);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            exp.set(i, op_write(_sa));
        }
        exp.set(8, _cr);
        if (debug) std::cout << "  exp  : " << exp.str() << " a+ofst" << std::endl;
        // compute expB + expDelta
        _cr = op_set(0);
        for (int i = 0; i < 8; i++) {
            _sa = op_read(expD[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(expB[i]);
            _cr = op_sel(_pr, _sa, _cr);
            _sa = op_xor(_pr, _sa);
            expD.set(i, op_write(_sa));
        }
        expD.set(8, _cr);
        if (debug) std::cout << "  exp-d: " << exp.str() << " b+delta" << std::endl;
        // subtract exp delta from final delta
        _sa = op_read(hasOne[0]);
        _r1 = op_move(_sa);
        _cr = op_set(0);
        for (int i = 0; i < 10; i++) {
            _sa = op_read(exp[i]);
            _pr = op_xor(_sa, _cr);
            _sa = op_read(expD[i]);
            _cr = op_sel(_pr, _cr, _sa);
            _sa = op_xor(_pr, _sa);
            _sa = op_and(_sa, _r1);
            exp.set(i, op_write(_sa));
        }
        if (debug) std::cout << "  exp  : " << exp.str() << " sub br=" << _cr << std::endl;
        B8 res;
        for (int i = 0; i < 8; i++) { // no-cost redirect
            res.set(i, exp[i]);
        }
        // check exponent overflow and underflow
        _sa = op_move(_cr);
        isUf.set(0, op_write(_sa));
        _sa = op_read(exp[8]);
        isInf.set(0, op_write(_sa));
        return res;
    }

    // for float32 div, nBit = 24
    // when calculating div, need to get at least 24 bit result
    // so place a's significand to bit 48 and B's to bit 24
    B32 uint_div_helper_for_float32(const B32 sigA, const B32 sigB, bool debug) {
        int nBit = 24;
        B64 qrs; // quotient, remainder
        B32 test;
        B32 res;
        // init 64-bit space
        _sa = op_set(0);
        for (int i = 0; i < nBit; i++) {
            qrs.set(i + nBit, sigA[i]); // no-cost redirect
            qrs.set(i, op_write(_sa));
        }
        // compute
        for (int i = 0; i < nBit; i++) {
            if (debug) {
                std::cout << "qrs : " << qrs.str() << std::endl;
                std::cout << "sigB: ";
                for (int k = 0; k < 8 + i; k++) {
                    std::cout << " ";
                }
                std::cout << sigB.str() << std::endl;
            }

            // test qrs - b
            _cr = op_set(0);
            for (int j = 0; j <= nBit; j++) {
                _sa = op_read(qrs[nBit - i + j]);
                _pr = op_xor(_sa, _cr);
                if (j == nBit) {
                  _sa = op_set(0);
                } else {
                  _sa = op_read(sigB[j]);
                }
                _cr = op_sel(_pr, _cr, _sa);
                _pr = op_xor(_pr, _sa);
                _sa = op_move(_pr);
                test.set(j, op_write(_sa));
            }
            if (debug) {
                std::cout << "test: ";
                for (int k = 0; k < 8 + i; k++) {
                    std::cout << " ";
                }
                std::cout << test.str() << " br=" << _cr << std::endl;
            }

            // update quotient - if positive, set quotient bit to 1
            _cr = op_not(_cr);
            _sa = op_move(_cr);
            res.set(nBit - 1 - i, op_write(_sa));
            if (debug) {
                std::cout << "res : ";
                for (int k = 0; k < 8 + i; k++) {
                    std::cout << " ";
                }
                std::cout << res.str() << std::endl;
            }

            // update remainder - if positive, copy sub result to qrs
            _pr = op_move(_cr);
            for (int j = 0; j < nBit; j++) {
                _sa = op_read(test[j]);
                _cr = op_move(_sa);
                _sa = op_read(qrs[nBit - i + j]);
                _sa = op_sel(_pr, _cr, _sa);
                qrs.set(nBit - i + j, op_write(_sa));
            }
        }
        return res;
    }

    bool _r1;
};

#endif
