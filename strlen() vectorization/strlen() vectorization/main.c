#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

/*
Reference : glibc/strlen.c
*/

/* Check 32bit at a time 
    (Can Process Extented-ASCII) */
int my_strlen(const char* const str)
{
    static_assert(CHAR_BIT == 8, "char is not 8-bit");

    // Continue untill char_p is alligned as 32bit boundary
    const char* char_p = str;
    for (; ((uintptr_t)char_p & (sizeof(int32_t) - 1)) != 0; ++char_p)
    {
        if (*char_p == '\0')
        {
            return str - char_p;
        }
    }

    /*  Magic bits
        11111110 11111110 11111110 11111111
        1-bit : Propagate Carry 1-bit to 0-bit hall
        0-bit : Stop propagation of carry to next byte */
    const int32_t magic = ~0x01010100;
    /*  Checker bits
        10000000 10000000 10000000 10000000
        If string is not extended ascii, String will not have last-bit of byte.
        So if character is not 0, 1-bit can't placed in Checker's 1-bit position.(unsigned) */
    const int32_t checker = 0x80808080;

    const int32_t* block_p = (const int32_t*)char_p;
    for (; ; ++block_p)
    {
        const int32_t block = *block_p;
        int32_t ret = block + magic;
        /*  If string is extended ascii, 1-bit can placed in Checker's 1-bit position.
            So to solve this ploblem, remove 1-bits was in block's 1-bit place */
        ret &= ~block;
        if ((ret & checker) != 0) // block include '\0' character (maybe(?))
        {
            const char* ch = (const char*)block_p;
            if (ch[0] == '\0')
                return ch - str;
            if (ch[1] == '\0')
                return ch + 1 - str;
            if (ch[2] == '\0')
                return ch + 2 - str;
            if (ch[3] == '\0')
                return ch + 3 - str;
        }
    }
    assert(0);
}

int main()
{
    char str[] = "HELLO WORLD";

    printf("%d", my_strlen(str));

    return 0;
}