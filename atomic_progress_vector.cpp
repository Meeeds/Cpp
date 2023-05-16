#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <deque>

void do_work(int size, std::atomic<int> &progress)
{
    for (int i = 0; i <= size; ++i)
    {
        std::chrono::milliseconds dura(350);
        std::this_thread::sleep_for(dura); // Sleeps for a bit
        progress = (100 * i) / size;
    }
}

void monitor_progress(std::deque<std::atomic<int>> &v_progress)
{
    bool isOver = false;
    while (not isOver)
    {
        isOver = true;
        for (int i = 0; i < v_progress.size(); ++i)
        {
            std::cout << "T#" << i << ":" << v_progress[i] << "% ";
            if (v_progress[i] != 100)
            {
                isOver = false;
            }
        }
        std::cout << std::endl;

        std::chrono::milliseconds dura(200);
        std::this_thread::sleep_for(dura); // Sleeps for a bit
    }
}

int main()
{
    const int THREAD_NUMBER = 4;
    std::vector<std::thread> threads(THREAD_NUMBER);
    std::deque<std::atomic<int>> v_progress;
    // Make and launch THREAD_NUMBER threads
    for (int i = 0; i < THREAD_NUMBER; ++i)
    {
        v_progress.emplace_back(0);
        threads[i] = std::thread(do_work, 10 * (i + 1), std::ref(v_progress[i]));
    }
    // Monitor the threads
    monitor_progress(std::ref(v_progress));

    for (int i = 0; i < THREAD_NUMBER; ++i)
    {
        threads[i].join();
    }
    return 0;
}