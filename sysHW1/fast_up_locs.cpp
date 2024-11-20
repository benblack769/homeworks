#include <iostream>
#include "intrinsic_help.h"
#include <inttypes.h>
#include <random>
#include <vector>
#include <ctime>

#ifdef float32
constexpr uint64_t num_regs = 2;
constexpr uint64_t size_reg = 8;
using myfvec = fvec<fvec8,num_regs>;
using Float = float;
#else
constexpr uint64_t num_regs = 2;
constexpr uint64_t size_reg = 4;
using myfvec = fvec<dvec4,num_regs>;
using Float = double;
#endif
constexpr uint64_t block_size = num_regs * size_reg;
using fvector = std::vector<Float>;

Float sum(fvector & arr){
	Float res = 0;
	for (Float & n : arr)
		res += n;
	return res;
}
/*void add_vel(int size,fvector & pos, fvector & vel){
	int i = 0;
	for(;i < size; i += block_size){
		Float * posptr = &pos[i];
		myfvec posblock(posptr);
		posblock += myfvec(&vel[i]);
		posblock.store(posptr);
	}
	for(; i < size; i++){
		pos[i] += vel[i];
	}
}*/
void add_blocks(Float * posvec,Float * velvec){
	myfvec block(posvec); 
	block += myfvec(velvec); 
	block.store(posvec);
}
void update_coords(int size,fvector & x,fvector & y,fvector & z,fvector & vx, fvector & vy, fvector & vz){
	/*add_vel(size,x,vx);
	add_vel(size,y,vy);
	add_vel(size,z,vz);
	*/
	int i = 0;
	for(;i < size + block_size; i += block_size){
		//#define do_stuff(posvec,velvec) { Float * ptr = &posvec[i]; myfvec block(ptr); block += myfvec(&velvec[i]); block.store(ptr);}
		add_blocks(&x[i],&vx[i]);
		add_blocks(&y[i],&vy[i]);
		add_blocks(&z[i],&vz[i]);
	}
	for(; i < size; i++){
		x[i] += vx[i];
		y[i] += vy[i];
		z[i] += vz[i];
	}
}
fvector gen_rand_vec(int size,std::default_random_engine randeng,std::uniform_real_distribution<Float> dist){
	fvector rvec(size);
	for (Float & f : rvec)
		f = dist(randeng);
	return rvec;
}
int main(int argc,char ** args){
	if(argc != 3){
		std::cout << "Required arguments: vector_length(N) and iterations_num(M)\n";
		return -1;
	}
	uint64_t size = std::stoi(args[1]);
	uint64_t iters = std::stoi(args[2]);
	
	std::default_random_engine re(1);
	std::uniform_real_distribution<Float> point_dist(0,1000.0);
	std::uniform_real_distribution<Float> veloc_dist(0,1.0);
	
	fvector x = gen_rand_vec(size,re,point_dist);
	fvector y = gen_rand_vec(size,re,point_dist);
	fvector z = gen_rand_vec(size,re,point_dist);
	
	fvector vx = gen_rand_vec(size,re,veloc_dist);
	fvector vy = gen_rand_vec(size,re,veloc_dist);
	fvector vz = gen_rand_vec(size,re,veloc_dist);
	
	int startt = clock();
	for(int iter = 0; iter < iters; iter++)
		update_coords(size,x,y,z,vx,vy,vz);
	double time = (clock() - startt) / (double)(CLOCKS_PER_SEC);
	
	Float chksum = sum(x) + sum(y) + sum(z);
	
	std::cout << "Mean time per coordinate: " << (1000000 * time / (size * iters)) << "us\n"; 
	std::cout << "Final checksum is: " << chksum << "\n";
	
	return 0;
}