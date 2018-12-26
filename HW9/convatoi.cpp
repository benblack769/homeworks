
#include "converter.h"

void convert_all(unsigned nlines, char *lines[], quote_t nums[]){
    for(unsigned ll;ll < nlines; ll++){
        nums[ll] = atoi(lines[ll]);
    }
}
