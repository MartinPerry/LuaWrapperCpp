#ifndef MURMUR_HASH_CONSTEXPR_H
#define MURMUR_HASH_CONSTEXPR_H

#include "./MyStringMacros.h"


#ifndef CONSTEXPR_DISABLE


#include <stdint.h>

#ifdef _MSC_VER
	//https://msdn.microsoft.com/cs-cz/library/2c8f766e.aspx
	#pragma warning( push )
	#pragma warning( disable : 4592 )
#endif


//128 bit murmur hash
//http://sizeofvoid.blogspot.cz/2012/07/compile-time-hashes.html

//
//http://roartindon.blogspot.cz/2014/10/compile-time-murmur-hash-in-c.html



//compile time murmur3_x86_32 hash (gcc)
//https://gist.github.com/mattyclarkson/5318077

constexpr inline size_t StringLengthCExpr(const char * const str) noexcept
{
	return (*str == 0) ? 0 : StringLengthCExpr(str + 1) + 1;
};





constexpr inline uint32_t Murmur3Rotate_32CExpr(const uint32_t target, const uint8_t rotation) noexcept
{
	return (target << rotation) | (target >> (32 - rotation));
};

constexpr inline uint32_t Murmur3ShiftXor_32CExpr(const uint32_t hash, const size_t shift) noexcept
{
	return hash ^ (hash >> shift);
};

constexpr inline uint32_t Murmur3Last_32CExpr(const size_t len, const uint32_t hash) noexcept
{
	return Murmur3ShiftXor_32CExpr(0xc2b2ae35 * Murmur3ShiftXor_32CExpr(
	0x85ebca6b * Murmur3ShiftXor_32CExpr(
	static_cast<uint32_t>(hash ^ len), 16), 13), 16);
};

constexpr inline uint32_t Murmur3Tail1_32CExpr(const char data, const size_t len, const uint32_t hash, const uint32_t constant) noexcept
{
	return Murmur3Last_32CExpr(len, hash ^ (0x1b873593 * Murmur3Rotate_32CExpr(
	0xcc9e2d51 * (constant ^ static_cast<uint8_t>(data)), 15)));
};

constexpr inline uint32_t Murmur3Tail2_32CExpr(const char data[2], const size_t len, const uint32_t hash, const uint32_t constant) noexcept
{
	return Murmur3Tail1_32CExpr(data[0], len, hash, constant ^ (static_cast<uint8_t>(data[1]) << 8));
};

constexpr inline uint32_t Murmur3Tail3_32CExpr(const char data[3], const size_t len, const uint32_t hash) noexcept
{
	return Murmur3Tail2_32CExpr(data, len, hash, 0 ^ (static_cast<uint8_t>(data[2]) << 16));
};

constexpr inline uint32_t Murmur3Rest_32CExpr(const char *const data, const size_t len, const uint32_t hash) noexcept
{
	return ((len & 3) == 3) ? Murmur3Tail3_32CExpr(&data[len - 3], len, hash) :
	((len & 3) == 2) ? Murmur3Tail2_32CExpr(&data[len - 2], len, hash, 0) :
	((len & 3) == 1) ? Murmur3Tail1_32CExpr(data[len - 1], len, hash, 0) :
	Murmur3Last_32CExpr(len, hash);
};

constexpr inline uint32_t Murmur3Load_32CExpr(const char *const data, const size_t i) noexcept
{
	return static_cast<uint32_t>(data[(i * sizeof(uint32_t)) + 3]) << 24 |
	static_cast<uint32_t>(data[(i * sizeof(uint32_t)) + 2]) << 16 |
	static_cast<uint32_t>(data[(i * sizeof(uint32_t)) + 1]) << 8 |
	static_cast<uint32_t>(data[(i * sizeof(uint32_t)) + 0]) << 0;
};

constexpr inline uint32_t Murmur3Update_32CExpr(const uint32_t hash, const uint32_t update) noexcept
{
	return 0xe6546b64 + (5 * Murmur3Rotate_32CExpr(hash ^ (
	0x1b873593 * Murmur3Rotate_32CExpr(0xcc9e2d51 * update, 15)), 13));
};

constexpr inline uint32_t Murmur3Loop_32CExpr(const char *const data, const size_t len, const uint32_t hash, const size_t i = 0) noexcept
{
	return (i < (len / 4)) ?
	Murmur3Loop_32CExpr(data, len,
	Murmur3Update_32CExpr(hash, Murmur3Load_32CExpr(data, i)), i + 1) :
	Murmur3Rest_32CExpr(data, len, hash);
};

constexpr inline uint32_t Murmur3_32(const char *const key, const size_t length, const uint32_t seed) noexcept
{
	return Murmur3Loop_32CExpr(key, length, seed);
};

constexpr inline uint32_t MurmurHash3_32(const char *const str, const uint32_t seed = MURMUR_HASH_DEF_SEED) noexcept
{	
	return Murmur3_32(str, StringLengthCExpr(str), seed);
};


#ifdef _MSC_VER
	#pragma warning( pop )
#endif

#endif

#endif