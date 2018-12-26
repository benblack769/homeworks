#include <iostream>
using namespace std;
const int* highest(int val, const int* start, const int* finish);

const int * average_of(const int* low, const int* high){
    return ((high - low) / 2) + low;
}

const int* highest(int val, const int* start, const int* finish){
    if(start >= finish){
        return nullptr;
    }
    const int* last = finish-1;
    if (*start > val || *last < val){
        return nullptr;
    }
    if(*last == val){
        return last;
    }
    const int* midpoint = average_of(start, finish);
    if(val >= *midpoint){
        return highest(val,midpoint,finish);
    }
    else{
        return highest(val,start,midpoint);
    }
}
const int* lowest(int val, const int* start, const int* finish){
    if(start >= finish){
        return nullptr;
    }
    const int* last = finish-1;
    if (*start > val || *last < val){
        return nullptr;
    }
    if(*start == val){
        return start;
    }
    const int* midpoint = average_of(start, finish);
    if(val > *midpoint){
        return lowest(val,midpoint+1,finish);
    }
    else{
        return lowest(val,start,midpoint+1);
    }
}
typedef const int* (* search_fn_ty) (int , const int* , const int* );
void search_output(search_fn_ty function){
    const int DATA_LEN = 9;
    const int data[DATA_LEN] = { 1, 1, 1, 4, 4, 5, 6, 7, 10 };
    for (auto search : { 0, 1, 2, 4, 7, 8, 9, 10, 11 }) {
        const auto where = function(search, data, data + DATA_LEN);
        std::cout << "Index of elem: " << search << " is: ";
        if (where == nullptr) {
            std::cout << "not found!" << std::endl;
        } else {
            std::cout << (where-data) << std::endl;
        }
    }
}

int main()
{
    cout << "Lowest function\n";
    search_output(lowest);
    cout << "Highest function\n";
    search_output(highest);
    return 0;

}
