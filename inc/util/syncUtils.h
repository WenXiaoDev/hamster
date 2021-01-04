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

#ifndef __HAMSTER_SYNC_UTIL_H__
#define __HAMSTER_SYNC_UTIL_H__

#include <pthread.h>
#include <sys/time.h>
#include <cstdint>

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

#endif /* __HAMSTER_SYNC_UTIL_H__ */