#ifndef _ASYNCWORK
#define _ASYNCWORK
#include "Config.h"
#include <thread>
#define  MAXTHREADNUM  16
class IQueuedWork
{
public:

	/**
	* This is where the real thread work is done. All work that is done for
	* this queued object should be done from within the call to this function.
	*/
	virtual void DoThreadedWork() = 0;

public:

	/**
	* Virtual destructor so that child implementations are guaranteed a chance
	* to clean up any resources they allocated.
	*/
	virtual ~IQueuedWork() { }
};

template<typename TTask>
class FAsyncTask:public IQueuedWork
{
	/** User job embedded in this task */
	TTask Task;
	/** Thread safe counter that indicates WORK completion, no necessarily finalization of the job */
	/** Pool we are queued into, maintained by the calling thread */
public:
	void Init(){};
	void DoThreadedWork() override
	{
		Task.DoWork();
	}
public:
	/** Default constructor. */
	FAsyncTask()
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T>
	FAsyncTask(T Arg)
		: Task(Arg)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2>
	FAsyncTask(T1 Arg1, T2 Arg2)
		: Task(Arg1, Arg2)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3)
		: Task(Arg1, Arg2, Arg3)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3, typename T4>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4)
		: Task(Arg1, Arg2, Arg3, Arg4)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5)
		: Task(Arg1, Arg2, Arg3, Arg4, Arg5)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6)
		: Task(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7)
		: Task(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8)
		: Task(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)
	{
		Init();
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	FAsyncTask(T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8, T9 Arg9)
		: Task(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)
	{
		Init();
	}

	/** Destructor, not legal when a task is in process */
	~FAsyncTask()
	{
	}

	/* Retrieve embedded user job, not legal to call while a job is in process
	* @return reference to embedded user job
	**/
	TTask &GetTask()
	{
		return Task;
	}

	/* Retrieve embedded user job, not legal to call while a job is in process
	* @return reference to embedded user job
	**/
	const TTask &GetTask() const
	{
		return Task;
	}

};

class  FQueuedThreadPool
{
public:
	/** Tells the pool to clean up all background threads */

	void DoAllWork();

	void AddWork(IQueuedWork * task);

public:
	FQueuedThreadPool(){ }

	~FQueuedThreadPool() {}
private:
	std::thread workThread[MAXTHREADNUM];
	TArray<IQueuedWork *> workQueue;
};

#endif // !_ASYNCWORK
