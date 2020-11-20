#pragma once

#include "../common/fn_common.h"

// Services that the platform layer provides to the game
struct platform_file_result
{
    uint32 FileSize;
    void* Data;
};

// @TODO(Anders): make async versions of read and write file. Look up I/O completion ports for windows platform layer

#define FN_PLATFORM_FILE_FREE(name) void name(void* data)
typedef FN_PLATFORM_FILE_FREE(platform_file_free);

#define FN_PLATFORM_FILE_WRITE(name) bool name(const char* fileName, uint32 size, void* data)
typedef FN_PLATFORM_FILE_WRITE(platform_file_write);

#define FN_PLATFORM_FILE_READ(name) platform_file_result name(const char* fileName)
typedef FN_PLATFORM_FILE_READ(platform_file_read);

#define FN_PLATFORM_DEBUG_LOG(name) void name(const char* message)
typedef FN_PLATFORM_DEBUG_LOG(platform_debug_log);

struct platform_job_queue;
#define FN_PLATFORM_JOB_QUEUE_CALLBACK(name) void name(platform_job_queue* queue, void* data)
typedef FN_PLATFORM_JOB_QUEUE_CALLBACK(platform_job_queue_callback);

#define FN_PLATFORM_SCHEDULE_JOB(name) void name(platform_job_queue* queue, platform_job_queue_callback* callback, void* data)
typedef FN_PLATFORM_SCHEDULE_JOB(platform_schedule_job);

#define FN_PLATFORM_COMPLETE_ALL_JOBS(name) void name(platform_job_queue* queue)
typedef FN_PLATFORM_COMPLETE_ALL_JOBS(platform_complete_all_jobs);

struct platform_api
{
    platform_file_write* WriteFile;
    platform_file_read* ReadFile;
    platform_file_free* FreeFile;
    platform_debug_log* DebugLog;
    platform_job_queue* HighPriorityQueue;
    platform_schedule_job* ScheduleJob;
    platform_complete_all_jobs* CompleteAllJobs;
};

global_variable platform_api PlatformAPI;