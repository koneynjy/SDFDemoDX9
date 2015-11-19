#include "AsyncWork.h"


void FQueuedThreadPool::DoAllWork()
{
	uint32 thNum = workQueue.size();
	if (!thNum) return;

	uint32 rep = thNum / MAXTHREADNUM;
	uint32 trail = thNum % MAXTHREADNUM;
	uint32 count = 0;
	for (uint32 i = 0; i < rep; i++)
	{
		for (uint32 t = 0; t < MAXTHREADNUM; t++)
		{
			workThread[t] = std::thread(&IQueuedWork::DoThreadedWork, workQueue[count++]);
		}

		for (uint32 t = 0; t < MAXTHREADNUM; t++)
		{
			workThread[t].join();
		}
	}

	for (uint32 t = 0; t < trail; t++)
	{
		workThread[t] = std::thread(&IQueuedWork::DoThreadedWork, workQueue[count++]);
	}

	for (uint32 t = 0; t < trail; t++)
	{
		workThread[t].join();
	}
}

void FQueuedThreadPool::AddWork(IQueuedWork* work)
{
	if (work)
		workQueue.push_back(work);
}