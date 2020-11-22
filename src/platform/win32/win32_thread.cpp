struct platform_job
{
    platform_job_queue_callback* Callback;
    void* Data;
};

struct platform_job_queue
{
    uint32 volatile JobCompletionGoal;
    uint32 volatile JobCompletionCount;

    uint32 volatile NextJobToWrite;
    uint32 volatile NextJobToRead;

    HANDLE SemaphoreHandle;

    platform_job Jobs[256];
};

struct win32_thread_info
{
    DWORD ThreadId;
    HANDLE ThreadHandle;

    platform_job_queue* Queue;
};

// @NOTE(Anders E): Can only be called from one thread safely.
internal void win32_thread_schedule_job(platform_job_queue* queue, platform_job_queue_callback* callback, void* data)
{
    uint32 newNextJobToWrite = (queue->NextJobToWrite + 1) % array_length(queue->Jobs);
    assert(newNextJobToWrite != queue->NextJobToRead);
    platform_job* job = queue->Jobs + queue->NextJobToWrite;
    job->Callback = callback;
    job->Data = data;
    queue->JobCompletionGoal++;
    _WriteBarrier();
    _mm_sfence();
    queue->NextJobToWrite = newNextJobToWrite;
    ReleaseSemaphore(queue->SemaphoreHandle, 1, 0);
}

internal bool win32_thread_execute_next_job(platform_job_queue* queue)
{
    bool shouldSleep = false;

    uint32 nextJobToRead = queue->NextJobToRead;
    uint32 newNextJobToRead = (nextJobToRead + 1) % array_length(queue->Jobs);

    if (nextJobToRead != queue->NextJobToWrite)
    {
        // dequeue the next job to execute from the job queue
        uint32 index = InterlockedCompareExchange(
            (LONG volatile*)&queue->NextJobToRead,
            newNextJobToRead,
            nextJobToRead
        );

        // ensure its the same job as before the thread safe increment
        if (index == nextJobToRead)
        {
            platform_job* job = &queue->Jobs[index];
            job->Callback(queue, job->Data);
            InterlockedIncrement((LONG volatile*)&queue->JobCompletionCount);
        }
    }
    else
    {
        //there is nothing in the queue -> set thread to sleep
        shouldSleep = true;
    }

    return shouldSleep;
}

internal void win32_thread_complete_all_jobs(platform_job_queue* queue)
{
    platform_job job = {};

    while (queue->JobCompletionGoal != queue->JobCompletionCount)
    {
        win32_thread_execute_next_job(queue);
    }

    queue->JobCompletionGoal = 0;
    queue->JobCompletionCount = 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    win32_thread_info* threadInfo = (win32_thread_info*)lpParameter;

    while (GlobalApplicationRunning)
    {
        if (win32_thread_execute_next_job(threadInfo->Queue))
        {
            WaitForSingleObjectEx(threadInfo->Queue->SemaphoreHandle, INFINITE, FALSE);
        }
    }

    return 0;
}

internal win32_thread_info* win32_thread_create_workers(platform_job_queue* queue)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    uint32 availableThreads = systemInfo.dwNumberOfProcessors;

    if (availableThreads <= 0)
        availableThreads = 1;

    win32_thread_info* threads = (win32_thread_info*)malloc(availableThreads * sizeof(win32_thread_info));

    HANDLE semaphoreHandle = CreateSemaphoreEx(0, 0, availableThreads, 0, 0, SEMAPHORE_ALL_ACCESS);
    queue->SemaphoreHandle = semaphoreHandle;

    for (uint32 threadIndex = 0; threadIndex < availableThreads; threadIndex++)
    {
        win32_thread_info* threadInfo = threads + threadIndex;
        threadInfo->Queue = queue;
        DWORD threadId;
        HANDLE threadHandle = CreateThread(0, 0, ThreadProc, threadInfo, 0, &threadId);
        threadInfo->ThreadId = threadId;
        threadInfo->ThreadHandle = threadHandle;
    }

    return threads;
}