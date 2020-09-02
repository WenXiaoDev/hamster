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

#include <pthread.h>
#include <errno.h>

#include "Thread.h"

using namespace Hamster;



Thread::Thread()
{
    mRunning = false;
    mLoaded = false;
    mIndex = -1;
    mTask = nullptr;
}

int Thread::init(int index)
{
    pthread_attr_t attr;

    if(index < 0) {
        return -1;
    }

    mIndex = index;

    pthread_mutex_init(&mLock, nullptr);
    pthread_cond_init(&mCond, nullptr);
    pthread_mutex_init(&mTaskLock, nullptr);
    pthread_cond_init(&mTaskCond, nullptr);

    pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
    pthread_create(&mTid, &attr, __run, this);

    pthread_attr_destroy(&attr);

    mRunning = true;

    return 0;
}

int Thread::release()
{

    return 0;
}

bool Thread::isRunning()
{
    //
    return false;
}

void Thread::setRunningState(bool vaule)
{

}

int Thread::loadTask(ITask* task)
{
    int ret = pthread_mutex_trylock(&mTaskLock);
    if(0 == ret) {
        mTask = task;
        pthread_mutex_unlock(&mTaskLock);
        pthread_cond_signal(&mTaskCond);
        return 0;
    }
    
    // EBUSY or EAGAIN or EINVAL
    if(EINVAL == ret) {
        printf("Thread::loadTask().error need initiate mutex first.\n");
    }

    return -1;
}