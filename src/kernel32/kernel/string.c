#include    "../include/string.h"

char * strcpy ( char * destination, const char * source )
{
    int i = 0;
    while (*(source + i) != '\x00')
    {
        *(destination + i) = *(source + i);
        i++;
    }
    *(destination + i) = *(source + i);
    return destination;
}

char * strncpy ( char * destination, const char * source, size_t num )
{
    memset((void*)destination, 0, num);
    for (int i=0; i<num; i++)
    {
        *(destination + i) = *(source + i);
        if (*(source + i) == '\x00') break;
    }
    
    return destination;
}

char * strcat ( char * destination, const char * source )
{
    int i=0;
    while (*(destination + i) != '\x00') i++;
    strcpy((destination + i), source);
    return destination;
}

char * strncat ( char * destination, const char * source, size_t num )
{
    int i=0;
    while (*(destination + i) != '\x00') i++;
    
    int j;
    for (j=0; j<num; j++)
    {
        if (*(source + j) == '\x00') break;
        else *(destination + i + j) = *(source + j);
    }
    
    *(destination + j) = '\x00';
    return destination;
}

int strcmp ( const char * str1, const char * str2 )
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 > len2)
    {
        return memcmp((const void*)str1, (const void*)str2, len2);
    }
    else
    {
        return memcmp((const void*)str1, (const void*)str2, len1);
    }
}

int strncmp ( const char * str1, const char * str2, size_t num )
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 >= num && len2 >= num) return memcmp((const void*)str1, (const void*)str2, num);
    else return strcmp (str1, str2);
}

char * strchr ( const char * str, int character )
{
    return memchr((void*)str, character, strlen(str));
}

size_t strcspn ( const char * str1, const char * str2 )
{
    int j = 0;
    for (int i=0; i<strlen(str2); i++)
    {
        for (j=0; j<strlen(str1); j++)
        {
            if (*(str2+i) == *(str1+j))
            {
                return j;
            }
        }
    }
    return j;
}

char * strpbrk ( const char * str1, const char * str2 )
{
    int indx = strcspn(str1, str2);
    if (indx == strlen(str1)) return (char*)0;
    else return str1 + indx;
}

char * strrchr ( const char * str, int character )
{
    char* p = strchr(str, character);
    char* save_p = p;
    while (*p != '\x00')
    {
        save_p = p;
        p = strchr(str, character);
    }
    return save_p;
}

size_t strspn ( const char * str1, const char * str2 )
{
    int i, j;

    for (i = 0; str1[i]; i++) 
    {
        for (j = 0; str2[j]; j++) 
        {
            if (str2[j] == str1[i]) break;
        }
        if (!str2[j]) break;
    }
    return i;
}

char * strstr(const char *str1, const char *str2)
{
    const char *s, *t;

    if (!str1 || !str2) return (0);

    if (!*str2) return ((char *)str1);
    
    for (; *str1; str1++) 
    {
        if (*str1 == *str2) 
        {
            t = str1;
            s = str2;
            for (; *t; s++, t++) 
            {
                if (*t != *s) break;
            }
            if (!*s) return ((char *)str1);
        }
    }
    return 0;
}

char * strtok(char *str, const char *delimiters)
{
    static char    *t;
    char *r;
    int n;

    if (str) t = str;

    r = t + strspn (t, delimiters);
    if (!(n = strcspn (r, delimiters))) return 0;
    t = r + n;

    if (*t) *t++ = 0;
    return r;
}

char strerror_s = ' ';
char * strerror ( int errnum )
{
    return &strerror_s;
}

size_t strlen ( const char * str )
{
    size_t i=0;
    while (*(str + i) != '\x00') i++;
    return i;
}
