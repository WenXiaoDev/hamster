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
#include "util/syncUtils.h"

namespace Hamster {

#define THREAD_POOL_SIZE_DEFAULT        20
#define THREAD_POOL_TASK_NUM_DEFAULT    512
#define SWITCH                          1

class ThreadPool : public Thread
{
public:
    /**
     * @brief Meyers Singleton ThreadPool
     * @return a reference of global ThreadPool object
     */
    static ThreadPool &getInstance();

    /**
     * @brief Blocking method, post a task to this threadpool
     * @param task a ITask object
     * @return 0 for succeed, a negative value for failure
     */
    int postTask(ITask * task);

    /**
     * @brief Clear all ITask objects, Would Block
     * @param None
     * @return 0 for succeed, a negative value for failure
     */
    int clearAll();

    /**
     * @brief Clear ITasks with given message
     * @param msg The message of ITask object
     * @return 0 for succeed, a negative value for failure
     */
    int clearIfExist(int msg);

    /**
     * @brief Current size of mTaskQueue
     * @param None
     * @return the number of ITasks in the task queue
     */
    int currentSize();

private:
    ThreadPool();
    ~ThreadPool();

    /**
     * @brief Called when initiate
     * @param None
     * @return 0 for succeed, a negative value for failure
     */
    int onFirstRef();

    /**
     * @brief Called when release
     * @param None
     * @return 0 for succeed, a negative value for failure
     */
    int onRelease();

    /**
     * @brief Fetch an ITask for a work thread from this ThreadPool
     * @param None
     * @return one ITask pointer in mTaskQueue for succeed, nullptr for failure
     */
    ITask *fetchTask();

    std::vector<Thread> mThreads;
    std::queue<ITask *> mTaskQueue;
    pthread_mutex_t mLock;
    pthread_cond_t mCond;
};

} /* namespace Hamster */

#endif /* __HAMSTER_THREAD_POOL_H__ */