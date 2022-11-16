//
//  Common.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#include "Common.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <mach/mach.h>
#include <mach/mach_time.h>

void* AlignedMalloc(size_t size, size_t alignment)
{
    alignment -= 1;
    uintptr_t r = (uintptr_t)malloc(size + alignment + sizeof(uintptr_t));
    if (!r) return NULL;
    
    uintptr_t t = r + sizeof(uintptr_t);
    uintptr_t o = (t + alignment) & ~(uintptr_t)alignment;
    ((uintptr_t*)o)[-1] = r;
    return (void*)o;
}

void AlignedFree(void* p)
{
    if (!p) return;
    free((void*)(((uintptr_t*)p)[-1]));
}

int AlignedCheck(void* p, int alignment)
{
    assert(alignment != 0);
    
    return ((uintptr_t)p % alignment) == 0;
}

u64 GetTimeNanoSeconds()
{
    static mach_timebase_info_data_t sTimebaseInfo;
    uint64_t time = mach_absolute_time();
    uint64_t nanos;
    
    if (sTimebaseInfo.denom == 0)
    {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    //printf("numer %i denom %i\n",sTimebaseInfo.numer,sTimebaseInfo.denom);
    nanos = (u64)(time) * (u64)(sTimebaseInfo.numer) / sTimebaseInfo.denom;
    return nanos;
}

double GetDeltaTime(u64 dt)
{
    return (double)(dt) / 1000000000.0;
}

void TimeStepAccumulator_Set(TimeStepAccumulator_t* t, u64 freq, u64 limit)
{
    t->accumulation = 0;
    t->frequency = freq;
    t->stepSize = 1000000000 / freq;
    t->accLimit = limit * t->stepSize;
}
void TimeStepAccumulator_Reset(TimeStepAccumulator_t* t)
{
    t->accumulation = 0;
}
double TimeStepAccumulator_GetStepSize(TimeStepAccumulator_t* t)
{
    return 1.0 / (double)(t->frequency);
}
double TimeStepAccumulator_GetDeltaTime(TimeStepAccumulator_t* t)
{
    u64 tmp = (t->accumulation > t->stepSize) ? t->stepSize : t->accumulation;
    return (double)(tmp) / 1000000000.0;
}
double TimeStepAccumulator_GetDeltaTimeAccumulated(TimeStepAccumulator_t* t)
{
    u64 tmp = t->accumulation;
    return (double)(tmp) / 1000000000.0;
}
int TimeStepAccumulator_Update(TimeStepAccumulator_t* t, u64 dt)
{
    t->accumulation += dt;
    if (t->accumulation > t->accLimit)
    {
        t->accumulation = t->accLimit;
    }
    return t->accumulation >= t->stepSize;
}
int TimeStepAccumulator_Tick(TimeStepAccumulator_t* t)
{
    int b = t->accumulation >= t->stepSize;
    if (b)
    {
        t->accumulation -= t->stepSize;
    }
    return b;
}
double TimeStepAccumulator_GetAlpha(TimeStepAccumulator_t* t)
{
    return TimeStepAccumulator_GetDeltaTimeAccumulated(t) / TimeStepAccumulator_GetStepSize(t);
}
