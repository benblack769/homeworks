#include <iostream>
#include <cstdint>
#include <random>
#include <vector>
#include <ctime>

using Float = float;
using fvector = std::vector<Float>;

Float sum(fvector & arr){
	Float res = 0;
	for (Float & n : arr)
		res += n;
	return res;
}
void __attribute__ ((noinline)) update_coords(fvector & x,fvector & y,fvector & z,fvector & vx, fvector & vy, fvector & vz){
	size_t size = x.size();
	
	Float * px = x.data();
	Float * py = y.data();
	Float * pz = z.data();
	Float * pvx = vx.data();
	Float * pvy = vy.data();
	Float * pvz = vz.data();	
	
	#define add_two_nums(src1,dest) \
	"movss (%[" dest "]),%%xmm0\n" 	\
	"addss (%[" src1 "]),%%xmm0\n"	\
	"movss %%xmm0,(%[" dest "])\n" 	\
	"addq $4,%[" dest "]\n"	\
	"addq $4,%[" src1 "]\n"	
	
	asm volatile(
	"movq $16, %%rax\n"
	
	"jmp vec_loop_cmp\n"
	"vec_loop_start:\n"
	
	"vmovups   (%[px]),	%%ymm0\n"
	"vmovups 32(%[px]),	%%ymm1\n"
	"vmovups   (%[py]),	%%ymm2\n"
	"vmovups 32(%[py]),	%%ymm3\n"
	"vmovups   (%[pz]),	%%ymm4\n"
	"vmovups 32(%[pz]),	%%ymm5\n"
	
	"vaddps    (%[pvx]),%%ymm0,%%ymm0\n"
	"vaddps  32(%[pvx]),%%ymm1,%%ymm1\n"
	"vaddps    (%[pvy]),%%ymm2,%%ymm2\n"
	"vaddps  32(%[pvy]),%%ymm3,%%ymm3\n"
	"vaddps    (%[pvz]),%%ymm4,%%ymm4\n"
	"vaddps  32(%[pvz]),%%ymm5,%%ymm5\n"
	
	"vmovups %%ymm0,   (%[px])\n"
	"vmovups %%ymm1, 32(%[px])\n"
	"vmovups %%ymm2,   (%[py])\n"
	"vmovups %%ymm3, 32(%[py])\n"
	"vmovups %%ymm4,   (%[pz])\n"
	"vmovups %%ymm5, 32(%[pz])\n"
	
	"addq $16,%%rax\n"
	
	"addq $64, %[px]\n"
	"addq $64, %[py]\n"
	"addq $64, %[pz]\n"
	"addq $64, %[pvx]\n"
	"addq $64, %[pvy]\n"
	"addq $64, %[pvz]\n"
	
	"vec_loop_cmp:\n"
	"cmpq %[size],%%rax\n"
	"jl vec_loop_start\n"
	
	"subq $16, %%rax\n"
	
	
	"jmp loop_cmp\n"
	"loop_start:\n"
	
	add_two_nums("pvx","px")
	add_two_nums("pvy","py")
	add_two_nums("pvz","pz")
	
	"incq %%rax\n"
	"loop_cmp:\n"
	"cmpq %[size],%%rax\n"
	"jl loop_start\n"
	:
	:[size] "rm" (size),[px] "r" (px) , [py] "r" (py),[pz] "r" (pz),[pvx] "r" (pvx),[pvy] "r" (pvy),[pvz] "r" (pvz)
	:"rax","ymm0","ymm1","ymm2","ymm3","ymm4","ymm5");
	#undef add_two_nums
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
	
	size_t startt = clock();
	for(size_t iter = 0; iter < iters; iter++)
		update_coords(x,y,z,vx,vy,vz);
	double time = (clock() - startt) / (double)(CLOCKS_PER_SEC);
	
	//Float chksum = sum(x) + sum(y) + sum(z);
	std::cout << std::scientific << (1000000 * time / (size * iters)) << '\n';
	//std::cout << "Mean time per coordinate: " << (1000000 * time / (size * iters)) << "us\n"; 
	//std::cout << "Final checksum is: " << chksum << "\n";
	
	return 0;
}
