#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <limits.h>

int main() {
    int n_ori;
    scanf("%d", &n_ori);

    /* VER1 */
    {
        int n = n_ori;

        int mask = n >> ((CHAR_BIT * sizeof(int)) - 1); // Bit filling by MSB

        // Trans sign only negative number
        n ^= mask;
        n -= mask;

        printf("VER1:%d\n", n);
    }

    /* VER2 
        ref:MSVC */
    {
        int n = n_ori;
        
        __asm
        {
            mov eax, [n]
            cdq //extend eax to edx(make bitmask by signbit)
            xor eax, edx
            sub eax, edx
            mov [n], eax
        }

        printf("VER2:%d\n", n);
    }
    return 0;
}
