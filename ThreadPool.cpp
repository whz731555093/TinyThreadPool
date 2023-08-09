#include "ThreadPool.h"

/**
* @brief 创建线程池.
* @param ThreadNum 创建的线程池中线程数
* @param 默认停止标志位为false
* @return None
**/
ThreadPool::ThreadPool(int ThreadNum) :StopThreadPool(false) {
    for (int i = 0; i < ThreadNum; i++) {
        this->ThreadWoker.emplace_back([this] {
            /* 每个线程的工作 */
            for (;;) {
                /* 线程需要操作任务队列，加锁防止竞争 */
                std::unique_lock<std::mutex> ThreadLock(this->ThreadMutex);

                /* 判断任务队列中是否有任务：若有任务，则不阻塞 */
                ThreadConditionVar.wait(ThreadLock, [this] {
                    return !this->TaskQueue.empty() || this->StopThreadPool;
                    });

                /* 如果终止标志位为true 且 任务队列为空，则返回 */
                if (this->StopThreadPool && this->TaskQueue.empty()) {
                    return;
                }

                /* 取任务 */
                std::function<void()> Task(std::move(this->TaskQueue.front()));

                /* 取出后，从任务队列删除任务 */
                this->TaskQueue.pop();

                /* 任务队列解锁 */
                ThreadLock.unlock();

                /* 执行任务 */
                Task();
            }
            });
    }
};

/**
* @brief 销毁线程池.
* @param None
* @return None
**/
ThreadPool::~ThreadPool() {
    {
        /* 由于要操作停止标志位，因此需加锁 */
        std::unique_lock<std::mutex> ThreadLock(this->ThreadMutex);

        /* 线程停止标志位置为true */
        StopThreadPool = true;
    }
    /* 条件变量通知所有线程可以开始工作 */
    this->ThreadConditionVar.notify_all();

    /* 所有线程开始工作 */
    for (auto& thread : ThreadWoker) {
        thread.join();
    }
}