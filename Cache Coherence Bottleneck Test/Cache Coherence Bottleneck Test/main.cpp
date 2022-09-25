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

#include <thread>
int main()
{
	enum
	{
		CACHE_LINE = 64,
		ITER_NUM = 1000000000,
	};
	alignas(CACHE_LINE) volatile char arr[CACHE_LINE * 2];

	// Same cache-line write
	timeStart();
	{
		thread a([&]()
			{
				for (int i = 0; i < ITER_NUM / 2; i++)
				{
					arr[0] = 1;
				}
			});
		thread b([&]()
			{
				for (int i = 0; i < ITER_NUM / 2; i++)
				{
					arr[CACHE_LINE - 1] = 2;
				}
			});
		a.join();
		b.join();
	}
	timeEnd();
	cout << "Same cache-line 2 thread write : ";
	timePrint();

	// Different cache-line 2 thread write
	timeStart();
	{
		thread a([&]()
			{
				for (int i = 0; i < ITER_NUM / 2; i++)
				{
					arr[0] = 1;
				}
			});
		thread b([&]()
			{
				for (int i = 0; i < ITER_NUM / 2; i++)
				{
					arr[CACHE_LINE] = 2;
				}
			});
		a.join();
		b.join();
	}
	timeEnd();
	cout << "Different cache-line 2 thread write : ";
	timePrint();

	// Just single thread write
	timeStart();
	{
		thread a([&]()
			{
				for (int i = 0; i < ITER_NUM; i++)
				{
					arr[0] = 1;
				}
			});
		a.join();
	}
	timeEnd();
	cout << "Just single thread write : ";
	timePrint();

	return 0;
}