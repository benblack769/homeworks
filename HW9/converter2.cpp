/*
 * Trivial implementation of converter function.
 * You may override this file or create your own.
 */
#include "converter.h"

#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <immintrin.h>
#include <chrono>
#include <thread>

using namespace std;
using vec_ty = __m256i;
using half_vec_ty = __m128i;

constexpr size_t VEC_SIZE = 32;
constexpr size_t NUM_VECS = 2;
constexpr size_t BUFWIDTH = VEC_SIZE * NUM_VECS;
constexpr uint8_t STR_MV_UNROLL_FACTOR = 1;
constexpr size_t VEC_32_SIZE = VEC_SIZE / sizeof(int32_t);

constexpr size_t MAX_STR_SIZE = 5;

template<typename unrollfn_ty,uint8_t unroll_factor>
struct unroll_obj{
    void operator()(unrollfn_ty fn,size_t loc){
        unroll_obj<unrollfn_ty,unroll_factor-1>()(fn,loc);
        fn(loc+unroll_factor-1);
    }
};
template<typename unrollfn_ty>
struct unroll_obj<unrollfn_ty,0>{
    void operator()(unrollfn_ty ,size_t ){
        //do nothing
    }
};
template<uint8_t num_times,typename exec_fn>
void exec_many(size_t start,exec_fn fn){
	unroll_obj<exec_fn,num_times>()(fn,start);
}
template<uint8_t unroll_factor,typename unrollfn_ty>
void even_unroll(size_t start,size_t end,unrollfn_ty fn){
    for(size_t i = start; i <= end - unroll_factor; i += unroll_factor){
        exec_many<unroll_factor>(i,fn);
    }
}
template<uint8_t unroll_factor,typename unrollfn_ty>
void unroll(size_t start,size_t end,unrollfn_ty fn){
    size_t i = start;
    for(; i <= end - unroll_factor; i += unroll_factor){
        exec_many<unroll_factor>(i,fn);
    }
	for(; i < end; i++){
		fn(i);
	}
}
inline vec_ty aligned_load(void * loc){
	return _mm256_load_si256(reinterpret_cast<vec_ty *>(loc));
}
inline void aligned_store(void * loc,vec_ty value){
	_mm256_store_si256(reinterpret_cast<vec_ty *>(loc),value);
}
inline vec_ty make_zero(){
	return _mm256_setzero_si256();
}
inline vec_ty set_chars(char c){
	return _mm256_set1_epi8(c);
}
inline vec_ty and8(vec_ty a,vec_ty b){
	return _mm256_and_si256(a,b);
}
inline vec_ty xor8(vec_ty a,vec_ty b){
	return _mm256_xor_si256(a,b);
}
inline vec_ty chars_eq(vec_ty a,vec_ty b){
	return _mm256_cmpeq_epi8(a,b);
}
inline vec_ty cnv8_16(half_vec_ty x){
	return _mm256_cvtepu8_epi16(x);
}
inline vec_ty cnv16_32(half_vec_ty x){
	return _mm256_cvtepu16_epi32(x);
}
inline half_vec_ty hbits(vec_ty x){
	return _mm256_extracti128_si256(x,1);
}
inline half_vec_ty lbits(vec_ty x){
	return _mm256_castsi256_si128(x);
}
inline vec_ty mul16(vec_ty a,vec_ty b){
	return _mm256_mullo_epi16(a,b);
}
inline vec_ty add16(vec_ty a,vec_ty b){
	return _mm256_add_epi16(a,b);
}
inline vec_ty h(vec_ty x){
	return cnv8_16(hbits(x));
}
inline vec_ty l(vec_ty x){
	return cnv8_16(lbits(x));
}
inline vec_ty h16(vec_ty x){
	return cnv16_32(hbits(x));
}
inline vec_ty l16(vec_ty x){
	return cnv16_32(lbits(x));
}
inline void store_16_into_nums(quote_t * at,vec_ty vec){
	aligned_store(at+VEC_32_SIZE*0,l16(vec));
	aligned_store(at+VEC_32_SIZE*1,h16(vec));
}
struct vec2_ty{
	vec_ty v1;
	vec_ty v2;
};
#define make_2_1(fname,vec2,vec) (vec2_ty{fname(vec2.v1,vec),fname(vec2.v2,vec)})
#define make_2_2(fname,vec2,vec22) (vec2_ty{fname(vec2.v1,vec22.v1),fname(vec2.v2,vec22.v2)})
#define make_2_dcall_sing(fname1,fname2,vec) (vec2_ty{fname1(fname2(vec.v1)),fname1(fname2(vec.v2))})
inline void aligned_store(void * loc,vec2_ty value){
	aligned_store(loc,value.v1);
	aligned_store(loc,value.v2);
}
inline vec2_ty and8(vec2_ty a,vec_ty b){
	return make_2_1(and8,a,b);
}
inline vec2_ty xor8(vec2_ty a,vec_ty b){
	return make_2_1(xor8,a,b);
}
inline vec2_ty chars_eq(vec2_ty a,vec_ty b){
	return make_2_1(chars_eq,a,b);
}
inline vec2_ty mul16(vec2_ty a,vec2_ty b){
	return make_2_2(mul16,a,b);
}
inline vec2_ty add16(vec2_ty a,vec2_ty b){
	return make_2_2(add16,a,b);
}
inline vec2_ty h(vec2_ty x){
	return make_2_dcall_sing(cnv8_16,hbits,x);
}
inline vec2_ty l(vec2_ty x){
	return make_2_dcall_sing(cnv8_16,lbits,x);
}
inline vec2_ty h16(vec2_ty x){
	return make_2_dcall_sing(cnv16_32,hbits,x);
}
inline vec2_ty l16(vec2_ty x){
	return make_2_dcall_sing(cnv16_32,lbits,x);
}

