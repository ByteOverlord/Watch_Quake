//
//  Threading.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#include "Threading.h"
#include "Threading.hpp"

#include "Common.h"

#include <pthread.h>
#include <stdio.h>

pthread_attr_t qosActive;
pthread_t myThread;

void SetConcurrency(int i)
{
    int threads = GetHardwareConcurrency();
    i = i > threads ? threads : i;
    i = i < 0 ? 0 : i;
    pthread_setconcurrency(i);
}

void SetAudioMixerLoop(ThreadingFunction func, void* ptr)
{
    pthread_attr_init(&qosActive);
    pthread_attr_set_qos_class_np(&qosActive,QOS_CLASS_USER_INTERACTIVE,0);
    pthread_create(&myThread,&qosActive,func,ptr);
}

void SetAudioMixerQOS(int i)
{
    printf("audiomixer qos ");
    if (i)
    {
        printf("user_interactive\n");
        pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE,0);
    }
    else
    {
        printf("background\n");
        pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND,0);
    }
}

#include <dispatch/dispatch.h>

dispatch_queue_t myQueue;
dispatch_source_t myTimer;

void SetGameUpdateTask(ThreadingFunction func, const char* name)
{
    dispatch_queue_attr_t qosAttribute = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_CONCURRENT,QOS_CLASS_USER_INTERACTIVE,0);
    myQueue = dispatch_queue_create(name,qosAttribute);
    myTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,myQueue);//dispatch_get_main_queue());
    u64 leeaway = (1.0 / 240.0) * 1000000000.0;
    u64 interval = (1.0 / 60.0) * 1000000000.0;
    dispatch_source_set_timer(myTimer,dispatch_walltime(NULL,0),interval,leeaway);
    dispatch_source_set_event_handler_f(myTimer,(dispatch_function_t)func);
}

void SetGameLoopState(int i)
{
    printf("gameloop ");
    if (i)
    {
        printf("resume\n");
        dispatch_resume(myTimer);
    }
    else
    {
        printf("suspend\n");
        dispatch_suspend(myTimer);
    }
}
