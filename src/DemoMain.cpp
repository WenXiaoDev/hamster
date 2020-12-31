#include <cstdio>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "ITask.h"
#include "Thread.h"

using namespace std;
using namespace Hamster;

/*
    x = (x + a/x)/2
*/
class DemoTask : public ITask
{
public:
    static const char *name;
    DemoTask(float a1, int a2, float r) : arg1(a1), arg2(a2), ret(r) {}
    DemoTask(float a1, int a2) : arg1(a1), arg2(a2), ret(0.0) {}

    ~DemoTask()
    {
        printf("DemoTask Finishing: %s\n", name);
    }

    void setParameters(float a1, int a2)
    {
        arg1 = a1;
        arg2 = a2;
        ret = 0.0;
    }

    void reset()
    {
        setParameters(0.0, 0);
    }

    int run()
    {
        double ans = 1.0;
        
        for(int i = 0; i < arg2; i++) {
            ans = (ans + arg1 / ans) / 2;
        }

        ret = ans;
        printf("the answer is %lf\n", ret);
        return 0;
    }

    double getReturn()
    {
        return ret;
    }

private:
    float arg1;
    int arg2;
    double ret;
};

const char *DemoTask::name = "NewtonSqrtMethod";

int main(int argc, char* argv[])
{
    DemoTask demo(3, 30, 0);
    Thread thread;

    thread.init(1, THREAD_TYPE_NONE);

    for(int i = 0; i < 3; i++) {
        // thread.loadTask(&demo);
        thread.loadTaskAndWait(&demo);
        // Sleep(500);
        printf("\n");
    }
    
    printf("Thread::requestExitAndWait().ret=%d\n", thread.requestExitAndWait());
    return 0;
}