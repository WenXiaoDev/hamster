/*
 * MIT License
 * Copyright (c) 2020 WenXiao
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <new>
#include <Windows.h>
#include <errno.h>

#include "ThreadPool.h"

using namespace Hamster;

ThreadPoolTask::ThreadPoolTask()
{
    mThreadPtr = 0;
    mPoolSize = THREAD_POOL_SIZE_DEFAULT;
    mMaxTaskNum = THREAD_POOL_TASK_NUM_DEFAULT;
}

int ThreadPoolTask::init()
{
    if(mPoolSize <= 0 || mMaxTaskNum <= 0) {
        return -1;
    }

    try {
        mTaskQueue = new std::queue<ITask *>;

        mPoolThread = new Thread;
        mPoolThread->init(0);
        setPoolThread(mPoolThread);

        mThreads = new std::vector<Thread>(mPoolSize);
        for(int i = 0; i < mThreads->size(); i++) {
            (*mThreads)[i].init(i);
        }

    } catch(std::bad_alloc) {
        return -1;
    }

    return 0;
}

void ThreadPoolTask::reset()
{

}

bool ThreadPoolTask::setPoolThread(Thread *thread)
{
    mPoolThread = thread;
}

int ThreadPoolTask::release()
{
    // none yet
}

int ThreadPoolTask::run()
{
#if SWITCH
    static int avoid = 1;
    int ret = 0;

    while(true) {
        if(mTaskQueue->empty()) {
            Sleep(avoid);
            avoid = (128 >= avoid) ? (avoid << 1) : 128;
            continue;
        }

        avoid = 1; // index avoidence

        for(int i = 0; i < mThreads->size(); i++) {
            mThreadPtr = (mThreadPtr >= mPoolSize) ? 0 : mThreadPtr;
            ret = mThreads->at(mThreadPtr).loadTask(mTaskQueue->front()); // none block
            if(0 == ret) {
                mTaskQueue->pop();
                break;
            }
            
            // process failure
            if(EINVAL == ret) {
                printf("THREAD POOL EXCEPTION\n");
            } else if(EBUSY == ret) {
                mThreadPtr++;
            }
        }

    }
#else
    Thread *carrierThread = nullptr;
    
    while(true) {
        if(mTaskQueue->empty()) {
            Sleep(10); // sleep for 10ms
            continue;
        }

        // find a empty work thread first
        for(int i = 0; i < mThreads->size(); i++) {
            mThreadPtr = (mThreadPtr >= mPoolSize) ? 0 : mThreadPtr;
            if(!(*mThreads)[mThreadPtr].isLoaded()) {
                carrierThread = &mThreads->at(mThreadPtr);
                break;
            }
        }

        auto task = mTaskQueue->front();
        mTaskQueue->pop();


    }
#endif
}

int ThreadPoolTask::postTask(ITask *task)
{
    if(nullptr == task) {
        return -1;
    }

    mTaskQueue->push(task);

    return 0;
}