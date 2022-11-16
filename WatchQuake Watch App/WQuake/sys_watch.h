//
//  sys_watch.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

//
//  sys_watch.h
//  qwatch
//
//  Created by Tomas Vymazal on 22.04.2022.
//

#ifndef sys_watch_h
#define sys_watch_h

#ifdef __cplusplus
extern "C" {
#endif

int Sys_ShowFPS(void);
void Sys_FrameBeforeRender(void);
void Sys_FrameRender(void);
void Sys_FrameAfterRender(void);
void Sys_Init(const char* resourcesDir, const char* documentsDir, const char* saveDir, const char* commandLine);

#ifdef __cplusplus
}
#endif

#endif /* sys_watch_h */
