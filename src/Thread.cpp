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
    mTask = nullptr;
    mExitPending = false;
    mRunning = false;
    mLoaded = false;
    mIndex = -1;
    mTask = nullptr;
    mType = THREAD_TYPE_NONE;
}

Thread::~Thread()
{
    release();
}

int Thread::init(int index, thread_type_t type)
{
    pthread_attr_t attr;

    mType = type;
    mIndex = index;

    pthread_mutex_init(&mLock, nullptr);
    pthread_cond_init(&mCond, nullptr);
    pthread_mutex_init(&mTaskLock, nullptr);
    pthread_cond_init(&mTaskCond, nullptr);

    pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
    pthread_create(&mTid, &attr, __run, this);

    pthread_attr_destroy(&attr);

    return 0;
}

int Thread::release()
{
    if (mRunning) {
        return requestExitAndWait();
    }

    return 0;
}

bool Thread::isRunning()
{
    //AutoLocker autoLocker(&mLock);
    return mRunning;
}

bool Thread::exitPending()
{
    AutoLocker autoLocker(&mLock);
    return mExitPending;
}

// unused
int Thread::requestExit()
{
    // printf("requestExit.enter\n");
    // if(pthread_equal(pthread_self(), mTid)) {
    //     return -1;
    // }

    // pthread_mutex_lock(&mLock);
    // mExitPending = true;
    // pthread_mutex_unlock(&mLock);

    // pthread_cond_signal(&mTaskCond);

    if(pthread_equal(pthread_self(), mTid)) {
        return -1;
    }

    pthread_cancel(mTid);

    return 0;
}

int Thread::requestExitAndWait()
{
    printf("Thread::requestExitAndWait().enter\n");
    // printf("requestExitAndWait.enter\n");
    // int ret = requestExit();
    // if(-1 != ret) {
    //     pthread_mutex_lock(&mLock);
    //     while(mRunning) {
    //         pthread_cond_wait(&mCond, &mLock);
    //     }
    //     pthread_mutex_unlock(&mLock);
    // }
    if(pthread_equal(pthread_self(), mTid)) {
        return -1;
    }

    // pthread_mutex_lock(&mLock);

    // if(!mRunning) {
    //     pthread_mutex_unlock(&mLock);
    //     return 0;
    // }

    // mExitPending = true;
    // pthread_cond_signal(&mTaskCond);
    // while(mRunning) {
    //     pthread_cond_wait(&mCond, &mLock);
    // }

    // pthread_mutex_unlock(&mLock);

    if(!mRunning) {
        return 0;
    }

    pthread_cancel(mTid);
    pthread_mutex_lock(&mLock);
    while(mRunning) {
        pthread_cond_wait(&mCond, &mLock);
    }
    pthread_mutex_unlock(&mLock);

    printf("Thread::requestExitAndWait().exit\n");

    return 0;
}

// unused
bool Thread::isLoaded()
{
    AutoLocker autoLocker(&mLock);
    return mLoaded;
}

// unused
void Thread::setRunningState(bool vaule)
{
    AutoLocker autoLocker(&mLock);
    mRunning = vaule;
}

int Thread::loadTask(ITask* task)
{
    // EBUSY or EAGAIN or EINVAL or 0
    int ret = pthread_mutex_trylock(&mTaskLock);
    if(0 == ret) {
        mTask = task;
        mLoaded = true;
        pthread_mutex_unlock(&mTaskLock);
        pthread_cond_signal(&mTaskCond);
    } else if(EINVAL == ret) {
        printf("Thread::loadTask().error need initiate mutex first.\n");
    }

    return ret;
}

//unused
int Thread::loadTaskAndWait(ITask* task)
{
    if(pthread_equal(pthread_self(), mTid)) {
        return -1;
    }

    pthread_mutex_lock(&mTaskLock);
    printf("loadTaskAndWait get locked\n");
    mTask = task;
    mLoaded = true;
    pthread_mutex_unlock(&mTaskLock);
    pthread_cond_signal(&mTaskCond);

    return 0;
}