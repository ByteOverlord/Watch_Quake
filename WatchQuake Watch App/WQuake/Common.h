//
//  Common.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#ifndef Common_h
#define Common_h

#include <stdint.h>

typedef uint8_t byte;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define WQ_BLITTER_BLOCK64
#define WQ_AUDIO_BLOCK64

#define WQ_AUDIO_UNDERWATER_FILTER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u64 accumulation;
    u64 frequency;
    u64 stepSize;
    u64 accLimit;
} TimeStepAccumulator_t;

u64 GetTimeNanoSeconds(void);
double GetDeltaTime(u64 dt);

void* AlignedMalloc(size_t size, size_t alignment);
void AlignedFree(void* p);
int AlignedCheck(void* p, int alignment);

void TimeStepAccumulator_Set(TimeStepAccumulator_t* t, u64 freq, u64 limit);
void TimeStepAccumulator_Reset(TimeStepAccumulator_t* t);
double TimeStepAccumulator_GetStepSize(TimeStepAccumulator_t* t);
double TimeStepAccumulator_GetDeltaTime(TimeStepAccumulator_t* t);
double TimeStepAccumulator_GetDeltaTimeAccumulated(TimeStepAccumulator_t* t);
int TimeStepAccumulator_Update(TimeStepAccumulator_t* t, u64 dt);
int TimeStepAccumulator_Tick(TimeStepAccumulator_t* t);
double TimeStepAccumulator_GetAlpha(TimeStepAccumulator_t* t);

#ifdef __cplusplus
}
#endif

#endif /* Common_h */
