#include <stdint.h>
#include <stdio.h>
#include <limits.h>

int main()
{
    int32_t dividend = INT_MAX; // INT_MAX = 2'147'483'647
    const int32_t divisor = 10;
    int64_t tmp;
    int32_t ret;

    const int64_t approximator = ((int64_t)1 << 32); // extend int32_t to int64_t
    const int64_t CCT_magic = approximator / divisor;

    tmp = (int64_t)dividend;

    tmp *= CCT_magic;

    ret = tmp >> 32; // return to int32_t. And Ceil

    printf("ret : %d\nanswer : %d\n", ret, INT_MAX / divisor); // 214'748'364

    int32_t modulo = dividend - (divisor * ret);
    printf("modulo = %d", modulo); // 7

    return 0;
}