#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <algorithm>


using namespace std;

void print_point(vector<int> current_point) {
    cout << "current_point=" ; 
    for(int l=0;  l < current_point.size(); ++l){
        cout << current_point[l] << ", " ;
    }
    cout << endl;
}

bool no_need_to_to_continue(vector<int> current_point, int index, int maxIndex) {
    int total = 0;
    for(int val : current_point){
        total+=std::abs(val);
    }
    if (total > (maxIndex-index) ){
        cout << " no_need_to_to_continue(" << maxIndex-index << ")" << " total=" << total << endl;
        print_point(current_point);
        return true;
    }
    return false;
}


int main()
{
    //int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<int>::max();
    unsigned int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<unsigned int>::max();;
    int DIMENSION = 1;
    int NUMBER_OF_RDM_WALKS = 1000;
    int MODULO = 0;
    cout << "MAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl;
    cout << "NUMBER_OF_RDM_WALKS=" << NUMBER_OF_RDM_WALKS << endl;
    cout << "MODULO=" << MODULO << endl;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr_index(0, DIMENSION-1); // for index random
    std::uniform_int_distribution<> distr_step(0, 1); // define step random

    //fill the vector with the dimension
    vector<int> current_point;
    for(int i=0;  i < DIMENSION; ++i){
        current_point.push_back(0);
    }

    //to print overall progress
    int step = 2;
    int nextPrint = step;

    //counters
    int went_back_to_start_count = 0;
    int failure_count = 0;
    int max_steps_before_going_back = 0;
    vector<unsigned int> steps_it_went_to_zero;
    for(int i=0;  i < NUMBER_OF_RDM_WALKS; ++i){
        
        int percent = (100 * i) / NUMBER_OF_RDM_WALKS;
        if (percent >= nextPrint)
        {
            std::cout << "\rGlobal progress:  " << std::string(percent/5, '|') << percent << "%" << endl;
            std::cout.flush();
            nextPrint += step;
        }

        //reset starting point to zero
        for(int j=0;  j < DIMENSION; ++j){
            current_point[j]=0;
        }

        //to print each walk progress
        double walk_step = 1.0;
        double walk_nextPrint = walk_step;
        //random walk
        for(unsigned int j=0;  j < MAX_TRY_BEFORE_GIVEUP; ++j){
            
            double walk_percent = 100*( (double) j / MAX_TRY_BEFORE_GIVEUP);
            if (walk_percent >= walk_nextPrint)
            {
                std::cout << "\rWalk: " << std::string(walk_percent/5, '|') << walk_percent << "%";
                std::cout.flush();
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
            if ( MODULO > 0 && random_index == (DIMENSION-1)) {
                current_point[random_index] = (current_point[random_index] + MODULO ) % MODULO;
            }

            //did we went back to zero ?
            bool went_back_to_start = true;
            for(int k=0;  k < DIMENSION; ++k){
                if(current_point[k]!=0){
                    went_back_to_start=false;
                    break;
                }
            }

            if (went_back_to_start) {
                cout << " went_back_to_start loop " << i << " after " << j+1 << " steps " << endl;
                went_back_to_start_count++;
                steps_it_went_to_zero.push_back(j+1);
                if (j+1 > max_steps_before_going_back){
                    max_steps_before_going_back = j+1;
                }
                break;
            }else if (j==MAX_TRY_BEFORE_GIVEUP-1){
                failure_count++;
                cout << " FAILED " << i << " ";
                print_point(current_point);
            }
        }
    }

    std::sort (steps_it_went_to_zero.begin(), steps_it_went_to_zero.end());
    std::cout << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << endl;
    std::cout << "max_steps_before_going_back " << max_steps_before_going_back << endl;
    cout << "steps_it_went_to_zero = [ ";
    for (auto const &here: steps_it_went_to_zero) {
        cout << here << ", ";
    }
    cout << " ]" << endl;
}