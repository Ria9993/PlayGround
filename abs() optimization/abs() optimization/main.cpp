#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int mask = n >> 31; // Bit filling by MSB

    // Trans sign only negative number
    n ^= mask;
    n += mask & 1;

    printf("%d", n);
}