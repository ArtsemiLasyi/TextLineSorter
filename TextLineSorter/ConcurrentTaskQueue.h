#pragma once

#include <queue> 
#include <functional>
#include <mutex>
#include <windows.h>

typedef std::function<void()> Task;
typedef std::queue<Task> TaskQueue;
typedef std::mutex Mutex;

class ConcurrentTaskQueue
{
public:
	ConcurrentTaskQueue();
	~ConcurrentTaskQueue();
	Task Pop();
	void Push(Task task);
	Task WinPop();
	void WinPush(Task task);
	bool IsEmpty();
private:
	TaskQueue* taskQueue;
	Mutex mutex;
	HANDLE winMutex;
};

