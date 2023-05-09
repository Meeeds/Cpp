#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <cmath>


using namespace std;


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

    void set_take_off(){
        //already take off, keep it
        if (_take_off) {
            return;
        }
        bool take_off = true;
        for(int k=0;  k < _current_point.size(); ++k){
            if(_current_point[k]==0){
                take_off = false;
                break;
            }
        }
        _take_off = take_off;
    } 

    bool one_is_zero(){
        for(int k=0;  k < _current_point.size(); ++k){
            if(_current_point[k]==0){
                return true;
            }
        }
        return false;
    }

    void print_point() const {
        cout << "(" ; 
        for(int l=0;  l < _current_point.size(); ++l){
            cout << _current_point[l] ;
            if(l!=_current_point.size()-1){
               cout << "," ;
            }
        }
        cout << ")" ; 
    }

    vector<int> _current_point;
    bool _take_off = false ;
};

void print_global_vector(const vector<Points>& aGlobal_vector ){
    std::cout << "global_vector[" << aGlobal_vector.size() << "] = [";
    for (auto it = aGlobal_vector.begin(); it != aGlobal_vector.end(); ++it ){
        it->print_point();
    }
    cout << "]" << endl;
}

int main()
{

    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();

    //unsigned int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<unsigned int>::max();
    unsigned int MAX_TRY_BEFORE_GIVEUP = sqrt(std::numeric_limits<unsigned int>::max());
    //int MAX_TRY_BEFORE_GIVEUP = 10000;
    int DIMENSION = 2;
    int NUMBER_OF_RDM_WALKS = 100000;
    int MODULO = 0;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    //std::mt19937 gen; // to have deterministic code 
    std::uniform_int_distribution<> distr_index(0, DIMENSION-1); // for index random
    std::uniform_int_distribution<> distr_step(0, 1); // define step random

    cout << "MAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl; 
    cout << "NUMBER_OF_RDM_WALKS=" << NUMBER_OF_RDM_WALKS << endl;
    cout << "MODULO=" << MODULO << endl;

    //fill the vector with the dimension size and number of try
    vector<Points> global_vector;


    for(int i=0;  i < NUMBER_OF_RDM_WALKS; ++i){
        global_vector.push_back(Points(DIMENSION));
    }

    //to print overall progress
    double step = 0.1;
    double nextPrint = step;

    //counters
    int went_back_to_start_count = 0;
    int failure_count = 0;
    unsigned int max_steps_before_going_back = 0;
    int success_x = 0;
    int success_y = 0;
    for(unsigned int walk_number=0;  walk_number < MAX_TRY_BEFORE_GIVEUP; ++walk_number){
        
        //just some progress bar
        double percent = 100*( (double) walk_number / MAX_TRY_BEFORE_GIVEUP);
        if (percent >= nextPrint)
        {
            float failures = (100.0 * global_vector.size() ) / NUMBER_OF_RDM_WALKS;
            std::cout << "FAILURES " << std::fixed << setprecision(2) << failures << "%" 
                << " Global progress:  " << setfill(' ') << setw(2) << percent << "%" << " steps done: " << walk_number << endl;
            print_global_vector(global_vector);
            // reset the seed
            // gen.seed((myclock::now() - beginning).count()); // remove to have deterministic code 
            if(nextPrint>2){
                nextPrint += 10*step;
            }else{
                nextPrint += step;
            }
        }

        //print_global_vector(global_vector);

        //go one step for each point in the global vector
        if(global_vector.size()==0){
            break;
        }
        for (auto it = global_vector.begin(); it != global_vector.end(); ){           
            
            //go +1 or -1 ?
            int aStep = 0;
            if ( distr_step(gen) == 0){
                aStep = 1;
            }else{
                aStep = -1;
            }

            //go +1 or -1 on a rdm direction
            
            //const unsigned int random_index = 0;
            const unsigned int random_index = distr_index(gen);

            it->_current_point[random_index] += aStep;
            if ( MODULO > 0 && random_index == (DIMENSION-1)) {
                it->_current_point[random_index] = ((it->_current_point[random_index] + MODULO ) % MODULO);
            }
            
            it->set_take_off();

            // erase the point if it went back to zero, chose one of the if you prefer
            // if(it->is_zero()){
            if(it->_take_off && it->one_is_zero()){
                //std::cout <<  " going to erase " ; it->print_point(); cout  << endl;
                it = global_vector.erase(it);
                went_back_to_start_count++;
                if (walk_number + 1 > max_steps_before_going_back){
                    max_steps_before_going_back = walk_number + 1;
                }
            }else{
                ++it;
            }
        }
    }

    failure_count=global_vector.size();
    std::cout << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << "="<< 100.0* failure_count / NUMBER_OF_RDM_WALKS << "%" << endl;
    std::cout << "max_steps_before_going_back " << max_steps_before_going_back << endl;
    
    return 0;
}