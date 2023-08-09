#include <iostream>
#include "ThreadPool.h"

int main() {
    ThreadPool Thread_Pool(4);

    /* ���10������ */
    for (int i = 0; i < 10; i++) {
        Thread_Pool.EnqueueTask([i] {
            std::cout << "Task: " << i << "is runing" << std::endl;

            /* ������Ϣ1s */
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::cout << "Task: " << i << "is done" << std::endl;
            });
    }
    return 0;
}