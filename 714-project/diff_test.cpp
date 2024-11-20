/*
Tests given implementation against gold standard implmentation
*/

#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include "conv.h"
#include "gold_conv.h"

void fill_random(std::vector<float> & vec){
    for(float & e : vec){
        e = rand()/double(RAND_MAX);
    }
}
bool are_similar(float a, float b){
    float rel_tol=1.e-1;
    float abs_tol=1e1;
    return fabsf(a-b) <= std::max(rel_tol * std::max(fabsf(a), fabsf(b)), abs_tol);
}
int count_neq(std::vector<float> & b1, std::vector<float> & b2){
    int neq_count = 0;
    assert(b1.size() == b2.size());
    for(int i = 0; i < b1.size(); i++){
        if(!are_similar(b1[i], b2[i])){
            std::cout << b1[i] << "\t\t" << b2[i] << '\n';
            neq_count += 1;
        }
    }
    return neq_count;
}
bool diff_test_conv(
    int batch_size,
    int in_channels,
    int out_channels,
    int kern_width,
    int kern_height,
    int image_width,
    int image_height,
    int group_size)
{
    srand(42);
    std::vector<float> input_img(batch_size*in_channels*image_width*image_height);
    std::vector<float> filter(out_channels*(in_channels/group_size)*kern_height*kern_width);
    std::vector<float> out_img_gold(batch_size*out_channels*image_width*image_height);
    std::vector<float> out_img_test(batch_size*out_channels*image_width*image_height);
    fill_random(input_img);
    fill_random(filter);
    fill_random(out_img_gold);//just in case...
    fill_random(out_img_test);//just in case...
    printf("FLO: %d\n",batch_size*image_width*image_height*kern_width*kern_height*(in_channels/group_size)*(out_channels/group_size));
    fflush(stdout);
    convolve_gold(
        &input_img[0],
        &filter[0],
        &out_img_gold[0],
        batch_size,
        in_channels,
        out_channels,
        kern_width,
        kern_height,
        image_width,
        image_height,
        group_size
    );
    convolve(
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
    int out_size = batch_size*image_width*image_height*out_channels;
    // for(int b = 0; b < batch_size; b++){
    //     for(int oc = 0; oc < out_channels; oc++){
    //         for(int oy = 0; oy < image_height; oy++){
    //             for(int ox = 0; ox < image_width; ox++){
    //                 float out_v = out_img_gold[((b*out_channels+oc)*image_height+oy)*image_width+ox];
    //                 int out_idx = std::find_if(out_img_test.begin(), out_img_test.end(), [&](float v){return are_similar(v, out_v);})  - out_img_test.begin();
    //                 if(out_idx < 0 || out_idx >= out_size){
    //                     continue;
    //                 }
    //                 int tx = out_idx%image_width;
    //                 out_idx /=  image_width;
    //                 int ty = out_idx%image_height;
    //                 out_idx /=  image_height;
    //                 int tc = out_idx%out_channels;
    //                 out_idx /=  out_channels;
    //                 int tb = out_idx%batch_size;
    //                 out_idx /=  batch_size;
    //                 assert(out_idx == 0);
    //                 printf("(%d,%d,%d,%d) -> (%d,%d,%d,%d)\n", b,oc,oy,ox,tb,tc,ty,tx);
    //             }
    //         }
    //     }
    // }

    printf("neq count: %d \ttot count: %d\n", count_neq(out_img_gold, out_img_test), out_size);
    fflush(stdout);
    return std::equal(out_img_gold.begin(), out_img_gold.end(), out_img_test.begin(), are_similar);
}
int main(){
    srand(1);

    assert(diff_test_conv(2,128,128,3,3,64,64,1));
    assert(diff_test_conv(2,128,128,3,3,64,64,2));
    assert(diff_test_conv(2,128,128,3,3,64,64,4));
    assert(diff_test_conv(2,128,128,3,3,64,64,8));
    assert(diff_test_conv(2,128,128,3,3,64,64,16));
}
