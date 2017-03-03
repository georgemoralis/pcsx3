#pragma once
#include <string>
#include "types.h"

inline U16 swap16(U16 _data)
{
	return _byteswap_ushort(_data);
}
inline U32 swap32(U32 _data)
{
	return _byteswap_ulong(_data);
}
inline U64 swap64(U64 _data)
{
	return _byteswap_uint64(_data);
}
inline U16 swap16(const U08* data)
{
	U16 value;
	std::memcpy(&value, data, sizeof(U16));

	return swap16(value);
}
inline U32 swap32(const U08* data)
{
	U32 value;
	std::memcpy(&value, data, sizeof(U32));

	return swap32(value);
}
inline U64 swap64(const U08* data)
{
	U64 value;
	std::memcpy(&value, data, sizeof(U64));

	return swap64(value);
}

template <int count>
void swap(U08*);

template <>
inline void swap<1>(U08* data)
{
}

template <>
inline void swap<2>(U08* data)
{
	const U16 value = swap16(data);

	std::memcpy(data, &value, sizeof(U16));
}

template <>
inline void swap<4>(U08* data)
{
	const U32 value = swap32(data);

	std::memcpy(data, &value, sizeof(U32));
}

template <>
inline void swap<8>(U08* data)
{
	const U64 value = swap64(data);

	std::memcpy(data, &value, sizeof(U64));
}

template <typename T>
inline T FromBigEndian(T data)
{
	static_assert(std::is_arithmetic<T>::value, "function only makes sense with arithmetic types");

	swap<sizeof(data)>(reinterpret_cast<U08*>(&data));
	return data;
}