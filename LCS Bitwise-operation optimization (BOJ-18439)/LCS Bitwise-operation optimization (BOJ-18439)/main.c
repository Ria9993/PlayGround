// https://koosaga.com/245
// http://www.secmem.org/blog/2019/09/12/lcs-with-bitset/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum { STR_MAX = 50000 };
char A[STR_MAX + 1];
char B[STR_MAX + 1];
uint64_t E['Z' + 1][(STR_MAX / 64) + 1] = { 0, };
uint64_t DP[(STR_MAX / 64) + 1] = { 0, };
int main()
{
	scanf("%s", A);
	scanf("%s", B);
	int A_size = strlen(A);
	int B_size = strlen(B);
	int block_size = (B_size / 64) + 1;

	// E set
	for (int i = 0; i < B_size; i++)
	{
		E[B[i]][i / 64] |= (uint64_t)1 << (i % 64);
	}

	// DP[0] set
	for (int i = 0; i < B_size; i++)
	{
		if (A[0] == B[i])
		{
			DP[i / 64] |= (uint64_t)1 << (i % 64);
			break;
		}
	}

	for (int A_i = 1; A_i < A_size; A_i++)
	{
		uint64_t shift_carry = 1;
		uint64_t sub_carry = 0;
		for (int block_i = 0; block_i < block_size; block_i++)
		{
			// x = (DP[A_i - 1] | A == B{E})
			uint64_t x = DP[block_i] | E[A[A_i]][block_i];

			// DP[A_i - 1] << 1
			uint64_t lowbit_magic = (DP[block_i] << 1) | shift_carry;
			shift_carry = DP[block_i] >> 63;

			// Apply last sub_carry
			uint64_t tmp = x;
			uint64_t sub_carry_tmp = sub_carry;
			sub_carry = (uint64_t)tmp < sub_carry ? 1 : 0;
			tmp -= sub_carry_tmp;

			// tmp = (DP[A_i - 1] | A == B{E}) - (DP[A_i - 1] << 1)
			sub_carry += (uint64_t)tmp < lowbit_magic ? 1 : 0;
			tmp -= lowbit_magic;

			DP[block_i] = x & (x ^ tmp);
		}
	}

	int ret = 0;
	for (int i = 0; i < B_size; i++)
	{
		ret += (DP[i / 64] >> (i % 64)) & 1;
	}

	printf("%d", ret);

	return 0;
}
