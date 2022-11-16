//
//  QuakeWrapper.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#ifndef QuakeWrapper_h
#define QuakeWrapper_h

#import <CoreGraphics/CoreGraphics.h>

#define WQ_STATE_EXIT -1
#define WQ_STATE_PAUSE 0
#define WQ_STATE_PLAY  1

#ifdef __cplusplus
extern "C" {
#endif

#import "Common.h"

extern float g_WQForwardSpeed;
extern float g_WQTurnX;
extern float g_WQTurnY;

typedef struct
{
    i32 frameCounter;
    float meanFrameTime;
    i32 width;
    i32 height;
    i32 devWidth;
    i32 devHeight;
    float devPixelsPerDot;
} WQGameStats_t;

int WQRequestState(int);
void WQNotifyActive(int isActive);
void WQSetScreenSize(int width, int height, float pixelsPerDot);
CGImageRef WQCreateGameImage(void);
void WQInit(void);
void WQSetLoop(void);
void WQFree(void);
int WQGetFrame(void);
WQGameStats_t WQGetStats(void);
const char* WQGetStatsString(void);
int WQShowFPS(void);

void WQInputTapAndPan(CGPoint point, int type);
void WQInputLongPress(void);
void WQInputCrownRotate(float f, float delta);

int WQGetFrameBufferLength(void);
void WQSetAudioFormat(int Hz, uint bits, int channels, int interleaved, int type);

const char* WQGetSelectedMapName(void);

#ifdef __cplusplus
}
#endif

#endif /* QuakeWrapper_h */
