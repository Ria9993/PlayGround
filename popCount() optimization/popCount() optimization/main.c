#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

int popcount(unsigned int n)
{
    int ret = 0;

    // 32bit
    const uint32_t mask1 = 0x55555555; // 01010101010101010101010101010101
    const uint32_t mask2 = 0x33333333; // 00110011001100110011001100110011
    const uint32_t mask3 = 0x0F0F0F0F; // 00001111000011110000111100001111
    const uint32_t mask4 = 0x00FF00FF; // 00000000111111110000000011111111
    const uint32_t mask5 = 0x0000FFFF; // 00000000000000001111111111111111

    ret = n;
    ret = (ret & mask1) + ((ret >> 1) & mask1);
    ret = (ret & mask2) + ((ret >> 2) & mask2);
    ret = (ret & mask3) + ((ret >> 4) & mask3);
    ret = (ret & mask4) + ((ret >> 8) & mask4);
    ret = (ret & mask5) + ((ret >> 16) & mask5);

    return ret;
}

int main()
{
    printf("%d", popcount(783272)); // 12

    return 0;
}