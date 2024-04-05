// File: Data Types
// Author: Deyuan Guo
// Date: June 2023

#ifndef MY_TYPES_H
#define MY_TYPES_H

#include <bitset>

template<std::size_t N>
class MyBitset {
    public:
        MyBitset() {}
        ~MyBitset() {}

        bool operator[](size_t pos) const { return _bits.test(pos); }
        void set(size_t pos, bool val = true) { _bits.set(pos, val); }
        void reset(size_t pos) { _bits.reset(pos); }
    protected:
        std::bitset<N> _bits;
};

class B1 : public MyBitset<1> {
    public:
        B1() {}
        B1(bool val) { _bits = std::bitset<1>(val); }

        std::string str() const { return _bits.to_string(); }
        bool to_bool() const { return static_cast<bool>(_bits.to_ulong()); }
};

class B8 : public MyBitset<8> {
    public:
        B8() {}
        B8(char val) { _bits = std::bitset<8>(val); }
        B8(unsigned char val) { _bits = std::bitset<8>(val); }

        std::string str() const { return _bits.to_string(); }
        char to_char() const { return static_cast<char>(_bits.to_ulong()); }
        unsigned char to_uchar() const { return static_cast<unsigned char>(_bits.to_ulong()); }
};

class B16 : public MyBitset<16> {
    public:
        B16() {}
        B16(short val) { _bits = std::bitset<16>(val); }
        B16(unsigned short val) { _bits = std::bitset<16>(val); }

        std::string str() const { return _bits.to_string(); }
        short to_short() const { return static_cast<short>(_bits.to_ulong()); }
        unsigned short to_ushort() const { return static_cast<unsigned short>(_bits.to_ulong()); }
};

class B32 : public MyBitset<32> { public: B32() {}
        B32(int val) { _bits = std::bitset<32>(val); }
        B32(unsigned val) { _bits = std::bitset<32>(val); }
        B32(float val) { _bits = std::bitset<32>(*reinterpret_cast<unsigned*>(&val)); }

        std::string str() const { return _bits.to_string(); }
        int to_int() const { return static_cast<int>(_bits.to_ulong()); }
        unsigned to_uint() const { return static_cast<unsigned>(_bits.to_ulong()); }
        float to_float() const { unsigned t = _bits.to_ulong(); return *reinterpret_cast<float*>(&t); }
};

class B64 : public MyBitset<64> {
    public:
        B64() {}
        B64(long long val) { _bits = std::bitset<64>(val); }
        B64(unsigned long long val) { _bits = std::bitset<64>(val); }
        B64(double val) { _bits = std::bitset<64>(*reinterpret_cast<unsigned long long*>(&val)); }
        B64(B32 val) { _bits = std::bitset<64>(val.to_uint()); }

        std::string str() const { return _bits.to_string(); }
        long long to_llong() const { return static_cast<long long>(_bits.to_ullong()); }
        unsigned long long to_ullong() const { return static_cast<unsigned long long>(_bits.to_ullong()); }
        double to_double() const { unsigned long long t = _bits.to_ullong(); return *reinterpret_cast<double*>(&t); }
};

class B256 : public MyBitset<256> {
    public:
        B256() {}
        std::string str() const { return _bits.to_string(); }
};

#endif

