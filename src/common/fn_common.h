#pragma once

#include <stdint.h>

// Compilers
#if !defined(COMPILER_MSVC)
#if _MSC_VER
#define COMPILER_MSVC 1
#else
#define COMPILER_MSVC 0
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

// used for functions that should only be accesed within the same translation unit
#define internal static
// used for local variables in functions that should be treated as globals
#define local_persist static
// global variables
#define global static  

#define array_length(a) (sizeof(a)/sizeof(a[0]))

#define kilobytes(number) ((number) * 1024ull)
#define megabytes(number) (kilobytes(number) * 1024ull)
#define gigabytes(number) (megabytes(number) * 1024ull)
#define terabytes(number) (gigabytes(number) * 1024ull)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

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

typedef float f32;
typedef double f64;

/* unsigned long long, 8bytes */
/* Any valid pointer to void can be converted to this type 
* then converted back to pointer to void, 
* and the result will compare equal to the original pointer */
typedef uintptr_t uint64ptr;

template<class T>
internal inline void fn_swap(T* a, T* b)
{
    T tmp = *a;
    *a = *b;
    *b = tmp;
}