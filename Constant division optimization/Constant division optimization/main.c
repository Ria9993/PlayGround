#include <stdint.h>
#include <stdio.h>

/* 
Reference : Clang x86-64
*/

int main(void)
{
    int n = 125;

    /* reciprocal by 10(divisor)
        but it will floor.
        So store this after make fixed point (<< 32)
    */
    const int64_t reciprocal = ((int64_t)1 << 32) / 10; 

    int64_t tmp = (n * reciprocal) >> 32;

    int32_t ret = (int32_t)tmp;


    printf("%d", ret);

    return 0;
}