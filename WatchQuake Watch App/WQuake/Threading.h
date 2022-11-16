//
//  Threading.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#ifndef Threading_h
#define Threading_h

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*ThreadingFunction)(void*);

void SetConcurrency(int i);
void SetAudioMixerQOS(int i);
void SetAudioMixerLoop(ThreadingFunction,void*);
void SetGameUpdateTask(ThreadingFunction, const char* name);
void SetGameLoopState(int i);

#ifdef __cplusplus
}
#endif

#endif /* Threading_h */
