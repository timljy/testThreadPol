// testThreadPool_1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>

#include <thread>
#include <vector>

using namespace std;

#define ARRAY_SIZE 10
#define THREAD_NUMS 2
#define OUTPUT_NUMS 20

enum THREADSTAT
{
	IDAL = 0,
	RUN
};

float g_SharedMenory[THREAD_NUMS][ARRAY_SIZE];
int g_ThreadCnt[THREAD_NUMS];
vector<thread> g_threaHolder;
int g_currCnt;
bool g_exit_Flag;
THREADSTAT g_run_flag[THREAD_NUMS];

struct CopyArrayParam
{
	float* dst;
	float* src;
	int size;
};

vector<CopyArrayParam> g_Param;

void CopyArray(float *des, float* src, int size)
{
	for (size_t i = 0; i < (size_t)size; i++)
	{
		des[i] = src[i];
	}
}

static void CopyfuncThread(vector<CopyArrayParam> param,int theadNum,int* curCnt)
{
	while (true)
	{
		if (g_exit_Flag)
		{
			break;
		}

		if (g_run_flag[theadNum] == RUN)
		{
			CopyArray(param[*curCnt].dst, param[*curCnt].src, param[*curCnt].size);
			g_run_flag[theadNum] = IDAL;
		}
		else
		{
			chrono::milliseconds timespan(100);
			this_thread::sleep_for(timespan);
		}
	}
}

int main()
{
	//initialize param
	float Output[OUTPUT_NUMS][ARRAY_SIZE] = { 0 };

	for (size_t i = 0; i < THREAD_NUMS; i++)
	{
		for (size_t a = 0; a < ARRAY_SIZE; a++)
		{
			g_SharedMenory[i][a] = (float)i;
		}

		g_run_flag[i] = IDAL;
	}

	for (size_t i = 0; i < OUTPUT_NUMS; i++)
	{
		CopyArrayParam param;
		param.dst = Output[i];
		param.src = g_SharedMenory[i%THREAD_NUMS];
		param.size = ARRAY_SIZE;

		g_Param.push_back(param);
	}

	g_currCnt = 0;
	g_exit_Flag = false;

	//create thread
	for (size_t i = 0; i < THREAD_NUMS; i++)
	{
		g_threaHolder.push_back(thread(CopyfuncThread, g_Param, i , &(g_ThreadCnt[i])));
	}

	//copy array here
	while (true)
	{
		if (g_currCnt == OUTPUT_NUMS)
		{
			break;
		}

		int count = min(THREAD_NUMS, OUTPUT_NUMS - g_currCnt);
		for (size_t i = 0; i < count; i++)
		{
			g_ThreadCnt[i] = g_currCnt + i;
		}

		for (size_t i = 0; i < count; i++)
		{
			g_run_flag[i] = RUN;
		}

		for (size_t i = 0; i < THREAD_NUMS; i++)
		{
			while (g_run_flag[i] != IDAL)
			{
				chrono::milliseconds timespan(10);
				this_thread::sleep_for(timespan);
			}
		}
		g_currCnt += count;
	}

	for (size_t i = 0; i < OUTPUT_NUMS; i++)
	{
		for (size_t a = 0; a < ARRAY_SIZE; a++)
		{
			std::cout << Output[i][a] << " ";
		}
		std::cout << "\n";
	}

	system("pause");

	//clear thread
	g_exit_Flag = true;
	for (size_t i = 0; i < THREAD_NUMS; i++)
	{
		g_threaHolder[i].join();
	}

	g_threaHolder.clear();
}