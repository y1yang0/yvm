#ifndef YVM_YVM_H
#define YVM_YVM_H

#include "RuntimeEnv.h"
#include "CodeExecution.hpp"
#include "Concurrent.hpp"

extern RuntimeEnv yrt;

struct YVM {
    explicit YVM();

    static bool loadClass(const char* name);
    static bool linkClass(const char* name);
    static bool initClass(CodeExecution& exec, const char* name);
    static void callMain(const char* name);
    static void warmUp(const std::vector<std::string> & libPaths);

    class ExecutorThreadPool : public ThreadPool {
    public:
        ExecutorThreadPool() :ThreadPool() {}
        void createThread() { threads.emplace_back(&ThreadPool::runPendingWork, this); }
        size_t getThreadNum() const { return threads.size(); }
        void storeTaskFuture(shared_future<void> taskFuture) { taskFutures.push_back(taskFuture); }
        vector<shared_future<void>> getTaskFutures() const { return taskFutures; }

    private:
        vector<shared_future<void>> taskFutures;
    };
    static ExecutorThreadPool executor;
};

#endif //YVM_YVM_H
