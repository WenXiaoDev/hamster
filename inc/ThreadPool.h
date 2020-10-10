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

#ifndef __HAMSTER_THREAD_POOL_H__
#define __HAMSTER_THREAD_POOL_H__

#include <vector>
#include <stack>
#include <queue>
#include <pthread.h>

#include "Thread.h"
#include "ITask.h"

namespace Hamster {

#define THREAD_POOL_SIZE_DEFAULT        20
#define THREAD_POOL_TASK_NUM_DEFAULT    512
#define SWITCH                          1

class ThreadPoolTask : public ITask
{
public:
    ThreadPoolTask();
    virtual ~ThreadPoolTask();
    int init();
    virtual void reset();
    bool setPoolThread(Thread *thread);
    int release();
    // won't release
    virtual int run();

    int postTask(ITask *task);
private:
    Thread *mPoolThread;
    // 直接在vector上轮询即可，指针随时保存
    std::vector<Thread> *mThreads;
    //std::stack<int> mAvailableThreads;
    std::queue<ITask *> *mTaskQueue;
    int mThreadPtr;
    unsigned int mPoolSize;
    unsigned int mMaxTaskNum;
};

ThreadPoolTask &getThreadPool()
{
    static ThreadPoolTask globalThreadPool;
    return globalThreadPool;
}

} /* namespace Hamster */

#endif /* __HAMSTER_THREAD_POOL_H__ */