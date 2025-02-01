//By AlSch092 @ github
#pragma once
#define OBFUSCATE_SEED 174440041
#define CONST_OPERATION_SEED 76543

#define ROTL(x, n, bits)  (((x) << (n)) | ((x) >> ((bits) - (n))))
#define ROTR(x, n, bits)  (((x) >> (n)) | ((x) << ((bits) - (n))))

#include <random>
#include <string>
#include <iostream>

using namespace std;

template<typename T>
class Obfuscator
{
public:
	static __forceinline void obfuscate(T& data)
	{
		constexpr int bit_width = sizeof(T) * 8;
		constexpr T ROT_KEY = (OBFUSCATE_SEED >> 3) ^ 0xb5;

		data = ROTL(data, 3, bit_width) ^ OBFUSCATE_SEED;
		data = (data + CONST_OPERATION_SEED) ^ ROT_KEY;
	}

	static __forceinline T deobfuscate(const T data)
	{
		constexpr int bit_width = sizeof(T) * 8;
		constexpr T ROT_KEY = (OBFUSCATE_SEED >> 3) ^ 0xb5;

		T deobfs_data = (data ^ ROT_KEY) - CONST_OPERATION_SEED;
		deobfs_data = ROTR(deobfs_data ^ OBFUSCATE_SEED, 3, bit_width);
		return deobfs_data;
	}

	static __forceinline void obfuscate_with_key(T& data, const unsigned int key)
	{
		hash<int> hasher;
		size_t hash_value = hasher(key);
		mt19937 rng(hash_value);

		uniform_int_distribution<T> distribution(1, numeric_limits<T>::max());
		T random_number = distribution(rng);

		constexpr int bit_width = sizeof(T) * 8;
		T ROT_KEY = (OBFUSCATE_SEED >> 3) ^ random_number;

		data = ROTL(data, 3, bit_width) ^ OBFUSCATE_SEED;
		data = (data + CONST_OPERATION_SEED) ^ ROT_KEY;
	}

	static __forceinline T deobfuscate_with_key(const T data, const unsigned int key)
	{
		hash<int> hasher;
		size_t hash_value = hasher(key);
		mt19937 rng(hash_value);

		uniform_int_distribution<T> distribution(1, numeric_limits<T>::max());
		T random_number = distribution(rng);

		constexpr int bit_width = sizeof(T) * 8;
		T ROT_KEY = (OBFUSCATE_SEED >> 3) ^ random_number;

		T deobfs_data = (data ^ ROT_KEY) - CONST_OPERATION_SEED;
		deobfs_data = ROTR(deobfs_data ^ OBFUSCATE_SEED, 3, bit_width);

		return deobfs_data;
	}
};

template<>
class Obfuscator<string> //template specialization for 'string' type since it requires looping over each char in the string
{
public:

	static const uint8_t obfs_seed = 0xFE; //XOR key
	static const uint8_t obfs_additive = 0x53; //extra key which is added or subtracted, to make XOR brute forcing unlikely (since we alternate + or - on each index)

	//this is a 'safer' method which makes sure 00's don't show up early in the string and terminate it wrongly
	static __forceinline void obfuscate(string& data)
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			uint8_t transformed = 0;

			if (i % 2 == 0)  //alternate + or - based on even/odd index to stop simple XOR brute forcers
				transformed = (data[i] ^ Obfuscator<string>::obfs_seed) + Obfuscator<string>::obfs_additive;
			else
				transformed = (data[i] ^ Obfuscator<string>::obfs_seed) - Obfuscator<string>::obfs_additive;

			data[i] = transformed;
		}
	}

	static __forceinline string deobfuscate(const string data)
	{
		string deobfs_str;

		for (size_t i = 0; i < data.size(); i++)
		{
			if (i % 2 == 0) //alternate + or - based on even/odd index to stop simple XOR brute forcers
				deobfs_str += (data[i] - Obfuscator<string>::obfs_additive) ^ Obfuscator<string>::obfs_seed;
			else
				deobfs_str += (data[i] + Obfuscator<string>::obfs_additive) ^ Obfuscator<string>::obfs_seed;

		}

		return deobfs_str;
	}

	static __forceinline void obfuscate_with_key(string& data, const uint8_t key, const uint8_t key_addition)
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			uint8_t transformed = 0;

			if (i % 2 == 0) // //alternate + or - based on even/odd index to stop simple XOR brute forcers
				transformed = (data[i] ^ key) + key_addition;
			else
				transformed = (data[i] ^ key) - key_addition;

			data[i] = transformed;
		}
	}

	static __forceinline string deobfuscate_with_key(string data, const uint8_t key, const uint8_t key_addition)
	{
		string deobfs_str;

		for (size_t i = 0; i < data.size(); i++)
		{
			if (i % 2 == 0) //alternate + or - based on even/odd index to stop simple XOR brute forcers
				deobfs_str += (data[i] - key_addition) ^ key;
			else
				deobfs_str += (data[i] + key_addition) ^ key;
		}

		return deobfs_str;
	}
};
