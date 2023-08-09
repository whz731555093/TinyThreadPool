#include "ThreadPool.h"

/**
* @brief �����̳߳�.
* @param ThreadNum �������̳߳����߳���
* @param Ĭ��ֹͣ��־λΪfalse
* @return None
**/
ThreadPool::ThreadPool(int ThreadNum) :StopThreadPool(false) {
    for (int i = 0; i < ThreadNum; i++) {
        this->ThreadWoker.emplace_back([this] {
            /* ÿ���̵߳Ĺ��� */
            for (;;) {
                /* �߳���Ҫ����������У�������ֹ���� */
                std::unique_lock<std::mutex> ThreadLock(this->ThreadMutex);

                /* �ж�����������Ƿ��������������������� */
                ThreadConditionVar.wait(ThreadLock, [this] {
                    return !this->TaskQueue.empty() || this->StopThreadPool;
                    });

                /* �����ֹ��־λΪtrue �� �������Ϊ�գ��򷵻� */
                if (this->StopThreadPool && this->TaskQueue.empty()) {
                    return;
                }

                /* ȡ���� */
                std::function<void()> Task(std::move(this->TaskQueue.front()));

                /* ȡ���󣬴��������ɾ������ */
                this->TaskQueue.pop();

                /* ������н��� */
                ThreadLock.unlock();

                /* ִ������ */
                Task();
            }
            });
    }
};

/**
* @brief �����̳߳�.
* @param None
* @return None
**/
ThreadPool::~ThreadPool() {
    {
        /* ����Ҫ����ֹͣ��־λ���������� */
        std::unique_lock<std::mutex> ThreadLock(this->ThreadMutex);

        /* �߳�ֹͣ��־λ��Ϊtrue */
        StopThreadPool = true;
    }
    /* ��������֪ͨ�����߳̿��Կ�ʼ���� */
    this->ThreadConditionVar.notify_all();

    /* �����߳̿�ʼ���� */
    for (auto& thread : ThreadWoker) {
        thread.join();
    }
}