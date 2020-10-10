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

#ifndef __HAMSTER_TASK_H__
#define __HAMSTER_TASK_H__

#include <cstdio>

namespace Hamster {
typedef void *(*entry_t)(void *);


class ITask
{
public:
    ITask() {};
    virtual ~ITask() {};

    virtual void reset() = 0;
    virtual int run() = 0;
private:

};

class Example : public ITask
{
public:
    Example()
    {
        reset();
    }

    Example(int a1, float a2, bool a3) : arg1(a1), arg2(a2), arg3(a3) {}

    void reset()
    {
        arg1 = 0;
        arg2 = 0.0;
        arg3 = false;
    }

    void setParameters(int a1, float a2, bool a3)
    {
        // may need mutex
        arg1 = a1;
        arg2 = a2;
        arg3 = a3;
    }

    int run()
    {
        printf("arg1 = %d, arg2 = %f, arg3 = %s", arg1, arg2, arg3 ? "true": "false");

        ret = 'c';

        return 0;
    }

private:
    int arg1;
    float arg2;
    bool arg3;
    char ret;
};

} /* namespace Hamster */

#endif /* __HAMSTER_TASK_H__ */