//
//  Threading.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#include "Threading.h"

#include "Common.h"

#include <stdio.h>

#include <dispatch/dispatch.h>

dispatch_queue_t gameQueue;
dispatch_source_t gameTimer;
dispatch_source_t mixerTimer;

void SetGameUpdateTasks(ThreadingFunction gameLoopFunc, ThreadingFunction mixerLoopFunc, const char* name)
{
    //dispatch_get_main_queue()); // serial queue
    dispatch_queue_attr_t qosAttribute = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_CONCURRENT,QOS_CLASS_USER_INTERACTIVE,0);
    gameQueue = dispatch_queue_create(name,qosAttribute);

    gameTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,gameQueue);
    {
        u64 leeaway = (1.0 / 480.0) * 1000000000.0;
        u64 interval = (1.0 / 60.0) * 1000000000.0;
        dispatch_source_set_timer(gameTimer,dispatch_walltime(NULL,0),interval,leeaway);
        dispatch_source_set_event_handler_f(gameTimer,(dispatch_function_t)gameLoopFunc);
    }

    mixerTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,gameQueue);
    {
        u64 leeaway = (1.0 / 120.0) * 1000000000.0;
        u64 interval = (1.0 / 20.0) * 1000000000.0;
        dispatch_source_set_timer(mixerTimer,dispatch_walltime(NULL,0),interval,leeaway);
        dispatch_source_set_event_handler_f(mixerTimer,(dispatch_function_t)mixerLoopFunc);
    }
}

void SetGameLoopState(int i)
{
    printf("game ");
    if (i)
    {
        printf("resume\n");
        dispatch_resume(gameTimer);
        dispatch_resume(mixerTimer);
    }
    else
    {
        printf("suspend\n");
        dispatch_suspend(gameTimer);
        dispatch_suspend(mixerTimer);
    }
}