inline void store_16_into_nums(quote_t * at,vec2_ty veclo,vec2_ty vechi){
#define store(num,vec,part) aligned_store(at+VEC_32_SIZE*num,part(vec))
	auto store = [&](size_t num,vec_ty vec){
		aligned_store(at+VEC_32_SIZE*num,l16(vec));
	};
	store(0,veclo.v1,l16);
	store(1,veclo.v1,h16);
	store(2,vechi.v1,l16);
	store(3,vechi.v1,h16);
	
	store(4,veclo.v2,l16);
	store(5,veclo.v2,h16);
	store(6,vechi.v2,l16);
	store(7,vechi.v2,h16);
#undef store
}

union {
	char chars[MAX_STR_SIZE][BUFWIDTH];
	vec2_ty vecs[MAX_STR_SIZE];
}buf;

void convert_all(unsigned nlines, char *lines[], quote_t nums[]){
	size_t ll = 0;
	vec_ty n48 = set_chars(~char(48));
	vec_ty nine = set_chars(9);
	vec_ty zero = make_zero();
	vec_ty all_bits = set_chars(0xff);
	for(; ll <= nlines-BUFWIDTH;ll+=BUFWIDTH){
		for(int i = 2; i < 5; i++){
			buf.vecs[i] = vec2_ty{zero,zero};
		}
		even_unroll<STR_MV_UNROLL_FACTOR>(0,BUFWIDTH,[&](size_t obj){
			//size_t obj = 0;
			char * linestr = lines[obj+ll];
			buf.chars[0][obj] = linestr[0];
			buf.chars[1][obj] = linestr[1];
			if(linestr[1]){
				buf.chars[2][obj] = linestr[2];
				if (linestr[2]){
					buf.chars[3][obj] = linestr[3];
					if(linestr[3]){
						buf.chars[4][obj] = linestr[4];
					}
				}
			}
		});
		/*even_unroll<STR_MV_UNROLL_FACTOR>(0,BUFWIDTH,[&](size_t obj){
			for(int i = 0; i < 5; i++)
			cout << buf.chars[i][obj];
			cout << endl;
		});*/
		
		
		//simple formula
		// x1 = x1 & andv + x1 == 0 ? x0 : x0 * 10
		
		//without branches
		// x1 = x1 & andv + (~(x1 == 0) & 9) * x0 + x0
		
		// with types incorperated
		// mulval = ~(x1 == 0) & 9
		// x1 = x1 & andv
		// x1h = h(x1)
		// x1l = l(x1)
		// x1h += h(mulval) * x0h + x0h
		// x1l += l(mulval) * x0l + x0l
		
		// now moving final h and l sums into nums
		// nums[0] = l(x0l)
		// nums[8] = h(x0h)
		
		vec2_ty x0 = and8(buf.vecs[0],n48);
		
		vec2_ty x0h = h(x0);
		vec2_ty x0l = l(x0);
		
		for(int i = 1; i < 5; i++){
			vec2_ty x1 = buf.vecs[i];
			vec2_ty mulval = and8(xor8(chars_eq(x1,zero),all_bits),nine);
			x1 = and8(x1,n48);
			vec2_ty x1h = h(x1);
			vec2_ty x1l = l(x1);
			x0h = add16(x1h,add16(mul16(h(mulval),x0h),x0h));
			x0l = add16(x1l,add16(mul16(l(mulval),x0l),x0l));
		}
		store_16_into_nums(nums+ll,x0l,x0h);
		/*printf("%d\t\t%d\n",ll+0,nums[ll+0]);
		printf("%d\t\t%d\n",ll+5,nums[ll+5]);
		printf("%d\t\t%d\n",ll+16,nums[ll+16]);
		printf("%d\t\t%d\n",ll+24,nums[ll+24]);
		printf("%d\t\t%d\n",ll+32,nums[ll+32]);
		printf("%d\t\t%d\n",ll+48,nums[ll+48]);
		fflush(stdout);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));*/
	}
	for(;ll < nlines; ll++){
		nums[ll] = atoi(lines[ll]);
	}
}











//
