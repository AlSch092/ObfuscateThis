# ObfuscateThis
Templated Obfuscation example in C++ for protecting/hiding values in memory, provides a class `Obfuscator` which contains static routines for obfuscating + deobfuscatiing basic types, along with a class template specialization for `string` type which deals with string encryption specifically.

## What is this?  
A single file solution written in C++ for obfuscating both templated primitive types and strings. Currently two different methods are displayed in Obfuscate.cpp: regular arithmetic and 'key-based'. Arithmetic obfuscation uses a simple XOR operation along with an addition or subtraction on every even/odd byte (to prevent XOR brute forcing). Key-based uses a similar approach, except the value used to add and subtract after XORing is generated by seeding an RNG (mt19937). Using the key-based approach, values can only be properly deobfuscated if the correct key is given, and different variables can use different keys. String obfuscation loops over each letter in an input string and performs the same operation as mentioned above - XOR and then add or subtract. In our example, the class ProtectedData contains a templated type variable that we would like to obfuscate.

## Memory Implications  
All routines are set as `__forceinline` such that a single routine cannot be hooked and reversed to reveal secrets, and only copies of our protected data are returned from deobfuscation functions. This ensures our protected data stays obfuscated within memory, and there are only a couple of assembler instructions where our protected data's memory address contains the origial value. 

Thank you for reading and taking an interest, happy coding.




