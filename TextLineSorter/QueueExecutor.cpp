#include "QueueExecutor.h"


QueueExecutor::QueueExecutor(ConcurrentTaskQueue* queue, int numberofthreads)
{
	InitializeCriticalSection(&this->taskCriticalSection);
	this->numberOfThreads = numberofthreads;
	this->threads = new HANDLE[this->numberOfThreads];
	this->queue = queue;
}

QueueExecutor::~QueueExecutor()
{
	DeleteCriticalSection(&this->taskCriticalSection);
	delete this->threads;
}

void QueueExecutor::Start()
{
	for (int i = 0; i < this->numberOfThreads; i++)
	{
		this->threads[i] = CreateThread(NULL, 0, Process, this, 0, NULL);
	}
}

DWORD WINAPI QueueExecutor::Process(LPVOID param)
{
	QueueExecutor* executor = (QueueExecutor*)param;

	do
	{
		EnterCriticalSection(&(executor->taskCriticalSection));
		if (!executor->queue->IsEmpty())
		{
			std::function<void()> Task = executor->queue->WinPop();
			Task();
		}
		else
		{
			LeaveCriticalSection(&(executor->taskCriticalSection));
			return 0;
		}
		LeaveCriticalSection(&(executor->taskCriticalSection));

	} while (!executor->queue->IsEmpty());

	return 0;
}