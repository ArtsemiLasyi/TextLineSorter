#include "ConcurrentTaskQueue.h"
#include <windows.h>

#pragma once
class QueueExecutor
{
public:
	QueueExecutor(ConcurrentTaskQueue* queue, int numberofthreads);
	~QueueExecutor();
	void Start();
	int numberOfThreads;
private:
	const int defaultNumberOfThreads = 8;
	static DWORD WINAPI Process(LPVOID param);
	HANDLE* threads;
	ConcurrentTaskQueue* queue;
	CRITICAL_SECTION taskCriticalSection;
};

