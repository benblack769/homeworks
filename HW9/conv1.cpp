
#include "converter.h"

void convert_all(unsigned nlines, char *lines[], quote_t nums[]){
	for(size_t ll = 0;ll < nlines; ll++){
		uint32_t num = 0;
		for(char * numstr = lines[ll]; *numstr; numstr++){
			num *= 10;
			num += *numstr - 48;
		}
		nums[ll] = num;
	}
}
