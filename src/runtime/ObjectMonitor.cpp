// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "ObjectMonitor.h"

void ObjectMonitor::enter(std::thread::id tid) {
    std::unique_lock<std::mutex> lock(internalMtx);

    if (monitorCnt == 0) {
        monitorCnt = 1;
        owner = tid;
        entered = true;
    } else {
        if (tid == owner) {
            monitorCnt++;
            entered = true;
        } else {
            cv.wait(lock, [=] { return monitorCnt == 0; });
            entered = true;
        }
    }
}

void ObjectMonitor::exit() {
    std::unique_lock<std::mutex> lock(internalMtx);

    if (!entered) {
        throw std::runtime_error("illegal monitor state");
    }

    monitorCnt--;
    if (monitorCnt == 0) {
        cv.notify_one();
    }
}
