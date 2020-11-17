#include "ConcurrentTaskQueue.h"
#include <mutex>
#include <windows.h>

ConcurrentTaskQueue::ConcurrentTaskQueue()
{
	this->taskQueue = new TaskQueue();
	this->winMutex = CreateMutex(NULL, false, NULL);
}

ConcurrentTaskQueue::~ConcurrentTaskQueue()
{
	delete this->taskQueue;
	CloseHandle(this->winMutex);
}

Task ConcurrentTaskQueue::Pop()
{
	this->mutex.lock();

	Task task;
	if (this->taskQueue->empty())
		task =  nullptr;
	else 
	{
		task = this->taskQueue->front();
		this->taskQueue->pop();
	}

	this->mutex.unlock();

	return task;
}

void ConcurrentTaskQueue::Push(Task task)
{
	this->mutex.lock();

	this->taskQueue->push(task);

	this->mutex.unlock();
}


Task ConcurrentTaskQueue::WinPop()
{
	DWORD waitResult = WaitForSingleObject(this->winMutex, INFINITE);
	if (waitResult == WAIT_OBJECT_0)
	{
		Task task;
		if (this->taskQueue->empty())
			task = nullptr;
		else
		{
			task = this->taskQueue->front();
			this->taskQueue->pop();
		}
		ReleaseMutex(this->winMutex);
		return task;
	}
	else
		return nullptr;
}

void ConcurrentTaskQueue::WinPush(Task task)
{
	DWORD waitResult = WaitForSingleObject(this->winMutex, INFINITE);
	if (waitResult == WAIT_OBJECT_0)
	{
		this->taskQueue->push(task);
		ReleaseMutex(this->winMutex);
	}
}

bool ConcurrentTaskQueue::IsEmpty()
{
	return this->taskQueue->empty();
}