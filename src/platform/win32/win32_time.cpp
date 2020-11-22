inline uint64 win32_time_get_perf_counter()
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result.QuadPart;
}

inline float win32_time_get_seconds_elapsed(uint64 start, uint64 end)
{
    return ((float)(end - start) / (float)GlobalPerfCountFrequency);
}