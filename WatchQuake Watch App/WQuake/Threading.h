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

void SetGameUpdateTasks(ThreadingFunction gameLoop, ThreadingFunction mixerLoop, const char* name);
void SetGameLoopState(int i);

#ifdef __cplusplus
}
#endif

#endif /* Threading_h */
