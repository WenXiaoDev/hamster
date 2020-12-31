#include <pthread.h>
#include <cstdio>
#include <windows.h>

pthread_mutex_t lock;
pthread_cond_t cond;

void cleaner(void *param)
{
    printf("cleaner.enter\n");
    pthread_mutex_unlock(&lock);
    printf("cleaner.exit\n");
}

void *testEntry(void *param)
{
    pthread_cleanup_push(cleaner, NULL);

    pthread_mutex_lock(&lock);
    //pthread_testcancel();

    pthread_cond_wait(&cond, &lock);

    printf("testEntry goes here\n");

    pthread_cleanup_pop(0);
    return NULL;
}

int cancelTest()
{
    pthread_t tid;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&tid, NULL, testEntry, NULL);
    Sleep(5000);

    pthread_cancel(tid);

    getchar();

    return 0;
}

int main(int argc, char *argv[])
{
    int *tmpState = nullptr;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, tmpState);

    printf("%ull  tmpState = \n", pthread_self());

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, tmpState);

    return 0;
}