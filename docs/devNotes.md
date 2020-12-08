The `pthread_mutex_unlock()` function attempts to unlock the specified mutex. If there are threads blocked on the mutex object when pthread_mutex_unlock() is called, resulting in the mutex becoming available, the scheduling policy is used to determine which thread acquires the mutex.

If the mutex type is PTHREAD_MUTEX_NORMAL, error detection is not provided. If a thread attempts to unlock a mutex that is has not locked or a mutex which is unlocked, undefined behavior results.

If the mutex type is PTHREAD_MUTEX_ERRORCHECK, error checking is provided. If a thread attempts to unlock a mutex that it has not locked or a mutex that is unlocked, an error is returned.

If the mutex type is PTHREAD_MUTEX_RECURSIVE, the mutex maintains the concept of a lock count. When a thread successfully acquires a mutex for the first time, the lock count is set to one. Every time a thread relocks this mutex, the lock count is incremented by one. When the lock count reaches zero, the mutex becomes available for other threads to acquire. If a thread attempts to unlock a mutex that it has not locked or a mutex that is unlocked, an error is returned.

If the mutex type is PTHREAD_MUTEX_DEFAULT, attempting to unlock the mutex if it was not locked by the calling thread results in undefined behavior. Attempting to unlock the mutex if it is not locked results in undefined behavior.

静态定义：
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
The PTHREAD_MUTEX_INITIALIZER macro initializes the static mutex mutex, setting its attributes to default values. This macro should only be used for static mutexes, as no error checking is performed.

动态定义：
pthread_mutex_t mutex;
pthread_mutex_init (&mutex, NULL);

动态定义和静态定义之间的联系：
The pthread_mutex_init() function shall initialize the mutex referenced by mutex with attributes specified by attr. If attr is NULL, the default mutex attributes are used; the effect shall be the same as passing the address of a  default  mutex attributes object. Upon successful initialization, the state of the mutex becomes initialized and unlocked.

In cases where default mutex attributes are appropriate, the macro PTHREAD_MUTEX_INITIALIZER can be used to initialize mutexes  that  are  statically  allocated.  The  effect  shall  be  equivalent  to dynamic initialization by a call to pthread_mutex_init() with parameter attr specified as NULL, except that no error checks are performed.