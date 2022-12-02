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

#ifndef YVM_YVM_H
#define YVM_YVM_H

#include "../gc/Concurrent.hpp"
#include "../interpreter/Interpreter.hpp"
#include "../runtime/RuntimeEnv.h"

extern RuntimeEnv runtime;

struct YVM {
    explicit YVM();

    static void callMain(const std::string& name);
    static void initialize(const std::string& libPath);

    class ExecutorThreadPool : public ThreadPool {
    public:
        ExecutorThreadPool() : ThreadPool() {}
        void createThread() {
            threads.emplace_back(&ThreadPool::runPendingWork, this);
        }
        size_t getThreadNum() const { return threads.size(); }
        void storeTaskFuture(shared_future<void> taskFuture) {
            taskFutures.push_back(taskFuture);
        }
        vector<shared_future<void>> getTaskFutures() const {
            return taskFutures;
        }

    private:
        vector<shared_future<void>> taskFutures;
    };
    static ExecutorThreadPool executor;
};

#endif  // YVM_YVM_H
