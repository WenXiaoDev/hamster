#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <assert.h>

using namespace std;

class Singleton
{
public:
    Singleton()
    {
        printf("Singleton On Create\n");
    }

    ~Singleton()
    {
        printf("Singleton On Destroy\n");
    }
};

Singleton &getInstance()
{
    static Singleton singleton;
    return singleton;
}

void *entry(void *param)
{
    printf("entry\n");
    Singleton &gs = getInstance();
    
    for(int i = 0; i < 50; i++) {
        gs = getInstance();
    }
    
    return nullptr;
}

template <typename T>
void __print(const T &t)
{
    cout<<t<<endl;
}

template <typename T, typename...Args>
void __print(const T &t, const Args&...args)
{
    cout<<t<<" ";
    __print(args...);
}

int main(int argc, char *argv[])
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t threads[30];
    for(int i = 0; i < 30; i++) {
        pthread_create(&threads[i], &attr, entry, nullptr);
    }
    pthread_attr_destroy(&attr);

    for(int i = 0; i < 30; i++) {
        pthread_join(threads[i], nullptr);
    }

    const char *str = "string";
    const int i = 1;
    const float f = 2.0;
    const bool b = true;
    __print(str, i, f, b);
    cout<<2.1<<endl;
    assert(1 == 0);
    return 0;
}