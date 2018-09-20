/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  Example of using concurrent bounded queue
 *
 *        Version:  1.0
 *        Created:  2018/09/19 (yyyy/mm/dd)
 *       Revision:  none
 *       Compiler:  g++ (C++14)
 *
 *         Author:  lionking
 *   Organization:  None
 *
 * =====================================================================================
 */

#include <cstdio>
#include <cstdlib>
#include <thread>
#include "cbqueue.h"


using namespace std;
using namespace dt;


ConcurrentBoundedQueue<int> buffer(10);


void read(const int tid)
{
    for (int i = 0; i < 5; ++i) {
        int value = 0;
        buffer.pop(value);
        printf("read thread t%d: value = %d\n", tid, value);
    }
}


void write(const int tid, const int base)
{
    for (int i = base; i < base + 10; ++i) {
        buffer.push(i);
        printf("write thread t%d: value = %d\n", tid, i);
    }
}


int main()
{
    thread rt1(read, 1);
    thread rt2(read, 2);
    thread rt3(read, 3);
    thread rt4(read, 4);
    thread wt1(write, 1, 0);
    thread wt2(write, 2, 10);

    wt1.join();
    wt2.join();
    //buffer.clear();
    rt1.join();
    rt2.join();
    rt3.join();
    rt4.join();

    return 0;
}
