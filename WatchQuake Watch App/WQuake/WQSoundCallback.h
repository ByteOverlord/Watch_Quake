//
//  WQSoundCallback.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#ifndef WQSoundCallback_h
#define WQSoundCallback_h

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

void* WQAudioMixerLoop(void* p);
void WQAudioCallback(void *const *channelData, uint frameCount, uint channelCount, uint bits);

#ifdef __cplusplus
}
#endif

#endif /* WQSoundCallback_h */
