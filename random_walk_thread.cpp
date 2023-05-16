#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <algorithm>
#include <thread>
#include <numeric>
#include <atomic>
#include <iomanip>
#include <deque>

using namespace std;

void print_point(vector<int> current_point) {
    cout << "current_point=" ; 
    for(int l=0;  l < current_point.size(); ++l){
        cout << current_point[l] << ", " ;
    }
    cout << endl;
}


class Progress {
    public:

    //Progress() = default;

    int _percent = 0;
    int _went_back_to_start = 0;
};

void do_random_walk(const int thread_id, const int size, const int dimension, 
                    const unsigned int max_try_before_giveup, const int MODULO, int& oWent_back_to_start,
                    std::atomic<int>& progress)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr_index(0, dimension-1); // for index random
    std::uniform_int_distribution<> distr_step(0, 1); // define step random

        //fill the vector with the dimension
    vector<int> current_point;
    for(int i=0;  i < dimension; ++i){
        current_point.push_back(0);
    }

        //to print overall progress
    int step = 2;
    int nextPrint = step;

    //counters
    oWent_back_to_start = 0;
    for(int i=0;  i < size; ++i){
        
        int percent = (100 * i) / size;
        if (percent >= nextPrint)
        {   
            //std::cout << "Thread progress(" << thread_id << "):  " << std::string(percent/5, '|') << percent << "%" << std::endl;
            progress = percent;
            progress.notify_one();
            nextPrint += step;
        }

        //reset starting point to zero
        for(int j=0;  j < dimension; ++j){
            current_point[j]=0;
        }

        //random walk
        for(unsigned int j=0;  j < max_try_before_giveup; ++j){

            //go +1 or -1 ?
            int aStep = 0;
            if ( distr_step(gen) == 0){
                aStep = 1;
            }else{
                aStep = -1;
            }

            //go +1 or -1 on a rdm direction
            int random_index = distr_index(gen);
            current_point[random_index] += aStep;
            if ( MODULO > 0 && random_index == (dimension-1)) {
                current_point[random_index] = (current_point[random_index] + MODULO ) % MODULO;
            }

            //did we went back to zero ?
            bool went_back_to_start = true;
            for(int k=0;  k < dimension; ++k){
                if(current_point[k]!=0){
                    went_back_to_start=false;
                    break;
                }
            }

            if (went_back_to_start) {
                oWent_back_to_start++;
                break;
            }
        }
    }

    progress=100;

}

void monitor_progress(std::deque<std::atomic<int>>& progress, int size){
    bool keep = true;
    float barWidth = 15.0;
    while (keep){
        chrono::milliseconds dura( 100 );
        this_thread::sleep_for(dura); // Sleeps for a bit
        keep = false;
        cout << "\r";      

        for (int i = 0; i < progress.size(); i++) {
            int l_progress = progress[i];
            cout << " T#" << i << ":[";
            int pos = l_progress * (barWidth / 100);
            for (int w = 0; w < barWidth; ++w) {
                if (w < pos) std::cout << "=";
                else if (w == pos) std::cout << ">";
                else std::cout << " ";
            }
            if (l_progress<100){
                std::cout << "|" << std::setfill(' ') << std::setw(2) << l_progress << "%"  << "]" ;
            }else{
                std::cout << "|END"  << "]" ;
            }
            
            if(l_progress != 100){
                keep = true;
            }
        }
        std::cout << endl;
    }
}

int main()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    unsigned int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<unsigned int>::max()   ;
    int DIMENSION = 2;
    int NUMBER_OF_RDM_WALKS = 100;
    int MODULO = 0;
    int THREAD_NUMBER = 10;
    cout << "MAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl;
    cout << "NUMBER_OF_RDM_WALKS=" << NUMBER_OF_RDM_WALKS << endl;
    cout << "THREAD_NUMBER=" << THREAD_NUMBER << endl;
    cout << "WALKS_PER_THREAD=" << NUMBER_OF_RDM_WALKS/THREAD_NUMBER << endl;
    int REAL_NUMBER_OF_RDM_WALKS = THREAD_NUMBER * (NUMBER_OF_RDM_WALKS/THREAD_NUMBER);
    cout << "REAL_NUMBER_OF_RDM_WALKS=" << REAL_NUMBER_OF_RDM_WALKS << endl;
    cout << "MODULO=" << MODULO << endl;

    cout << "C++:" <<__cplusplus << endl;

    std::vector<std::thread> threads(THREAD_NUMBER);
    std::vector<int> went_back_count(THREAD_NUMBER);

    std::deque<std::atomic<int>> cProgress;
    
    // spawn n threads:
    for (int i = 0; i < THREAD_NUMBER; i++) {
        went_back_count[i] = 0;
        cProgress.emplace_back(0);
        threads[i] = std::thread(do_random_walk, i, NUMBER_OF_RDM_WALKS/THREAD_NUMBER, DIMENSION, MAX_TRY_BEFORE_GIVEUP, MODULO, std::ref(went_back_count[i]),  std::ref(cProgress[i]) );
    }


    
    monitor_progress(std::ref(cProgress), REAL_NUMBER_OF_RDM_WALKS);

    for (int i = 0; i < THREAD_NUMBER; i++) {
        threads[i].join();
    }

    int went_back_to_start_count = std::accumulate(went_back_count.begin(), went_back_count.end(), decltype(went_back_count)::value_type(0));


    //do_random_walk(1, NUMBER_OF_RDM_WALKS, DIMENSION, MAX_TRY_BEFORE_GIVEUP, MODULO, went_back_to_start_count);
    int failure_count = REAL_NUMBER_OF_RDM_WALKS - went_back_to_start_count;
    std::cout << endl  << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << "="<< 100.0* failure_count / REAL_NUMBER_OF_RDM_WALKS << "%" << endl;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[sec]" << std::endl;

    return 0;
}