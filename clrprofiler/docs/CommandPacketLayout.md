Packet Structure
================
The layout for a command packet is pretty simple. In order to ensure we have all of the data we will implement a double count schema. This is redundant but useful so we can accurately terminate our bulk packages. This also gives us the ability to send compressed data without altering the surrounding packets.

## A few rules
* All non packet length and non command integers will be 64bit
 - If an integer comes across as 32bit it will be **0 PADDED**.
 - If an integer is 128bit (yeesh) it will be sent as 128bit.
* Same goes for decimals, reals, floats, etc.
* Strings can be ANSI, ASCII, Unicode, UTF-8, UTF-16, UTF-32, etc.
 - This is to support multiple languages and not limit the application
* Strings will be FNV-1a hashed for storage and retrieval
 - Use std::hash<T> in C++ or use the helper class in the .NET libraries
* Exceptions to the string rule will be any class, function, or assembly name
 - These will use the MDTOKEN so they can stay unique during transfer
 - Only later will these be stored as hashed values
* Entire packets will be terminated with 0xCCCCCCCC
* Individual commands will be terminated with 0x0000
* For fields that are variable use bitflags
 - For instance if you have a max of 16 options use a 16 bit integer to check
* Packet lengths will include terminator
* Command lengths will include terminator


## Entire Packet

```
--------------------------------------------------------------------------------
  Packet Length 4 bytes   |   ... multiple commands ...     |    0xCCCCCCCC
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
  Command Length 4 bytes  |      ... Command Data ...       |    0x0000
--------------------------------------------------------------------------------
```
