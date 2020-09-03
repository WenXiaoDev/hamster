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

#ifndef __HAMSTER_THREAD_H__
#define __HAMSTER_THREAD_H__

#include <pthread.h>

#include "ITask.h"

namespace Hamster {
class Thread
{
public:
    friend void *__run(void *param);
    Thread() = default;
    virtual ~Thread();
    //Thread(entry_t entry);

    int init(int index);
    int release();
    
    bool isRunning();
    bool exitPending();
    int requestExit();
    int requestExitAndWait();

    bool isLoaded();
    void setRunningState(bool value);

    /**
     * @brief   try to load a task
     * @param   task a ITask object pointer
     * @return  EBUSY on mutex busy, EAGAIN on system interrupt, with both you may try again
     *          EINVAL on error: init failed
     */
    int loadTask(ITask *task);
    int loadTaskAndWait(ITask *task);

    Thread(Thread &thread) = delete;
    Thread &operator=(Thread &other) = delete;

    class AutoLocker
    {
    public:
        AutoLocker(pthread_mutex_t *m) : mutex(m)
        {
            pthread_mutex_lock(mutex);
        }

        ~AutoLocker()
        {
            pthread_mutex_unlock(mutex);
        }
    private:
        pthread_mutex_t *mutex;
    };
private:
    pthread_t mTid;
    pthread_mutex_t mLock;
    pthread_cond_t mCond;

    pthread_mutex_t mTaskLock;
    pthread_cond_t mTaskCond;

    bool mExitPending;
    bool mRunning;
    bool mLoaded;
    int mIndex;
    ITask *mTask;
};

void *__run(void *param)
{
    Thread *thiz = static_cast<Thread *>(param);
    if(nullptr == thiz) {
        printf("__run() invalid parameter.\n");
        return NULL;
    }

    if(thiz->mIndex < 0) {
        printf("__run() index = %d, need init first.\n", thiz->mIndex);
        return NULL;
    }

    pthread_detach(thiz->mTid);

    while(true) {
        pthread_mutex_lock(&thiz->mTaskLock);
        while(nullptr == thiz->mTask) {
            pthread_cond_wait(&thiz->mTaskCond, &thiz->mTaskLock);
        }

        // mTask would be assigned a dummy object when Thread was requested exit
        if(thiz->exitPending()) {
            pthread_mutex_unlock(&thiz->mTaskLock);
            thiz->mTask = nullptr;
            break;
        }

        thiz->mTask->run();
        thiz->mTask = nullptr;
        pthread_mutex_unlock(&thiz->mTaskLock);
    }

    thiz->setRunningState(false);
    pthread_cond_signal(&thiz->mCond);

    return NULL;
}
} /* namespace Hamster */

#endif /* __HAMSTER_THREAD_H__ */