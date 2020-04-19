#pragma once

#include <stdint.h>

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) ((Kilobytes(Value))*1024LL)
#define Gigabytes(Value) ((Megabytes(Value))*1024LL)

#define Pi32 3.14159265359f

#define internal static
#define local_persist static
#define global_variable static  

#define array_length(a) (sizeof(a)/sizeof(a[0]))

#if DEBUG_BUILD
#define assert(Expression) if (!(Expression)) {*(int*)0 = 0;}
#else
#define assert(Expression)
#endif

/* signed char, 1 byte, -128 to 127 */
using int8 = int8_t;

/* unsigned char, 1 byte , 0 to 255 */
using uint8 = uint8_t;

/* short, 2 bytes, -32,768 to 32,767 */
using int16 = int16_t;

/* unsigned short, 2 bytes, 0 to 65,535 */
using uint16 = uint16_t;

/* int, 4 bytes, -2,147,483,648 to 2,147,483,647 */
using int32 = int32_t;

/* unsigned int, 4 bytes, 0 to 4,294,967,295 */
using uint32 = uint32_t;

/* long long, 8 bytes, -(2^63) to (2^63)-1 */
using int64 = int64_t;

/* unsigned long long, 8 bytes, 0 to 18,446,744,073,709,551,615 */
using uint64 = uint64_t;

/* unsigned long long, 8bytes */
/* Any valid pointer to void can be converted to this type 
* then converted back to pointer to void, 
* and the result will compare equal to the original pointer */
using uint64ptr = uintptr_t;