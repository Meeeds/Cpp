#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>

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
    int MAX_TRY_BEFORE_GIVEUP = 1000000;
    int DIMENSION = 3;
    int NUMBER_OF_TRY = 10000;
    cout << "aMAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl;
    cout << "NUMBER_OF_TRY=" << NUMBER_OF_TRY << endl;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr_index(0, 5-1); // define the range


    vector<int> current_point;
    for(int i=0;  i < DIMENSION; ++i){
        current_point.push_back(0);
    }

    int step = 2;
    int nextPrint = step;

    int went_back_to_start_count = 0;
    int failure_count = 0;
    for(int i=0;  i < NUMBER_OF_TRY; ++i){
        
        int percent = (100 * i) / NUMBER_OF_TRY;
        if (percent >= nextPrint)
        {
            std::cout << "\rGlobal progress:  " << std::string(percent/5, '|') << percent << "%" << endl;
            std::cout.flush();
            nextPrint += step;
        }
        //reset starting point
        for(int j=0;  j < DIMENSION; ++j){
            current_point[j]=0;
        }
        int walk_step = 1;
        int walk_nextPrint = walk_step;
        //random walk
        for(int j=0;  j < MAX_TRY_BEFORE_GIVEUP; ++j){
            
            int walk_percent = 100*( (float) j / MAX_TRY_BEFORE_GIVEUP);
            if (walk_percent >= walk_nextPrint)
            {
                //std::cout << "\rWalk: " << std::string(walk_percent/5, '|') << walk_percent << "%";
                std::cout.flush();
                walk_nextPrint += walk_step;
            }

            int step = 0;
            if (std::rand() % 2 == 0){
                step = 1;
            }else{
                step = -1;
            }

            current_point[distr_index(gen)/2]+=step;

            bool went_back_to_start = true;
            for(int k=0;  k < DIMENSION; ++k){
                if(current_point[k]!=0){
                    went_back_to_start=false;
                    break;
                }
            }
            if (went_back_to_start) {
                //cout << " went_back_to_start loop " << i << " after " << j+1 << " steps " << endl;
                went_back_to_start_count++;
                break;
            }else if (j==MAX_TRY_BEFORE_GIVEUP-1){
                failure_count++;
                //cout << " FAILED " << i << " ";
                //print_point(current_point);
            }
            /*else if (no_need_to_to_continue(current_point, j, MAX_TRY_BEFORE_GIVEUP)){
                break;
            }*/
        }
    }

    std::cout << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << endl;
}