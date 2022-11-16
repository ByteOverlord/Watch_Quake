//
//  cd_watch.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 4.11.2022.
//

#ifndef cd_watch_h
#define cd_watch_h

#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"

void CDAudio_SetPath(const char* path);

void CDAudio_SetMixerSamplerate(u32 sampleRate);

#ifdef __cplusplus
}
#endif

#endif /* cd_watch_h */
