//By AlSch092 @ github

#define OBFUSCATE_SEED 174440041
#define CONST_OPERATION_SEED 76543

#include <Windows.h>
#include <stdio.h>
#include <random>

namespace Obfuscator
{
	template <class T>
	void obfuscate(T& data)
	{
		data = (data ^ OBFUSCATE_SEED) + CONST_OPERATION_SEED;
	}

	template <class T>
	T deobfuscate(T& data)
	{
		T deobfuscated_val = (data - CONST_OPERATION_SEED) ^ OBFUSCATE_SEED;
		return deobfuscated_val;
	}

	template <class T>
	void obfuscate_with_key(T& data, int key)
	{
		std::hash<int> hasher;
		size_t hash_value = hasher(key);
		std::mt19937 rng(hash_value);

		std::uniform_int_distribution<int> distribution(1, 2147483647);
		int random_number = distribution(rng);

		data = (data ^ OBFUSCATE_SEED) + random_number;
	}

	template <class T>
	T deobfuscate_with_key(T& data, int key)
	{
		std::hash<int> hasher;
		size_t hash_value = hasher(key);
		std::mt19937 rng(hash_value);

		std::uniform_int_distribution<int> distribution(1, 2147483647);
		int random_number = distribution(rng);

		T deobfuscated_val = (data - random_number) ^ OBFUSCATE_SEED;
		return deobfuscated_val;
	}
}

template<class T>
class ProtectedData
{
private:
	T someData; //data member to protect in memory

public:
	
	T GetData()
	{ 
		T data_deobfs = Obfuscator::deobfuscate(someData);
		return data_deobfs; 
	}
	
	void SetData(T value) 
	{ 
		this->someData = value;  
		Obfuscator::obfuscate(this->someData);
	}

	T GetData(int key)
	{
		T data_deobfs = Obfuscator::deobfuscate_with_key(someData, key);
		return data_deobfs;
	}

	void SetData(T value, int key)
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

	return 0;
}