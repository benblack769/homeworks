/*
Tests given implementation against gold standard implmentation
*/

#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include "omp.h"
#include <ctime>
#include <chrono>
#include "conv.h"
#include "gold_conv.h"

void fill_random(std::vector<float> & vec){
    for(float & e : vec){
        e = rand()/double(RAND_MAX);
    }
}
void perf_test_conv(
    int batch_size,
    int in_channels,
    int out_channels,
    int kern_width,
    int kern_height,
    int image_width,
    int image_height,
    int group_size)
{
    std::vector<float> input_img(batch_size*in_channels*image_width*image_height);
    std::vector<float> filter(out_channels*(in_channels/group_size)*kern_height*kern_width);
    std::vector<float> out_img_gold(batch_size*out_channels*image_width*image_height);
    std::vector<float> out_img_test(batch_size*out_channels*image_width*image_height);
    fill_random(input_img);
    fill_random(filter);
    double tot_time = 0;
    uint64_t FLO = (uint64_t)batch_size*image_width*image_height*kern_width*kern_height*group_size*(in_channels/group_size)*(out_channels/group_size);
    uint64_t num_iters = 10;
    for(int i = 0; i < num_iters; i++){
        tot_time += convolve(
            &input_img[0],
            &filter[0],
            &out_img_test[0],
            batch_size,
            in_channels,
            out_channels,
            kern_width,
            kern_height,
            image_width,
            image_height,
            group_size
        );
    }
    uint64_t total_ops = FLO * num_iters;
    // end += 1; //to handle dividie by zero issues
    printf("%f\n", tot_time/num_iters);
    //printf("TIME: %f\t\tFLOPS: %f\n",tot_time/num_iters,total_ops/tot_time);
}
int main(){
    srand(1);
    printf("OpenMP has %d threads\n",omp_get_max_threads());
    // (batch_size, input_channels, output_channels, kernel_width, kernel_height, image_width, image_height, group_size).
    perf_test_conv(16,128,128,3,3,64,64,1);
    perf_test_conv(16,128,128,3,3,64,64,2);
    perf_test_conv(16,128,128,3,3,64,64,4);
    perf_test_conv(16,128,128,3,3,64,64,8);
    perf_test_conv(16,128,128,3,3,64,64,16);
    perf_test_conv(16,128,128,3,3,64,64,32);
    perf_test_conv(16,128,128,3,3,64,64,64);
    perf_test_conv(16,128,128,3,3,64,64,128);
    return 0;
}
