#pragma once

#if __cplusplus
extern "C" {
#endif

typedef long long i64;
typedef int		  i32;
typedef short	  i16;
typedef char	  i8;

typedef unsigned long long u64;
typedef unsigned int	   u32;
typedef unsigned short	   u16;
typedef unsigned char	   u8;

typedef float  f32;
typedef double f64;

#define SI_NULL 0

typedef u8 b8;

#define SI_TRUE	 ((b8)1)
#define SI_FALSE ((b8)0)

// Exit codes
#define SI_EXIT_SUCCESS 0
#define SI_EXIT_FAILURE 1

#if __cplusplus
}
#endif