# ObfuscateThis
Templated Obfuscation example in C++ for protecting/hiding values in memory, provides a class `Obfuscator` which contains static routines for obfuscating + deobfuscatiing basic types, along with a class template specialization for `string` type which deals with string encryption specifically.

## What is this?  
A standalone class for runtime obfuscation written in C++ which supports all basic data types and `std::string`. A simple wrapper is included in the 'tests' file (Obfuscate.cpp), and demonstrates usage of the class. String obfuscation loops over each letter of an input string and performs a XOR and then adds/subtracts another value based on the element's index being even or odd. Additional measures have been made to ensure that XOR brute forcing cannot occur, by using bit shifting. 

## Memory Implications  
All routines are set as `__forceinline` such that a single location in memory cannot be hooked to reveal secrets. During deobfuscation, the obfuscated value is not revealed in memory, as only a deobfuscated copy is returned from the function.

Thank you for reading and taking an interest, happy coding.
