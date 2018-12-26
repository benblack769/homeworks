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
	return _mm256_store_si256(reinterpret_cast<vec_ty *>(loc),value);
}
inline vec_ty make_zero(){
	return _mm256_setzero_si256();
}
inline vec_ty set_shorts(short s){
	return _mm256_set1_epi16(s);
}
inline vec_ty set_chars(char c){
	return _mm256_set1_epi8(c);
}
inline vec_ty andvalues(vec_ty a,vec_ty b){
	return _mm256_and_si256(a,b);
}
inline vec_ty xor8(vec_ty a,vec_ty b){
	return _mm256_xor_si256(a,b);
}
inline vec_ty chars_eq(vec_ty a,vec_ty b){
	return _mm256_cmpeq_epi8(a,b);
}
inline vec_ty shorts_eq(vec_ty a,vec_ty b){
	return _mm256_cmpeq_epi16(a,b);
}
inline vec_ty shiftleft(vec_ty a,uint8_t shiftnum){
	return _mm256_slli_epi16(a,shiftnum);
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
vec_ty highscreen = set_shorts(0xff);
inline vec_ty h(vec_ty x){
	return _mm256_srli_epi16(x,8);
}
inline vec_ty l(vec_ty x){
	return andvalues(x,highscreen);
}
inline vec_ty h16(vec_ty x){
	return cnv16_32(hbits(x));
}
inline vec_ty l16(vec_ty x){
	return cnv16_32(lbits(x));
}

constexpr size_t VEC_SIZE = 16;
constexpr size_t NUM_VECS = 1;
constexpr size_t BUFWIDTH = VEC_SIZE * NUM_VECS;
constexpr uint8_t STR_MV_UNROLL_FACTOR = 2;
constexpr size_t VEC_32_SIZE = VEC_SIZE / 2;
constexpr size_t VEC_16_SIZE = VEC_SIZE;

constexpr size_t MAX_STR_SIZE = 5;

constexpr inline int square(int x){
	return x * x;
}
constexpr inline uint16_t ipow(uint16_t x,uint16_t y){
	return y == 0 ? 1 : (y%2 ? x*ipow(x,y-1) : square(ipow(x,y/2)));
}
#define mul10(vec) add16(shiftleft(vec,3),shiftleft(vec,1))
struct twovecs{
	vec_ty lv;
	vec_ty hv;
};
/*
template<const int n>
twovecs calc_vecmul(vec_ty * vecs){
	constexpr uint16_t p = ipow(10,n-1);
	vec_ty n48 = set_chars(~char(48));
	vec_ty mulval = _mm256_set1_epi16(p);
	vec_ty x1 = andvalues(*vecs,n48);
	vec_ty x1l = l(x1);
	vec_ty x1h = h(x1);
	x1l = mul16(x1l,mulval);
	x1h = mul16(x1h,mulval);
	twovecs tvs = calc_vecmul<n-1>(vecs+1);
	return twovecs{add16(tvs.lv,x1l),add16(tvs.hv,x1h)};
}
template<>
twovecs calc_vecmul<1>(vec_ty *vecs){
	vec_ty n48 = set_chars(~char(48));
	vec_ty x1 = andvalues(*vecs,n48);
	return twovecs{l(x1),h(x1)};
}*/
twovecs calc_vecmul4(vec_ty * vecs){
	vec_ty n48 = set_chars(~char(48));
	//vec_ty p4 = set_shorts(ipow(10,3));
	//vec_ty p3 = set_shorts(ipow(10,2));
	//vec_ty p2 = set_shorts(ipow(10,1));
	vec_ty x4 = andvalues(vecs[0],n48);
	vec_ty x3 = andvalues(vecs[1],n48);
	vec_ty x2 = andvalues(vecs[2],n48);
	vec_ty x1 = andvalues(vecs[3],n48);
	vec_ty x4l = l(x4);
	vec_ty x4h = h(x4);
	vec_ty x3l = l(x3);
	vec_ty x3h = h(x3);
	vec_ty x2l = l(x2);
	vec_ty x2h = h(x2);
	vec_ty x1l = l(x1);
	vec_ty x1h = h(x1);
	vec_ty suml = (x4l);
	vec_ty sumh = (x4h);
	suml = add16(mul10(suml),(x3l));
	sumh = add16(mul10(sumh),(x3h));
	suml = add16(mul10(suml),(x2l));
	sumh = add16(mul10(sumh),(x2h));
	suml = add16(mul10(suml),x1l);
	sumh = add16(mul10(sumh),x1h);
	return twovecs{suml,sumh};
}
/*
twovecs calc_vecmul(int n,vec_ty * vecs){
    vec_ty n48 = set_chars(~char(48));
    vec_ty initnums = andvalues(vecs[0],n48);
    vec_ty lo = l(initnums);
    vec_ty hi = h(initnums);
    for(int i = 1; i < n; i++){
    	vec_ty nums = andvalues(vecs[i],n48);
        vec_ty nlo = l(nums);
        vec_ty nhi = h(nums);
        lo = add16(add16(shiftleft(lo,3),shiftleft(lo,1)),nlo);
        hi = add16(add16(shiftleft(hi,3),shiftleft(hi,1)),nhi);
    }
    return twovecs{lo,hi};
}*/

union {
	uint16_t shorts[MAX_STR_SIZE][BUFWIDTH];
	vec_ty vecs[MAX_STR_SIZE];
}buf;

void convert_all(unsigned nlines, char *lines[], quote_t nums[]){
	size_t ll = 0;
	uintptr_t aligned_nums = ((uintptr_t)nums+31) & ~ (uintptr_t)0x1F;
	for(;ll < (aligned_nums - uintptr_t(nums)) / sizeof(quote_t); ll++){
		nums[ll] = atoi(lines[ll]);
	}
	vec_ty n48 = set_shorts(0x00cf);
	vec_ty nine = set_shorts(9);
	vec_ty zero = make_zero();
	vec_ty all_bits = set_chars(0xff);
	for(; ll <= nlines-BUFWIDTH;ll+=BUFWIDTH){
		char ** clines = lines + ll;
		#define linesets(n) _mm256_set_epi16(clines[15][n],clines[14][n],clines[13][n],clines[12][n],clines[11][n],clines[10][n],clines[9][n],clines[8][n],clines[7][n],clines[6][n],clines[5][n],clines[4][n],clines[3][n],clines[2][n],clines[1][n],clines[0][n])
		vec_ty v0 = linesets(0);
		vec_ty v1 = linesets(1);
		vec_ty v2 = linesets(2);
		vec_ty v3 = linesets(3);
		vec_ty v4 = linesets(4);
		//simple formula
		// x1 = x1 & andv + x1 == 0 ? x0 : x0 * 10

		//without brancheslines
		// x1 = x1 & andv + (~(x1 == 0) & 9) * x0 + x0

		// with types incorperated
		// mulval = ~(x1 == lines0) & 9
		// x1 = x1 & andv
		// x1h = h(x1)
		// x1l = l(x1)
		// x1h += h(mulval) * x0h + x0h
		// x1l += l(mulval) * x0l + x0l

		// now moving final h and l sums into nums
		// nums[0] = l(x0l)
		// nums[8] = h(x0h)

		v0 = andvalues(v0,n48);
		v1 = andvalues(v1,n48);
		v2 = andvalues(v2,n48);
		vec_ty sum = v0;
		sum = add16(mul10(sum),(v1));
		sum = add16(mul10(sum),(v2));
		#define get_mul_val(vec) andvalues(xor8(shorts_eq(vec,zero),all_bits),nine)
		/*vec_ty mulval3 = get_mul_val(v3);
		v3 = andvalues(v3,n48);
		sum = add16(v3,add16(sum,mul16(sum,mulval3)));

		//if(_mm256_testz_si256(zero,v4)){
			vec_ty mulval4 = get_mul_val(v4);
			v4 = andvalues(v4,n48);
			sum = add16(v4,add16(sum,mul16(sum,mulval4)));*/
		//}
/*		sum = add16(sum,add16(mul16(mulval3,mulval4)))
			for(uint64_t i = 1; i < 5; i++){
				vec_ty x1 = buf.vecs[i];
				vec_ty mulval = andvalues(xor8(chars_eq(x1,zero),all_bits),nine);
				x1 = andvalues(x1,n48);
				vec_ty x1h = h(x1);
				vec_ty x1l = l(x1);
				x0h = add16(x1h,add16(mul16(h(mulval),x0h),x0h));
				x0l = add16(x1l,add16(mul16(l(mulval),x0l),x0l));
*/

		aligned_store(nums+ll+VEC_32_SIZE*0,l16(sum));
		aligned_store(nums+ll+VEC_32_SIZE*1,h16(sum));
	}
	for(;ll < nlines; ll++){
		nums[ll] = atoi(lines[ll]);
	}
}










//
