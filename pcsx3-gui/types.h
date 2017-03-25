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

template< typename T > using le_ptr = T *;
template< typename T > using le_ref = T &;

template< typename T > T inline LoadBE(T * src) { return *src; };
template< typename T > inline void StoreBE(T * dst, T src) { *dst = src;  };

inline s16 LoadBE(s16 * src) { return _loadbe_i16(src); };
inline s32 LoadBE(s32 * src) { return _loadbe_i32(src); };
inline s64 LoadBE(s64 * src) { return _loadbe_i64(src); };

inline u16 LoadBE(u16 * src) { return _load_be_u16(src); };
inline u32 LoadBE(u32 * src) { return _load_be_u32(src); };
inline u64 LoadBE(u64 * src) { return _load_be_u64(src); };

inline void StoreBE(s16 * dst, s16 const src) { _storebe_i16(dst, src); };
inline void StoreBE(s32 * dst, s32 const src) { _storebe_i32(dst, src); };
inline void StoreBE(s64 * dst, s64 const src) { _storebe_i64(dst, src); };

inline void StoreBE(u16 * dst, u16 const src) { _store_be_u16(dst, src); };
inline void StoreBE(u32 * dst, u32 const src) { _store_be_u32(dst, src); };
inline void StoreBE(u64 * dst, u64 const src) { _store_be_u64(dst, src); };


template< typename T > inline void ReadBE(T & val)
{
    val = LoadBE(&val); // swap inplace
}

template< typename T > inline void WriteBE(T & val)
{
    StoreBE(&val, val); // swap inplace
}

#if 0
// when compiling in release mode, we got:
// pcsx3\pcsx3-gui\types.h(35): fatal error C1001: An internal error has occurred in the compiler.
// it looks as if MOVBE intrinsics are buggy under certain conditions.
template< typename T > struct be_ref
{
    T * be_ptr_;

    be_ref(T * ptr) : be_ptr_(ptr) {}

    operator T () { return LoadBE(be_ptr_); }
    be_ref< T > & operator = (T const value) { StoreBE(be_ptr_, T(value)); return *this; }
};

template< typename T > struct be_ptr
{
    be_ref< T > be_ref_;

    be_ptr(T * ptr = nullptr) : be_ref_(ptr) {}

    be_ref< T > & operator * () { return be_ref_; }
    be_ref< T > & operator -> () { return be_ref_; }
};
#endif

