#ifndef YVM_YVM_H
#define YVM_YVM_H

#include "../gc/Concurrent.hpp"
#include "../interpreter/Interpreter.hpp"
#include "../runtime/RuntimeEnv.h"

extern RuntimeEnv yrt;

struct YVM {
    explicit YVM();

    static bool loadClass(const std::string& name);
    static bool linkClass(const std::string& name);
    static bool initClass(Interpreter& exec, const std::string& name);
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
