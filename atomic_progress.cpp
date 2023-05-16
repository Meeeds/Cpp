#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <thread>
#include <numeric>
#include <atomic>
#include <iomanip>


void do_work(int size, std::atomic<int>& progress){  
    for(int i=0; i<=size;++i){
        std::chrono::milliseconds dura( 350 );
        std::this_thread::sleep_for(dura); // Sleeps for a bit
        progress = (100*i)/size;
    }
    progress = 100;
}


void monitor_progress(std::atomic<int>& progress0,
                      std::atomic<int>& progress1, 
                      std::atomic<int>& progress2, 
                      std::atomic<int>& progress3)
{ 
    bool isOver=false;
    while(not isOver){
        
        isOver = (progress0+progress1+progress2+progress3== 400) ;
        std::cout << " T#0:" << progress0
             << " T#1:" << progress1
             << " T#2:" << progress2
             << " T#3:" << progress3 << std::endl; 

        std::chrono::milliseconds dura( 200 );
        std::this_thread::sleep_for(dura); // Sleeps for a bit
    }
}

int main()
{

    std::atomic<int> progress0, progress1, progress2, progress3;

    std::thread thread0 = std::thread(do_work, 20, std::ref(progress0));
    std::thread thread1 = std::thread(do_work, 30, std::ref(progress1));
    std::thread thread2 = std::thread(do_work, 40, std::ref(progress2));
    std::thread thread3 = std::thread(do_work, 50, std::ref(progress3));

    monitor_progress(progress0, progress1, progress2, progress3);
    thread0.join();
    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}