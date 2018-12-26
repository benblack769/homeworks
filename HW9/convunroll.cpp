
#include "converter.h"
#include <iostream>

constexpr uint32_t nums48(uint32_t n){
	return n == 0 ? 0 : nums48(n-1)*10 + 48;
}
void convert_all(unsigned nlines, char *lines[], quote_t nums[]){
	const size_t maxnum = 6;
	for(size_t ll = 0;ll < nlines; ll++){
		char * numstr = lines[ll];
		char * oldnumstr = lines[ll];
		uint32_t num = 0;
		num +=  100 * (uint8_t(*numstr)-48);numstr++;
		num +=  10 * (uint8_t(*numstr)-48);numstr++;
		num +=  (uint8_t(*numstr)-48);numstr++;
		for(size_t i = 3; i <= maxnum; i++){
			if(uint8_t(*numstr)){
	    		num *= 10;
	    		num += uint8_t(*numstr)-48;
	            numstr++;
			}
		}
		//size_t numsize = numstr - oldnumstr;
		nums[ll] = num;// - sub_nums[numsize];
		//std::cout << nums[ll] << std::endl;
	}
}
