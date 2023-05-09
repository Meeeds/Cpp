#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <algorithm>
#include <iomanip>

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
    unsigned int MAX_TRY_BEFORE_GIVEUP = std::numeric_limits<unsigned int>::max();
    //int MAX_TRY_BEFORE_GIVEUP = 10000;
    int DIMENSION = 1;
    int NUMBER_OF_RDM_WALKS = 1000;
    int MODULO = 0;

    std::srand(std::time(nullptr)); 

    cout << "MAX_TRY_BEFORE_GIVEUP=" << MAX_TRY_BEFORE_GIVEUP << endl;
    cout << "DIMENSION=" << DIMENSION << endl; 
    cout << "NUMBER_OF_RDM_WALKS=" << NUMBER_OF_RDM_WALKS << endl;
    cout << "MODULO=" << MODULO << endl;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr_index(0, DIMENSION-1); // define the range


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
    int max_steps_before_going_back = 0;
    for(unsigned int walk_number=0;  walk_number < MAX_TRY_BEFORE_GIVEUP; ++walk_number){
        
        double percent = 100*( (double) walk_number / MAX_TRY_BEFORE_GIVEUP);
        if (percent >= nextPrint)
        {
            float failures = (100.0 * global_vector.size() ) / NUMBER_OF_RDM_WALKS;
            std::cout << "FAILURES " << std::fixed << setprecision(2) << failures << "%" 
                << " Global progress:  " << setfill(' ') << setw(2) << percent << "%" << " steps done: " << walk_number << endl;
            print_global_vector(global_vector);
            if(nextPrint>2){
                nextPrint += 10*step;
            }else{
                nextPrint += step;
            }
        }

        //print_global_vector(global_vector);

        //go one step for each point in the global vector
        int keep_index = 0;
        if(global_vector.size()==0){
            break;
        }
        for (auto it = global_vector.begin(); it != global_vector.end(); ){           
            //go +1 or -1 ?

            if(percent > 50){
                it->print_point();
                cout << "still not at zero ? try something else" << endl;
                unsigned int walk_number2=0;
                for(walk_number2=0;  walk_number2 < MAX_TRY_BEFORE_GIVEUP; ++walk_number2){
                    int aStep2 = 0;
                    if ( (std::rand() % 2) == 0){
                        aStep2 = 1;
                    }else{
                        aStep2 = -1;
                    }
                    it->_current_point[0] += aStep2;
                    if(it->is_zero()){
                        cout << "youpi only after " << walk_number2 << endl;
                        cout << 100.0 * (1.0 * walk_number2) / MAX_TRY_BEFORE_GIVEUP << endl;
                        return 0;
                    }
                    
                    it->print_point();
                    cout << " " << walk_number2 << endl;
                }
                cout << "FAILED NEW";
            }

            int aStep = 0;
            if ( (std::rand() % 2) == 0){
                aStep = 1;
            }else{
                aStep = -1;
            }

            //go +1 or -1 on a rdm direction
            
            unsigned int random_index = distr_index(gen);
            //unsigned int random_index = (std::rand() % 2);

            it->_current_point[0] += aStep;
            if ( MODULO > 0 && random_index == (DIMENSION-1)) {
                it->_current_point[random_index] = ((it->_current_point[random_index] + MODULO ) % MODULO);
            }
            

            //erase the point if it went back to zero
            if(it->is_zero()){
                //std::cout << "i = " << i << " going to erase "; it->print_point();
                global_vector.erase(it);
                went_back_to_start_count++;
                if (walk_number + 1 > max_steps_before_going_back){
                    max_steps_before_going_back = walk_number + 1;
                }
            }else{
                ++it;
            }
            keep_index++;
        }
    }

    failure_count=global_vector.size();
    std::cout << "went_back_to_start_count " << went_back_to_start_count << endl;
    std::cout << "failure_count " << failure_count << endl;
    std::cout << "max_steps_before_going_back " << max_steps_before_going_back << endl;
    
    return 0;
}