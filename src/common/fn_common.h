#pragma once

#include <stdint.h>

#define kilobytes(Value) ((Value)*1024LL)
#define megabytes(Value) ((kilobytes(Value))*1024LL)
#define gigabytes(Value) ((megabytes(Value))*1024LL)

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
typedef int8_t int8;

/* unsigned char, 1 byte , 0 to 255 */
typedef uint8_t uint8;

/* short, 2 bytes, -32,768 to 32,767 */
typedef int16_t int16;

/* unsigned short, 2 bytes, 0 to 65,535 */
typedef uint16_t uint16;

/* int, 4 bytes, -2,147,483,648 to 2,147,483,647 */
typedef int32_t int32;

/* unsigned int, 4 bytes, 0 to 4,294,967,295 */
typedef uint32_t uint32;

/* long long, 8 bytes, -(2^63) to (2^63)-1 */
typedef int64_t int64;

/* unsigned long long, 8 bytes, 0 to 18,446,744,073,709,551,615 */
typedef uint64_t uint64;

/* unsigned long long, 8bytes */
/* Any valid pointer to void can be converted to this type 
* then converted back to pointer to void, 
* and the result will compare equal to the original pointer */
typedef uintptr_t uint64ptr;