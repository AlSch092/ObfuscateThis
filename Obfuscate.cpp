//By AlSch092 @ github

#define OBFUSCATE_SEED 174440041
#define CONST_OPERATION_SEED 76543

#include <Windows.h>
#include <stdio.h>
#include <random>

namespace Obfuscator
{
	template <class T>
	__forceinline void obfuscate(T& data)
	{
		data = (data ^ OBFUSCATE_SEED) + CONST_OPERATION_SEED;
	}

	template <class T>
	__forceinline T deobfuscate(T& data)
	{
		T deobfuscated_val = (data - CONST_OPERATION_SEED) ^ OBFUSCATE_SEED;
		return deobfuscated_val;
	}

	template <class T>
	__forceinline void obfuscate_with_key(T& data, int key)
	{
		std::hash<int> hasher;
		size_t hash_value = hasher(key);
		std::mt19937 rng(hash_value);

		std::uniform_int_distribution<int> distribution(1, 2147483647);
		int random_number = distribution(rng);

		data = (data ^ OBFUSCATE_SEED) + random_number;
	}

	template <class T>
	__forceinline T deobfuscate_with_key(T& data, int key)
	{
		std::hash<int> hasher;
		size_t hash_value = hasher(key);
		std::mt19937 rng(hash_value);

		std::uniform_int_distribution<int> distribution(1, 2147483647);
		int random_number = distribution(rng);

		T deobfuscated_val = (data - random_number) ^ OBFUSCATE_SEED;
		return deobfuscated_val;
	}

	__forceinline void obfuscate_string(std::string& input)
	{
		for (int i = 0; i < input.length(); i++)
		{
			if (i % 2 == 0) //destroys chances of someone brute forcing XOR key - alternating digits having a different constant operation
				input[i] = (input[i] ^ OBFUSCATE_SEED) + CONST_OPERATION_SEED;
			else
				input[i] = (input[i] ^ OBFUSCATE_SEED) - CONST_OPERATION_SEED;
		}
	}

	__forceinline std::string get_deobfuscated_string(std::string& input)
	{
		std::string deobfs;

		for (int i = 0; i < input.length(); i++)
		{
			if (i % 2 == 0)
			{
				char deobfs_ch = (input[i] - CONST_OPERATION_SEED) ^ OBFUSCATE_SEED;
				deobfs.push_back(deobfs_ch);
			}
			else
			{
				char deobfs_ch = (input[i] + CONST_OPERATION_SEED) ^ OBFUSCATE_SEED;
				deobfs.push_back(deobfs_ch);
			}
		}

		return deobfs;
	}
}

template<class T>
class ProtectedData
{
private:
	T someData; //data member to protect in memory

public:
	
	__forceinline T GetData()
	{ 
		T data_deobfs = Obfuscator::deobfuscate(someData);
		return data_deobfs; 
	}
	
	__forceinline void SetData(T value)
	{ 
		this->someData = value;  
		Obfuscator::obfuscate(this->someData);
	}

	__forceinline T GetData(int key)
	{
		T data_deobfs = Obfuscator::deobfuscate_with_key(someData, key);
		return data_deobfs;
	}

	__forceinline void SetData(T value, int key)
	{
		this->someData = value;  
		Obfuscator::obfuscate_with_key(this->someData, key);
	}
};

int main(void)
{
	//this example presents two different obfuscation tactics: regular XOR + static operation, and key-based. more will be added soon!
	ProtectedData<int>* obfuscatedClass = new ProtectedData<int>();
	
	//at the address of someData in ProtectedData class, the value at rest will never be what we set here for more than a few assembler instructions
	obfuscatedClass->SetData(5); 	
	
	int value = obfuscatedClass->GetData(); //not using key

	printf("Value: %d\n", value);

	obfuscatedClass->SetData(5, 12345); //key-based approach - a deterministic value is generated based on a key and used in the obfuscation process
	
	int wrong_value = obfuscatedClass->GetData(2222); //
	
	printf("Value: %d - using the wrong key will result in a value different than the original\n", wrong_value);

	int correct_value_with_key = obfuscatedClass->GetData(12345);

	printf("Value: %d\n", correct_value_with_key);

	std::string str_to_obfuscate = "Secret Message"; //string obfuscation example

	Obfuscator::obfuscate_string(str_to_obfuscate);

	printf("Obfuscated string (1): %s\n", str_to_obfuscate.c_str());

	printf("Deobfuscated string (1): %s\n", Obfuscator::get_deobfuscated_string(str_to_obfuscate).c_str());
	
	delete obfuscatedClass;
	return 0;
}
