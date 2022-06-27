#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <limits.h>

int main() {
    int n;
    scanf("%d", &n);

    int mask = n >> ((CHAR_BIT * sizeof(int)) - 1); // Bit filling by MSB

    // Trans sign only negative number
    n ^= mask;
    n += mask & 1;

    printf("%d", n);
}