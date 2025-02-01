//By AlSch092 @ github
#include "Obfuscator.hpp"

template<class T>
class ProtectedData  //simple wrapper around obfuscate and deobfuscate, this class is completely optional to use. the actual obfuscator is in Obfuscator.hpp
{
private:
	T someData; //data member to protect in memory

public:

	ProtectedData(const T val)
	{
		SetData(val);
	}

	__forceinline T GetData()
	{
		return Obfuscator<T>::deobfuscate(someData);
	}

	__forceinline void SetData(const T value)
	{
		this->someData = value;
		Obfuscator<T>::obfuscate(this->someData);
	}

	__forceinline T GetData(const unsigned int key)
	{
		return Obfuscator<T>::deobfuscate_with_key(someData, key);
	}

	__forceinline void SetData(const T value, const unsigned int key)
	{
		this->someData = value;
		Obfuscator<T>::obfuscate_with_key(this->someData, key);
	}
};

int main(void)
{
	//test integer obfuscations
	unsigned int OriginalValue = 5;
	const unsigned int key = 12345;

	ProtectedData<unsigned int>* obfuscatedClass = new ProtectedData<unsigned int>(OriginalValue);

	unsigned int value = obfuscatedClass->GetData(); //returns original value of 5, now we can try adding a randomized key

	cout << "Original value to obfuscate: " << value << endl;

	obfuscatedClass->SetData(OriginalValue, key); //key-based approach - a deterministic value is generated based on a key and used in the obfuscation process

	int wrong_value = obfuscatedClass->GetData(2222); //wrong key (2222) gets a wrong value back

	cout << "GetData with key 2222 (wrong key) -> Value = " << wrong_value << endl;

	int correct_value_with_key = obfuscatedClass->GetData(key); //get data back with correct key

	cout << "GetData with key " << key << " -> Value = " << correct_value_with_key << endl;

	//test string obfuscations
	string str_to_obfs = "hello world";

	Obfuscator<string>::obfuscate(str_to_obfs);

	cout << "Obfuscated string: " << str_to_obfs << endl;

	string deobfs_str = Obfuscator<string>::deobfuscate(str_to_obfs);

	cout << "Deobfuscated string: " << deobfs_str << endl;

	string str_to_obfs_with_key = "a string large enough to show that no strange edge cases will come up, a quick brown fox jumps over !@#$%^&*() the 1234567890 gate";

	Obfuscator<string>::obfuscate_with_key(str_to_obfs_with_key, 0x12, 0x34);

	cout << "Obfuscated string (using keys 0x12, 0x34): " << str_to_obfs_with_key << endl;

	string deobfs_str_with_key = Obfuscator<string>::deobfuscate_with_key(str_to_obfs_with_key, 0x12, 0x34);

	cout << "Deobfuscated string (using keys 0x12, 0x34): " << deobfs_str_with_key << endl;

	delete obfuscatedClass;

	return 0;
}