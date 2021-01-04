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
#include <cstdint>
#include <sys/time.h>

#include "ITask.h"

namespace Hamster {

typedef enum {
    THREAD_TYPE_QUEUE = 0,
    THREAD_TYPE_WORK,
    THREAD_TYPE_NONE,
} thread_type_t;

class Thread
{
public:
    Thread();
    virtual ~Thread();

    int init(int index, thread_type_t type);
    int release();
    
    bool isLoaded();
    bool isRunning();
    bool exitPending();
    
    int requestExit();
    int requestExitAndWait();

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

private:
    static void *__run(void *param);
    static void cleaner(void *param);
    void setRunningState(bool value);

    pthread_t mTid;
    pthread_mutex_t mLock;
    pthread_cond_t mCond;

    pthread_mutex_t mTaskLock;
    pthread_cond_t mTaskCond;

    bool mExitPending;
    bool mRunning;
    bool mLoaded;
    int mIndex;
    thread_type_t mType;
    ITask *mTask;
};

} /* namespace Hamster */

#endif /* __HAMSTER_THREAD_H__ */