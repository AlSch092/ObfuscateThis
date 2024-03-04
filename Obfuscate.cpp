//By AlSch092 @ github

#define OBFUSCATE_SEED 174440041
#define CONST_OPERATION_SEED 76543

#include <random>
#include <string>
#include <iostream>

using namespace std;

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
		return (data - CONST_OPERATION_SEED) ^ OBFUSCATE_SEED;
	}

	template <class T>
	__forceinline void obfuscate_with_key(T& data, int key)
	{
		hash<int> hasher;
		size_t hash_value = hasher(key);
		mt19937 rng(hash_value);

		uniform_int_distribution<int> distribution(1, INT_MAX);
		int random_number = distribution(rng);

		data = (data ^ OBFUSCATE_SEED) + random_number;
	}

	template <class T>
	__forceinline T deobfuscate_with_key(T& data, int key)
	{
		hash<int> hasher;
		size_t hash_value = hasher(key);
		mt19937 rng(hash_value);

		uniform_int_distribution<int> distribution(1, INT_MAX);
		int random_number = distribution(rng);

		return (data - random_number) ^ OBFUSCATE_SEED;
	}

	__forceinline void obfuscate_string(char* input, int maxStrLen)
	{
		if (input == NULL) return;

		int len = strnlen_s(input, maxStrLen);

		for (int i = 0; i < len; i++)
		{
			if (i % 2 == 0) //destroys chances of someone brute forcing XOR key - alternating digits having a different operation
				input[i] = (input[i] ^ OBFUSCATE_SEED) + CONST_OPERATION_SEED;
			else
				input[i] = (input[i] ^ OBFUSCATE_SEED) - CONST_OPERATION_SEED;
		}
	}

	__forceinline string get_deobfuscated_string(char* input, int maxStrLen)
	{
		if (input == NULL) return (string)NULL;

		int len = strnlen_s(input, maxStrLen);

		string deobfs;

		for (int i = 0; i < len; i++)
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
	
	ProtectedData(T val)
	{
		SetData(val);
	}

	__forceinline T GetData()
	{ 
		return Obfuscator::deobfuscate(someData);
	}
	
	__forceinline void SetData(T value)
	{ 
		this->someData = value;  
		Obfuscator::obfuscate(this->someData);
	}

	__forceinline T GetData(int key)
	{
		return Obfuscator::deobfuscate_with_key(someData, key);
	}

	__forceinline void SetData(T value, int key)
	{
		this->someData = value;  
		Obfuscator::obfuscate_with_key(this->someData, key);
	}
};

int main(void)
{
	int OriginalValue = 5;
	int key = 12345;

	//this example presents two different obfuscation tactics: regular XOR + static operation, and key-based. more will be added over time
	ProtectedData<int>* obfuscatedClass = new ProtectedData<int>(OriginalValue);
	int value = obfuscatedClass->GetData(); //returns original value of 5, now we can try adding a randomized key
	cout << "Original value to obfuscate: " << value << endl;

	obfuscatedClass->SetData(OriginalValue, key); //key-based approach - a deterministic value is generated based on a key and used in the obfuscation process
	
	int wrong_value = obfuscatedClass->GetData(2222); //wrong key (2222) gets a wrong value back
	
	cout << "GetData with key 2222 -> Value = " << wrong_value << endl;

	int correct_value_with_key = obfuscatedClass->GetData(key); //get data back with correct key

	cout << "GetData with key " << key << " -> Value = " << correct_value_with_key << endl;

	char* str_to_obfuscate = new char[20] {0};
	strcpy_s(str_to_obfuscate, 15, "Secret Message"); //this hardcoded string parameter will leave traces of the secret string, please see my project "HideStaticReferences" on how to remove string traces

	Obfuscator::obfuscate_string(str_to_obfuscate, 50);

	cout << "Obfuscated string: " << str_to_obfuscate << endl;

	string deobfs_str = Obfuscator::get_deobfuscated_string(str_to_obfuscate, 50);

	cout << "Deobfuscated string: " << deobfs_str << endl;

	delete obfuscatedClass;
	delete[] str_to_obfuscate;

	return 0;
}
