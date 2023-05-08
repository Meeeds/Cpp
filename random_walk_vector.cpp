#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;



bool no_need_to_to_continue(vector<int> current_point, int index, int maxIndex) {
    int total = 0;
    for(int val : current_point){
        total+=std::abs(val);
    }
    if (total > (maxIndex-index) ){
        cout << " no_need_to_to_continue(" << maxIndex-index << ")" << " total=" << total << endl;
        return true;
    }
    return false;
}

class Points {
    public : 

    Points() = default;

    Points(int aDimension)
    {
        for(int i=0;  i < aDimension; ++i){
            _current_point.push_back(0);
        }  
    }

    bool is_zero(){
        for(int k=0;  k < _current_point.size(); ++k){
            if(_current_point[k]!=0){
                return false;
            }
        }
        return true;
    }

    void print_point() {
        cout << "_current_point=" ; 
        for(int l=0;  l < _current_point.size(); ++l){
            cout << _current_point[l] << ", " ;
        }
        cout << endl;
    }

    vector<int> _current_point;
};

int main()
{
    unsigned int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<unsigned int>::max();
    //int MAX_TRY_BEFORE_GIVEUP = 10000;
    int DIMENSION = 3;
    int NUMBER_OF_TRY = 1000;
    int MODULO = 3;
    cout << "MAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl;
    cout << "NUMBER_OF_TRY=" << NUMBER_OF_TRY << endl;
    cout << "MODULO=" << MODULO << endl;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr_index(0, DIMENSION-1); // define the range


    //fill the vector with the dimension size number of try

    vector<Points> global_vector;
    for(int i=0;  i < NUMBER_OF_TRY; ++i){
        global_vector.push_back(Points(DIMENSION));
    }

    //to print overall progress
    double step = 0.1;
    double nextPrint = step;

    //counters
    int went_back_to_start_count = 0;
    int failure_count = 0;
    int max_steps_before_going_back = 0;
    for(unsigned int i=0;  i < MAX_TRY_BEFORE_GIVEUP; ++i){
        
        double percent = 100*( (double) i / MAX_TRY_BEFORE_GIVEUP);
        if (percent >= nextPrint)
        {
            float failures = (100.0 * global_vector.size() ) / NUMBER_OF_TRY;
            std::cout << "FAILURES " << std::fixed << setprecision(2) << failures << "%" 
                << " Global progress:  " << setfill(' ') << setw(2) << percent << "%" << " steps done: " << i << endl;
            std::cout.flush();
            if(nextPrint>2){
                nextPrint += 10*step;
            }else{
                nextPrint += step;
            }
        }
        //go one step for each point in the global vector
        for (auto it = global_vector.begin(); it != global_vector.end(); ){
            
            //go +1 or -1 ?
            int aStep = 0;
            if (std::rand() % 2 == 0){
                aStep = 1;
            }else{
                aStep = -1;
            }

            //go +1 or -1 on a rdm direction
            int random_index = distr_index(gen);
            it->_current_point[random_index] += aStep;
            if ( MODULO > 0 && random_index == (DIMENSION-1)) {
                it->_current_point[random_index] = (it->_current_point[random_index] + MODULO ) % MODULO;
            }

            if(it->is_zero()){
                std::cout << "i = " << i << " going to erase ";
                it->print_point();
                global_vector.erase(it);
                went_back_to_start_count++;
            }else{
                ++it;
            }
        }
    }

    failure_count=global_vector.size();
    std::cout << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << endl;
    std::cout << "max_steps_before_going_back " << max_steps_before_going_back << endl;
    
}