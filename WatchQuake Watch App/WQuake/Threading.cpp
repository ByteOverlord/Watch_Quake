//
//  Threading.cpp
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#include "Threading.hpp"

#include <thread>
#include <chrono>

int GetHardwareConcurrency(void)
{
    return std::thread::hardware_concurrency();
}

void SleepFor(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
