#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

/* 
Reference : À¯¿µÃµ(Megayuchi)
https://megayuchi.com/2015/09/10/xor%EC%9D%84-%EC%9D%B4%EC%9A%A9%ED%95%98%EC%97%AC-%EB%B6%84%EA%B8%B0-%EC%97%86%EC%9D%B4-%EB%91%90-%EB%B3%80%EC%88%98%EC%9D%98-%EA%B0%92-%EA%B5%90%ED%99%98%ED%95%98%EA%B8%B0/)
*/

int main()
{
    int a = 5;
    int b = 10;

    /* Original C Code
    if (a < b)
    {
        //swap
        int tmp = a;
        a = b;
        b = tmp;
    } */
    
    // ASM Code without Branch
    __asm
    {
        mov eax, [a]
        mov ebx, [b]

        ; carry mask
        xor ecx, ecx; ecx = 0
        cmp eax, ebx
        sbb ecx, 0; IF a < b, THEN 0xFFFFFFFF, ELSE 0

        ;xor mask
        mov edx, eax;
        xor edx, ebx; a^ b
            and edx, ecx; IF a < b, THEN 0, ELSE a^ b

            ; change
            xor eax, edx
            xor ebx, edx

            mov[a], eax
            mov[b], ebx
    }

    printf("a = %d, b = %d", a, b);

    return 0;
}