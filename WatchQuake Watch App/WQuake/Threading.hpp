//
//  Threading.hpp
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#ifndef Threading_hpp
#define Threading_hpp

#ifdef __cplusplus
extern "C" {
#endif

int GetHardwareConcurrency(void);
void SleepFor(int milliseconds);

#ifdef __cplusplus
}
#endif

#endif /* Threading_hpp */
