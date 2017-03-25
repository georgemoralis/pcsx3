#pragma once

using S08 = char;
using S16 = short;
using S32 = int;
using S64 = long long;

using U08 = unsigned char;
using U16 = unsigned short;
using U32 = unsigned int;
using U64 = unsigned long long;

using F32 = float;
using F64 = double;

using i8  = char;
using i16 = short int;
using i32 = int;
using i64 = long long int;

using s8  = i8;
using s16 = i16;
using s32 = i32;
using s64 = i64;

using u8  = unsigned char;
using u16 = unsigned short int;
using u32 = unsigned int;
using u64 = unsigned long long int;

template< typename T > using le_ptr = T *;
template< typename T > using le_ref = T &;

#include <immintrin.h>

template< typename T > T inline LoadBE(T & src) { return src; };
template< typename T > inline void StoreBE(T & dst, T src) { dst = src;  };

inline s16 LoadBE(s16 & src) { return _loadbe_i16(&src); };
inline s32 LoadBE(s32 & src) { return _loadbe_i32(&src); };
inline s64 LoadBE(s64 & src) { return _loadbe_i64(&src); };

inline u16 LoadBE(u16 & src) { return _load_be_u16(&src); };
inline u32 LoadBE(u32 & src) { return _load_be_u32(&src); };
inline u64 LoadBE(u64 & src) { return _load_be_u64(&src); };

inline void StoreBE(s16 & dst, s16 src) { _storebe_i16(&dst, src); };
inline void StoreBE(s32 & dst, s32 src) { _storebe_i32(&dst, src); };
inline void StoreBE(s64 & dst, s64 src) { _storebe_i64(&dst, src); };

inline void StoreBE(u16 & dst, u16 src) { _store_be_u16(&dst, src); };
inline void StoreBE(u32 & dst, u32 src) { _store_be_u32(&dst, src); };
inline void StoreBE(u64 & dst, u64 src) { _store_be_u64(&dst, src); };


template< typename T > inline void ReadBE(T & val)
{
    val = LoadBE(val); // swap inplace
}

template< typename T > inline void WriteBE(T & val)
{
    StoreBE(val, val); // swap inplace
}
