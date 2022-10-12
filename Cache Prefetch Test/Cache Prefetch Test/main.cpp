#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

system_clock::time_point startTime;
system_clock::time_point endTime;
__forceinline void timeStart()
{
	startTime = system_clock::now();
}
__forceinline void timeEnd()
{
	endTime = system_clock::now();
}
void timePrint()
{
	cout << static_cast<nanoseconds>(endTime - startTime).count() << "ns" << endl;
}

enum
{
	ITER_NUM = 1000
};

// iteration order index
int row_first_index[ITER_NUM * ITER_NUM];
int col_first_index[ITER_NUM * ITER_NUM];
int random_index[ITER_NUM * ITER_NUM];

int arr1[ITER_NUM][ITER_NUM];
int arr2[ITER_NUM][ITER_NUM];
int arr3[ITER_NUM][ITER_NUM];

#include <random>
int main()
{
	/* order init */
	{
		srand((unsigned int)time(NULL));
		int i = 0;
		for (int row = 0; row < ITER_NUM; row++)
			for (int col = 0; col < ITER_NUM; col++)
				row_first_index[i++] = row * ITER_NUM + col;
		i = 0;
		for (int col = 0; col < ITER_NUM; col++)
			for (int row = 0; row < ITER_NUM; row++)
				col_first_index[i++] = row * ITER_NUM + col;
		i = 0;
		random_device rd;
		mt19937 gen(rd());
		for (; i < ITER_NUM * ITER_NUM; i++)
		{
			uniform_int_distribution<int> rand_dis(0, ITER_NUM * ITER_NUM - 1);
			random_index[i] = rand_dis(gen);
		}
	}

	// row-first loop
	timeStart();
	{
		for (int i = 0; i < ITER_NUM * ITER_NUM; i++)
		{
			((int*)arr1)[row_first_index[i]] = 1;
		}
	}
	timeEnd();
	cout << "row-first loop : ";
	timePrint();

	// col-first loop
	timeStart();
	{
		for (int i = 0; i < ITER_NUM * ITER_NUM; i++)
		{
			((int*)arr2)[col_first_index[i]] = 1;
		}
	}
	timeEnd();
	cout << "col-first loop : ";
	timePrint();

	// random loop
	timeStart();
	{
		for (int i = 0; i < ITER_NUM * ITER_NUM; i++)
		{
			((int*)arr3)[random_index[i]] = 1;
		}
	}
	timeEnd();
	cout << "random loop :    ";
	timePrint();

	return 0;
}