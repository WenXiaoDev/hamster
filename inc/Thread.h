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
    static void *__run(void *param)
    {
        Thread *runContext = static_cast<Thread *>(param);
        if(nullptr == runContext) {
            printf("__run() invalid parameter.\n");
            return nullptr;
        }

        if(runContext->mIndex < 0 && runContext->mType != THREAD_TYPE_WORK) {
            printf("__run() index = %d, need init first.\n", runContext->mIndex);
            return nullptr;
        }

        pthread_detach(runContext->mTid);

        runContext->mRunning = true;

        pthread_cleanup_push(cleaner, runContext);
        
        while(true) {
            pthread_mutex_lock(&runContext->mTaskLock);
            // while(nullptr == runContext->mTask && !runContext->exitPending()) {
            while(nullptr == runContext->mTask) {
                printf("Thread worker sleep\n");
                pthread_cond_wait(&runContext->mTaskCond, &runContext->mTaskLock);
            }

            // EXIT PENDING CHECK IS NOT USED ANYMORE
            // if(nullptr == runContext->mTask && runContext->mExitPending) {
            //     runContext->mLoaded = false;
            //     runContext->mTask = nullptr;
            //     pthread_mutex_unlock(&runContext->mTaskLock);
            //     break;
            // }
            printf("Thread worker wakeup\n");

            runContext->mTask->routine();
            runContext->mLoaded = false;
            runContext->mTask = nullptr;
            pthread_mutex_unlock(&runContext->mTaskLock);
        }

        pthread_cleanup_pop(0);

        // runContext->setRunningState(false);
        runContext->mRunning = false;
        pthread_cond_signal(&runContext->mCond);

        return nullptr;
    }

    static void cleaner(void *param)
    {
        Thread *runContext = static_cast<Thread *>(param);
        if(nullptr == runContext) {
            printf("cleaner() invalid parameter.\n");
            return;
        }

        pthread_mutex_unlock(&runContext->mTaskLock);
        runContext->mRunning = false;
        pthread_cond_signal(&runContext->mCond);
    }

    Thread();
    virtual ~Thread();
    //Thread(entry_t entry);

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

    // pre-announcement
    class Condition;
    class Locker
    {
    public:
        friend class Condition;

        Locker()
        {
            pthread_mutexattr_t attr;

            // with error check type for compatibility
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
            pthread_mutex_init(&mutex, &attr);

            pthread_mutexattr_destroy(&attr);
        }
        ~Locker() {}

        void lock()
        {
            pthread_mutex_lock(&mutex);
        }

        int trylock()
        {
            return pthread_mutex_trylock(&mutex);
        }

        int unlock()
        {
            return pthread_mutex_unlock(&mutex);
        }
    private:
        pthread_mutex_t mutex;
    };

    class Condition
    {
    public:
        Condition() {pthread_cond_init(&cond, nullptr);}
        ~Condition() {}

        void wait(Locker &lock)
        {
            pthread_cond_wait(&cond, &lock.mutex);
        }

        void waitRelative(Locker &lock, uint64_t time_ms)
        {
            struct timespec tspec;
            struct timeval now;

            gettimeofday(&now, nullptr);
            tspec.tv_sec = now.tv_sec + (now.tv_usec / 1000 + time_ms) % 1000;
            tspec.tv_nsec = (now.tv_usec % 1000) * 1000 + (time_ms % 1000) * 1000000;

            pthread_cond_timedwait(&cond, &lock.mutex, &tspec);
        }

        void signal()
        {
            pthread_cond_signal(&cond);
        }
    private:
        pthread_cond_t cond;
    };

    class AutoLockerEnc
    {
    public:
        AutoLockerEnc(Locker &m) : mutex(m)
        {
            mutex.lock();
        }

        ~AutoLockerEnc()
        {
            mutex.unlock();
        }
    private:
        Locker &mutex;
    };

private:
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