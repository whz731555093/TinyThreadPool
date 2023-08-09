#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <vector>
#include <queue>
#include <thread>               /* �߳���� */
#include <mutex>                /* ������ �����Դ��ռ���� */
#include <condition_variable>   /* ������ ���ڻ����̺߳������߳� */
#include <functional>           /* �����ӣ��������Ϊ�淶���ĺ���ָ�� */

class ThreadPool {
public:
    ThreadPool(int ThreadNum);
    ~ThreadPool();

    /**
    * @brief �������� �����̳߳ص��������
    * @param None
    * @return None
    **/
    template<class F, class... Args>    /* �ɱ��б�ģ�� */
    void EnqueueTask(F&& f, Args&&... args) {  /* ����ģ���У���ֵ����=�������ã�����ֵ�������ֵ���ã�����ֵ�������ֵ���� */
        /* ����ת���ڴ˴��ȼ����������� */
        std::function<void()> Task =
            std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            /* �����������֮ǰ�ȼ��� */
            std::unique_lock<std::mutex> ThreadLock(this->ThreadMutex);

            /* ���������̳߳ص���������� */
            this->TaskQueue.emplace(std::move(Task));
        }
        /* ��������֪ͨ�߳� */
        this->ThreadConditionVar.notify_one();
    }
private:
    /* �̳߳���������һ��ֻ��һ������˿����õ���ģʽ��.
    �����ÿ��������= ����ģʽ ����ָ�� call_once() */
    ThreadPool(const ThreadPool& Thread_Pool) = delete;
    ThreadPool& operator=(const ThreadPool& Thread_Pool) = delete;

    /* �߳����� */
    std::vector<std::thread> ThreadWoker;
    /* ������� */
    std::queue<std::function<void()>> TaskQueue;

    /* ������ */
    std::mutex ThreadMutex;
    /* �������� */
    std::condition_variable ThreadConditionVar;

    /* ֹͣ��־λ */
    bool StopThreadPool;
};

#endif