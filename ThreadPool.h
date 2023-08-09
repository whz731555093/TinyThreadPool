#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <vector>
#include <queue>
#include <thread>               /* 线程相关 */
#include <mutex>                /* 互斥量 解决资源抢占问题 */
#include <condition_variable>   /* 条件量 用于唤醒线程和阻塞线程 */
#include <functional>           /* 函数子，可以理解为规范化的函数指针 */

class ThreadPool {
public:
    ThreadPool(int ThreadNum);
    ~ThreadPool();

    /**
    * @brief 新增任务 加入线程池的任务队列
    * @param None
    * @return None
    **/
    template<class F, class... Args>    /* 可变列表模板 */
    void EnqueueTask(F&& f, Args&&... args) {  /* 函数模板中，右值引用=万能引用，传左值则进行左值引用，传右值则进行右值引用 */
        /* 完美转发在此处等价于万能引用 */
        std::function<void()> Task =
            std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            /* 操作共享变量之前先加锁 */
            std::unique_lock<std::mutex> ThreadLock(this->ThreadMutex);

            /* 将任务传入线程池的任务队列中 */
            this->TaskQueue.emplace(std::move(Task));
        }
        /* 条件变量通知线程 */
        this->ThreadConditionVar.notify_one();
    }
private:
    /* 线程池在任务中一般只有一个，因此可以用单例模式，.
    即禁用拷贝构造和= 单例模式 智能指针 call_once() */
    ThreadPool(const ThreadPool& Thread_Pool) = delete;
    ThreadPool& operator=(const ThreadPool& Thread_Pool) = delete;

    /* 线程数组 */
    std::vector<std::thread> ThreadWoker;
    /* 任务队列 */
    std::queue<std::function<void()>> TaskQueue;

    /* 互斥量 */
    std::mutex ThreadMutex;
    /* 条件变量 */
    std::condition_variable ThreadConditionVar;

    /* 停止标志位 */
    bool StopThreadPool;
};

#endif