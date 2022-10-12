#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <emmintrin.h> // SSE2

/* Find First Set
	Return index of first set
	IF n == 0, Undefined behavior (return (32)) */
uint32_t ffs(uint32_t n)
{
	uint32_t ret = 0;

	if ((n & 0x0000FFFF) == 0)
	{
		n >>= 16;
		ret += 16;
	}
	if ((n & 0x000000FF) == 0)
	{
		n >>= 8;
		ret += 8;
	}
	if ((n & 0x0000000F) == 0)
	{
		n >>= 4;
		ret += 4;
	}
	if ((n & 0x00000003) == 0)
	{
		n >>= 2;
		ret += 2;
	}
	if ((n & 0x00000001) == 0)
	{
		ret += 1;
	}

	return ret;
}

/* Return index of the value
	IF the value is not in array, return array_size */
uint32_t my_find32(
	const int32_t* const arr,
	const unsigned int arr_size,
	const int32_t value)
{
	enum {
		SSE_BYTE = 16,
		INT32_PER_SSE = 4
	};

	const int32_t* arr_p = arr;

	// Continue untill aligned by 128bit boundary
	for (; (uintptr_t)arr_p % SSE_BYTE != 0; arr_p++)
	{
		if (*arr_p == value)
		{
			return arr_p - arr;
		}
	}

	// Compare 128bit at a time
	for (; arr_p < (arr + arr_size) - INT32_PER_SSE; arr_p += INT32_PER_SSE)
	{
		const __m128i packed_data = _mm_load_si128((__m128i*)arr_p);
		const __m128i packed_value = _mm_set1_epi32(value);

		const __m128i packed_compare = _mm_cmpeq_epi32(packed_data, packed_value);
		const int32_t compare_mask = _mm_movemask_ps(*(__m128*)(&packed_compare));

		if (compare_mask != 0)
		{
			const uint32_t first_set = ffs(compare_mask);
			return (arr_p + first_set) - arr;
		}
	}

	// Process if they are any remains
	for (; arr_p < arr + arr_size; arr_p++)
	{
		if (*arr_p == value)
		{
			return arr_p - arr;
		}
	}

	// Value is not in Array
	return arr_p - arr;
}

int main()
{
	const int32_t arr[] = { 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

	unsigned int find_idx = my_find32(arr, sizeof(arr) / sizeof(int32_t), 5);

	printf("%u", find_idx); // 15

	return 0;
}
