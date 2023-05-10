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
                    std::atomic<Progress>* progress)
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
            //std::cout << "\rThread progress(" << thread_id << "):  " << std::string(percent/5, '|') << percent << "%" ; std::cout.flush();
            Progress aProgress = progress->load();
            aProgress._percent=percent;
            progress->store(aProgress);
            nextPrint += step;
        }

        //reset starting point to zero
        for(int j=0;  j < dimension; ++j){
            current_point[j]=0;
        }

        //to print each walk progress
        double walk_step = 1.0;
        double walk_nextPrint = walk_step;
        bool has_printed = false;
        //random walk
        for(unsigned int j=0;  j < max_try_before_giveup; ++j){
            
            double walk_percent = 100*( (double) j / max_try_before_giveup);
            if (walk_percent >= walk_nextPrint)
            {
                //std::cout << "\rWalk(" << thread_id << "): " << std::string(walk_percent/5, '|') << walk_percent << "%"; std::cout.flush(); has_printed = true;
                walk_nextPrint += walk_step;
            }

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
                Progress aProgress = progress->load();
                aProgress._went_back_to_start++;
                progress->store(aProgress);
                break;
            }
        }
        if(has_printed){
            cout << endl;
        }
        //cout << endl;
    }

    Progress aProgress = progress->load();
    aProgress._percent = 100;
    progress->store(aProgress);
}

void monitor_progress(std::vector<std::atomic<Progress>*>* progress, int size){
    bool keep = true;
    float barWidth = 15.0;
    while (keep){
        chrono::milliseconds dura( 200 );
        this_thread::sleep_for(dura); // Sleeps for a bit
        keep = false;
        cout << "\r";
        for (int i = 0; i < progress->size(); i++) {
            int l_progress = (*progress)[i]->load()._percent;
            int success = (*progress)[i]->load()._went_back_to_start;
            cout << " T#" << i << ":[";
            int pos = l_progress * (barWidth / 100);
            for (int w = 0; w < barWidth; ++w) {
                if (w < pos) std::cout << "=";
                else if (w == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "]" << std::setfill(' ') << std::setw(3) << l_progress << "% " ;
            std::cout << "@" << std::fixed << setprecision(2) << success << "=" << (100.0*success)/size << "%" ;
            if(l_progress != 100){
                keep = true;
            }
        }
        std::cout << endl;
    }
}

int main()
{
    //int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<int>::max();
    unsigned int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<unsigned int>::max()/10;
    int DIMENSION = 2;
    int NUMBER_OF_RDM_WALKS = 1000;
    int MODULO = 0;
    int THREAD_NUMBER = 7;
    cout << "MAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl;
    cout << "NUMBER_OF_RDM_WALKS=" << NUMBER_OF_RDM_WALKS << endl;
    cout << "THREAD_NUMBER=" << THREAD_NUMBER << endl;
    cout << "WALKS_PER_THREAD=" << NUMBER_OF_RDM_WALKS/THREAD_NUMBER << endl;
    int REAL_NUMBER_OF_RDM_WALKS = THREAD_NUMBER * (NUMBER_OF_RDM_WALKS/THREAD_NUMBER);
    cout << "REAL_NUMBER_OF_RDM_WALKS=" << REAL_NUMBER_OF_RDM_WALKS << endl;
    cout << "MODULO=" << MODULO << endl;


    std::vector<std::thread> threads(THREAD_NUMBER);
    std::vector<int> went_back_count(THREAD_NUMBER);

    std::vector<std::atomic<Progress>*> cProgress;
    
    // spawn n threads:
    for (int i = 0; i < THREAD_NUMBER; i++) {
        went_back_count[i] = 0;
        cProgress.emplace_back(new std::atomic<Progress>());
        threads[i] = std::thread(do_random_walk, i, NUMBER_OF_RDM_WALKS/THREAD_NUMBER, DIMENSION, MAX_TRY_BEFORE_GIVEUP, MODULO, std::ref(went_back_count[i]),  cProgress[i] );
    }

    monitor_progress(&cProgress, NUMBER_OF_RDM_WALKS/THREAD_NUMBER);

    for (int i = 0; i < THREAD_NUMBER; i++) {
        threads[i].join();
    }

    int went_back_to_start_count = std::accumulate(went_back_count.begin(), went_back_count.end(), decltype(went_back_count)::value_type(0));


    //do_random_walk(1, NUMBER_OF_RDM_WALKS, DIMENSION, MAX_TRY_BEFORE_GIVEUP, MODULO, went_back_to_start_count);
    int failure_count = REAL_NUMBER_OF_RDM_WALKS - went_back_to_start_count;
    std::cout << endl  << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << "="<< 100.0* failure_count / REAL_NUMBER_OF_RDM_WALKS << "%" << endl;

    return 0;
}