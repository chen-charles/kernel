#ifndef __CSTRING__
#define __CSTRING__

#include "global.h"
#include "type.h"

EXTERN_C

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef size_t
#define size_t uintptr_t
#endif

//Copying
void * memcpy ( void * destination, const void * source, size_t num );
void * memmove ( void * destination, const void * source, size_t num );
char * strcpy ( char * destination, const char * source );
char * strncpy ( char * destination, const char * source, size_t num );

//Concatenation
char * strcat ( char * destination, const char * source );
char * strncat ( char * destination, const char * source, size_t num );

//Comparison
int memcmp ( const void * ptr1, const void * ptr2, size_t num );
int strcmp ( const char * str1, const char * str2 );
//int strcoll ( const char * str1, const char * str2 );
int strncmp ( const char * str1, const char * str2, size_t num );
//size_t strxfrm ( char * destination, const char * source, size_t num );

//Searching
void * memchr ( void * ptr, int value, size_t num ); //Locate character in block of memory
char * strchr ( const char * str, int character );
size_t strcspn ( const char * str1, const char * str2 );
char * strpbrk ( const char * str1, const char * str2 );
char * strrchr ( const char * str, int character );
size_t strspn ( const char * str1, const char * str2 );
char * strstr(const char *str1, const char *str2);
char * strtok(char *str, const char *delimiters);


void * memset ( void * ptr, int value, size_t num );
char * strerror ( int errnum );
size_t strlen ( const char * str );


EXTERN_C_END

#endif
