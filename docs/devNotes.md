## pthread_mutex_lock()


## pthread_mutex_unlock()
The `pthread_mutex_unlock()` function attempts to unlock the specified mutex. If there are threads blocked on the mutex object when `pthread_mutex_unlock()` is called, resulting in the mutex becoming available, the scheduling policy is used to determine which thread acquires the mutex.

**Initiation**  
If the mutex type is `PTHREAD_MUTEX_NORMAL`, error detection is not provided. If a thread attempts to unlock a mutex that is has not locked or a mutex which is unlocked, undefined behavior results.

If the mutex type is `PTHREAD_MUTEX_ERRORCHECK`, error checking is provided. If a thread attempts to unlock a mutex that it has not locked or a mutex that is unlocked, an error is returned.

If the mutex type is `PTHREAD_MUTEX_RECURSIVE`, the mutex maintains the concept of a lock count. When a thread successfully acquires a mutex for the first time, the lock count is set to one. Every time a thread relocks this mutex, the lock count is incremented by one. When the lock count reaches zero, the mutex becomes available for other threads to acquire. If a thread attempts to unlock a mutex that it has not locked or a mutex that is unlocked, an error is returned.

If the mutex type is `PTHREAD_MUTEX_DEFAULT`, attempting to unlock the mutex if it was not locked by the calling thread results in undefined behavior. Attempting to unlock the mutex if it is not locked results in undefined behavior.

**static definition**  
`static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;`  
The `PTHREAD_MUTEX_INITIALIZER` macro initializes the static mutex mutex, setting its attributes to default values. This macro should only be used for static mutexes, as no error checking is performed.

**dynamic definition**
```C++
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);
```

**difference between dynamic definition and static definition**  
The `pthread_mutex_init()` function shall initialize the mutex referenced by mutex with attributes specified by attr. If attr is `NULL`, the default mutex attributes are used; the effect shall be the same as passing the address of a  default  mutex attributes object. Upon successful initialization, the state of the mutex becomes initialized and unlocked.

In cases where default mutex attributes are appropriate, the macro `PTHREAD_MUTEX_INITIALIZER` can be used to initialize mutexes  that  are  statically  allocated.  The  effect  shall  be  equivalent  to dynamic initialization by a call to `pthread_mutex_init()` with parameter attr specified as `NULL`, except that no error checks are performed.

## 线程退出
1 从线程内部退出  
接口: `pthread_exit()`
注意事项：不会释放共享内存（mutex、condition variable、semaphores、file descriptors）

2 从外部请求线程退出  
接口：`pthread_cancel(int tid)`  
这个接口的退出行为取决于两个属性:  
a. cancelability state(enable/disable)，通过pthread_setcancelstate()更改  

b. cancelability type(asynchronous/deffered)，通过pthread_setcanceltype()更改  
如果 cancel type为deffered，则需要等到线程执行cancellation_point()才cancel

### pthread_cancel(int tid)专题
从线程tid外部调用时，向线程tid传递cancel消息，线程tid的退出行为取决于自身属性`cancelstate`和`canceltype`
https://blog.csdn.net/worldxie0565/article/details/12004951

`pthread_cancel()`接口调用后不会立刻引起线程退出，默认情况下(cancelstate = PHREAD_CANCEL_ASYNCHRONOUS ; canceltype = PTHREAD_CANCEL_DEFERRED)会等到`cancellation point`才退出；POSIX官方定义的`cancellation point`包含以下几个：
```c++
// POSIX defined
pthread_join(3)
pthread_cond_wait(3)
pthread_cond_timedwait(3)
sem_wait(3)
sigwait(3)

// programmer defined
pthread_testcancel(3)
```
前五个是pthread库中定义的线程取消点  

当线程因为`pthread_cancel()`退出时，已申请的系统资源不会被释放，这时可能会产生死锁。这种情况下，为了确保安全，应当在线程开始处和结尾处分别调用宏函数`pthread_clean_push`和`pthread_clean_pop`；前者会在取消点生效时，调用预先给定的func接口完成清理工作；

```C++
pthread_clean_push(func, param);
// do something
pthread_clean_pop(0);
```

以取消点`pthread_cond_wait()`为例，细节如下。  
首先，这段代码就是所谓的“取消点”，实际上应当是一段时间，而不是字面意义上的某个“时间点”；  
在`pthread_cond_wait()`的实现中，首先通过`__pthread_enable_asynccancel()`将线程的`canceltype`修改为了PHREAD_CANCEL_ASYNCHRONOUS，如果这之前线程已经接收到取消信号，此时将立即生效；  
如果线程没有被取消，那么会调用`__pthread_disable_asynccancel()`，将`canceltype`再改回PTHREAD_CANCEL_DEFERRED，此后如果再接受到取消信号，则需要等到下一个`cancellation point`才会生效；  
综上所述，可以得出，`pthread_cancel`的线程取消指令将会在`canceltype`的修改与恢复之间响应；

```c++
/* Enable asynchronous cancellation. Required by the standard. */
cbuffer.oldtype = __pthread_enable_asynccancel();
 
/* Wait until woken by signal or broadcast. */
lll_futex_wait(&cond->__data.__futex, futex_val);
 
/* Disable asynchronous cancellation. */
__pthread_disable_asynccancel(cbuffer.oldtype);
```

**精心设计的线程，应当妥善使用`pthread_clean_push`,`pthread_clean_pop`来管理线程申请的系统资源；同时也应当妥善设计整个线程中的`cancellation point`，从而可以得心应手地通过`pthread_cancel`接口来控制线程的退出；