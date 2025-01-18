//By AlSch092 @ github

#define OBFUSCATE_SEED 174440041
#define CONST_OPERATION_SEED 76543

#include <random>
#include <string>
#include <iostream>

using namespace std;

template<typename T>
class Obfuscator
{
public:
	static void obfuscate(T& data) //static cannot be inline
	{
		data = (data ^ OBFUSCATE_SEED) + CONST_OPERATION_SEED;
	}

	static T deobfuscate(T& data)
	{
		return (data - CONST_OPERATION_SEED) ^ OBFUSCATE_SEED;
	}

	static void obfuscate_with_key(T& data, int key)
	{
		hash<int> hasher;
		size_t hash_value = hasher(key);
		mt19937 rng(hash_value);

		uniform_int_distribution<int> distribution(1, INT_MAX);
		int random_number = distribution(rng);

		data = (data ^ OBFUSCATE_SEED) + random_number;
	}

	static T deobfuscate_with_key(T& data, int key)
	{
		hash<int> hasher;
		size_t hash_value = hasher(key);
		mt19937 rng(hash_value);

		uniform_int_distribution<int> distribution(1, INT_MAX);
		int random_number = distribution(rng);

		return (data - random_number) ^ OBFUSCATE_SEED;
	}
};

template<>
class Obfuscator<string> //template specialization for 'string' type since it requires looping over each char in the string
{
public:

	static const uint8_t obfs_seed = 0xFE; //XOR key
	static const uint8_t obfs_additive = 0x53; //extra key which is added or subtracted, to make XOR brute forcing unlikely
	static const uint8_t reserved_value = 0xFF; //reserved value to encode adjusted `0x00`, since we may run into 00's early in the string which terminate it early

	//this is a 'safer' method which makes sure 00's don't show up early in the string and terminate it wrongly
	static void obfuscate(string& data)
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			uint8_t transformed = (data[i] ^ Obfuscator<string>::obfs_seed) + Obfuscator<string>::obfs_additive;

			if (transformed == 0x00 || transformed == reserved_value) //encode original value explicitly
			{	
				data[i] = reserved_value;  //use reserved value to indicate special handling
				data.insert(i + 1, 1, data[i] ^ 0xAA); //encode original value with a secondary transform
				++i; //skip the inserted value
			}
			else
			{
				data[i] = transformed;
			}
		}
	}

	static string deobfuscate(const string& data)
	{
		string deobfs_str;

		for (size_t i = 0; i < data.size(); i++)
		{
			if (data[i] == reserved_value)
			{			
				uint8_t original = data[i + 1] ^ 0xAA; //decode explicitly encoded values
				deobfs_str += original;
				++i; //skip the next byte since it's part of the encoding
			}
			else
			{		
				deobfs_str += (data[i] - Obfuscator<string>::obfs_additive) ^ Obfuscator<string>::obfs_seed; //Normal decoding
			}
		}

		return deobfs_str;
	}

	static void obfuscate_with_key(string& data, uint8_t key, uint8_t key_addition)
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			uint8_t transformed = (data[i] ^ key) + key_addition;

			if (transformed == 0x00 || transformed == reserved_value) //encode original value explicitly
			{
				data[i] = reserved_value;  //use reserved value to indicate special handling
				data.insert(i + 1, 1, data[i] ^ 0xAA); //encode original value with a secondary transform
				++i; //skip the inserted value
			}
			else
			{
				data[i] = transformed;
			}
		}
	}

	static string deobfuscate_with_key(string& data, uint8_t key, uint8_t key_addition)
	{
		string deobfs_str;

		for (size_t i = 0; i < data.size(); i++)
		{
			if (data[i] == reserved_value)
			{
				uint8_t original = data[i + 1] ^ 0xAA; //decode explicitly encoded values
				deobfs_str += original;
				++i; //skip the next byte since it's part of the encoding
			}
			else
			{
				deobfs_str += (data[i] - key_addition) ^ key; //Normal decoding
			}
		}

		return deobfs_str;
	}
};

template<class T>
class ProtectedData  //simple wrapped around obfuscate and deobfuscate, this class is completely optional to use
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
		return Obfuscator<T>::deobfuscate(someData);
	}
	
	__forceinline void SetData(T value)
	{ 
		this->someData = value;  
		Obfuscator<T>::obfuscate(this->someData);
	}

	__forceinline T GetData(int key)
	{
		return Obfuscator<T>::deobfuscate_with_key(someData, key);
	}

	__forceinline void SetData(T value, int key)
	{
		this->someData = value;  
		Obfuscator<T>::obfuscate_with_key(this->someData, key);
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
	
	cout << "GetData with key 2222 (wrong key) -> Value = " << wrong_value << endl;

	int correct_value_with_key = obfuscatedClass->GetData(key); //get data back with correct key

	cout << "GetData with key " << key << " -> Value = " << correct_value_with_key << endl;

	string str_to_obfs = "hello world";

	Obfuscator<string>::obfuscate(str_to_obfs);

	cout << "Obfuscated string: " << str_to_obfs << endl;

	string deobfs_str = Obfuscator<string>::deobfuscate(str_to_obfs);

	cout << "Deobfuscated string: " << deobfs_str << endl;

	string str_to_obfs_with_key = "a string large enough to show that no strange edge cases will come up, in particular we want to see if any 0x00 comes up which might falsely terminate the string at an earlier time.";

	Obfuscator<string>::obfuscate_with_key(str_to_obfs_with_key, 0x12, 0x34);

	cout << "Obfuscated string (using keys 0x12, 0x34): " << str_to_obfs_with_key << endl;

	string deobfs_str_with_key = Obfuscator<string>::deobfuscate_with_key(str_to_obfs_with_key, 0x12, 0x34);

	cout << "Deobfuscated string (using keys 0x12, 0x34): " << deobfs_str_with_key << endl;

	delete obfuscatedClass;

	return 0;
}
