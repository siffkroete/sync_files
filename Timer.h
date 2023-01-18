#pragma once
#include <ctime>
using namespace std;


class Timer
{
public:

	Timer()
	{
	}

	~Timer()
	{
	}

	void start(void)
	{
		m_start = clock();
	}

	float getTimeInSek(void)
	{
		return((float)(clock() - m_start) / (float)CLOCKS_PER_SEC);
	}

	

private:
	unsigned long long m_start;
	unsigned long long m_timeElapsed;
};

