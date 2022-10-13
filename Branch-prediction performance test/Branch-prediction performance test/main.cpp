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

#include <random>

enum {
	ITER_NUM = 1000000
};
int arr[ITER_NUM] = { 0, };

int main()
{
	float branchRatio[] = { 
		0.5f, 
		0.6f, 
		0.7f, 
		0.8f, 
		0.9f, 
		1.0f 
	};
	
	std::random_device rd;
	std::mt19937 gen(rd());
	for (int test_i = 0; test_i < sizeof(branchRatio) / sizeof(int); test_i++)
	{
		memset(arr, 0, ITER_NUM * sizeof(int));

		// arr set random_value or 0, based branchRatio
		std::uniform_int_distribution<int> dist(0, ITER_NUM);
		for (int i = 0; i < ITER_NUM * branchRatio[test_i];)
		{
			const int rand_idx = dist(gen);
			if (arr[rand_idx] == 0)
			{
				arr[rand_idx] = dist(gen);
				i++;
			}
		}

		// compute sum
		int sum = 0;
		timeStart();
		{
			for (int i = 0; i < ITER_NUM - 2; i++)
			{
				if (arr[i] != 0)
					sum += (arr[i] * 5) + (arr[i] % 5) 
						+ (arr[i + 1] * 5) + (arr[i + 1] % 5)
						+ (arr[i + 2] * 5) + (arr[i + 2] % 5);
				else
					sum -= (arr[i] * 7) + (arr[i] % 7) 
						+ (arr[i + 1] * 7) + (arr[i + 1] % 7)
						+ (arr[i + 2] * 7) + (arr[i + 2] % 7);;
			}
		}
		timeEnd();
		cout << branchRatio[test_i] * 100 << "% : ";
		timePrint();

		// for avoid release optimization
		cout << "(sum : " << sum << ")" << endl << endl;
	}
	return 0;
}