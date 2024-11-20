#include <algorithm>
#include <cassert>
#include <omp.h>
#include <cstdio>
#include <chrono>
#include <memory>
#include <iostream>
#include "convolve_help.h"
#include "fvec.hpp"

class aligned_vecf{
public:
    static constexpr size_t alignment = 32;
    std::unique_ptr<float[]> buf;
    size_t mysize;
    aligned_vecf(size_t size_requested):
        buf(new float[size_requested+alignment*2]),
        mysize(size_requested){}
    float * data(){
        return (float*)((uintptr_t)(((char*)(buf.get()))+alignment) & ~(uintptr_t)(alignment-1));
    }
    size_t size(){
        return mysize;
    }
};

void transpose_image(float * in_img, float * out_img, int batch_size, int channels, int image_width, int image_height, int group_size){
    assert(channels%group_size == 0);
    int channel_group_size = channels/group_size;

    for(int b = 0; b < batch_size; b++){
        for(int g = 0; g < group_size; g++){
            for(int y = 0; y < image_height; y++){
                for(int x = 0; x < image_width; x++){
                    for(int c = 0; c < channel_group_size; c++){
                        out_img[(((b*group_size+g)*image_height+y)*image_width+x)*channel_group_size+c] = in_img[(((b*group_size+g)*channel_group_size+c)*image_height+y)*image_width+x];
                    }
                }
            }
        }
    }
}

void inv_transpose_image(float * in_img, float * out_img, int batch_size, int channels, int image_width, int image_height, int group_size){
    assert(channels%group_size == 0);
    int channel_group_size = channels/group_size;

    for(int b = 0; b < batch_size; b++){
        for(int g = 0; g < group_size; g++){
            for(int y = 0; y < image_height; y++){
                for(int x = 0; x < image_width; x++){
                    for(int c = 0; c < channel_group_size; c++){
                        out_img[(((b*group_size+g)*channel_group_size+c)*image_height+y)*image_width+x] = in_img[(((b*group_size+g)*image_height+y)*image_width+x)*channel_group_size+c];
                    }
                }
            }
        }
    }
}

void transpose_kernel(float * in_kern, float * out_kern, int in_channels, int out_channels, int kern_width, int kern_height, int group_size, int outs_per_group){
    assert(in_channels%group_size == 0);
    int eff_group_size_out = group_size*outs_per_group;
    assert(out_channels % eff_group_size_out == 0);
    int eff_out_group_size = out_channels / eff_group_size_out;

    int out_group_size = out_channels/group_size;
    int in_group_size = in_channels/group_size;

    for(int g = 0; g < group_size; g++){
        for(int og = 0; og < outs_per_group; og++){
            for(int oc = 0; oc < eff_out_group_size; oc++){
                for(int ic = 0; ic < in_group_size; ic++){
                    for(int ky = 0; ky < kern_height; ky++){
                        for(int kx = 0; kx < kern_width; kx++){
                            out_kern[((((g*outs_per_group+og)*kern_height+ky)*kern_width+kx)*in_group_size+ic)*eff_out_group_size+oc] = in_kern[((((g*outs_per_group+og)*eff_out_group_size+oc)*in_group_size+ic)*kern_height+ky)*kern_width+kx];
                        }
                    }
                }
            }
        }
    }
}
template<int vec_size>
struct vec_type_help{ using vtype = typename vec_type_help<vec_size-1>::vtype; };
template<>
struct vec_type_help<8>{ using vtype = fvec8; };
template<>
struct vec_type_help<4>{ using vtype = fvec4; };
template<>
struct vec_type_help<1>{ using vtype = fvec1; };


template<int n_channels>
void convolve_help_static(float * in_buf, float * filter, float * out_buf, int in_channels){
    // static_assert(kern_size%2 == 1, "kern_size must be odd");
    constexpr int width = 64;
    constexpr int height = 64;
    constexpr int kern_size = 3;
    // constexpr int in_channels = n_channels;
    constexpr int out_channels = n_channels;

    constexpr int out_group_s = std::min(32, n_channels);
    assert((size_t)in_buf % out_group_s == 0);
    assert((size_t)filter % out_group_s == 0);
    assert((size_t)out_buf % out_group_s == 0);

    constexpr int stride = 1;
    constexpr int hkern = kern_size / 2;
    constexpr int hokern = (kern_size-1) / 2;
    constexpr int oheight = height/stride;
    constexpr int owidth = width/stride;

    using v_small = typename vec_type_help<n_channels>::vtype;
    using v_group = fvec<v_small, out_group_s/v_small::size()>;
    // static_assert(out_channels % out_group_s == 0, "out_channels must be divisible by 32");
    // std::fill(out_buf, out_buf+owidth*oheight*out_channels, 0.f);
    constexpr int max_tile_size = 4;
    constexpr int max_in_group_size = std::min(32, n_channels);
    for(int og = 0; og < out_channels; og += out_group_s){
        for(int ing = 0; ing < in_channels; ing += max_in_group_size){
            int max_in_group = ing+max_in_group_size;
            for(int oyt = 0; oyt < oheight; oyt += max_tile_size){
                for(int oxt = 0; oxt < owidth; oxt += max_tile_size){
                    int tile_size_y = std::min(max_tile_size, oheight - oyt);
                    int tile_size_x = std::min(max_tile_size, owidth - oxt);
                    for(int oy = oyt; oy < oyt + tile_size_y; oy++){
                        for(int ox = oxt; ox < oxt + tile_size_x; ox++){
                            v_group out_accum(&out_buf[(oy*owidth + ox)*out_channels+og]);
                            for(int offy = -hokern; offy <= hkern; offy++){
                                for(int offx = -hokern; offx <= hkern; offx++){
                                    int ky = offy + hokern;
                                    int kx = offx + hokern;
                                    int iy = oy * stride + offy;
                                    int ix = ox * stride + offx;
                                    if (iy >= 0 && iy < height && ix >= 0 && ix < width){
                                        for(int ic = ing; ic < max_in_group; ic++){
                                            v_small in_v(in_buf[(iy*width + ix)*in_channels+ic]);
                                            v_group filt_g(&filter[((ky*kern_size+kx)*in_channels+ic)*out_channels+og]);
                                            out_accum = fma(filt_g, in_v, out_accum);
                                        }
                                    }
                                }
                            }
                            out_accum.store(&out_buf[(oy*owidth + ox)*out_channels+og]);
                        }
                    }
                }
            }
        }
    }
}

void conv_help(
    float * in_image,
    float * kernel,
    float * out_image,
    int in_channels,
    int out_channels,
    int kern_width,
    int kern_height,
    int image_width,
    int image_height
){
    /*
    now just a regular convolution, not a grouped convolution. Not batched either
    */
    switch (out_channels) {
        case 1: return convolve_help_static<1>(in_image, kernel, out_image, in_channels);
        case 2: return convolve_help_static<2>(in_image, kernel, out_image, in_channels);
        case 4: return convolve_help_static<4>(in_image, kernel, out_image, in_channels);
        case 8: return convolve_help_static<8>(in_image, kernel, out_image, in_channels);
        case 16: return convolve_help_static<16>(in_image, kernel, out_image, in_channels);
        case 32: return convolve_help_static<32>(in_image, kernel, out_image, in_channels);
        case 64: return convolve_help_static<64>(in_image, kernel, out_image, in_channels);
        case 128: return convolve_help_static<128>(in_image, kernel, out_image, in_channels);
        case 256: return convolve_help_static<256>(in_image, kernel, out_image, in_channels);
        default: assert(false && "convolution only supports powers of 2 up to 256 channels");
    }
}
double convolve(
    float * in_image,
    float * kernel,
    float * out_image,
    int batch_size,
    int in_channels,
    int out_channels,
    int kern_width,
    int kern_height,
    int image_width,
    int image_height,
    int group_size
){
    assert(in_channels%group_size == 0 && out_channels%group_size == 0);
    assert(kern_width%2 == 1 && kern_height%2 == 1);
    int out_group_size = out_channels/group_size;
    int in_group_size = in_channels/group_size;

    // allocates data for the transposition and performs the transposition
    aligned_vecf trans_buffer_store_in(batch_size*in_channels*image_height*image_width);
    aligned_vecf trans_buffer_store_out(batch_size*out_channels*image_height*image_width);
    aligned_vecf trans_buffer_store_kernel(group_size*(in_channels/group_size)*(out_channels/group_size)*kern_width*kern_height);
    float * transposed_img_in = trans_buffer_store_in.data();
    float * transposed_img_out = trans_buffer_store_out.data();
    float * transposed_kernel = trans_buffer_store_kernel.data();

    int outs_per_group = std::max(group_size, 4) / group_size;
    int eff_group_size_out = group_size*outs_per_group;
    assert(out_channels % eff_group_size_out == 0);
    int eff_out_group_size = out_channels / eff_group_size_out;

    //transposes are in the timing, because they will have to be done every time-- the cost of a bad format
    transpose_kernel(kernel, transposed_kernel, in_channels,out_channels,kern_width,kern_height,group_size,outs_per_group);
    transpose_image(in_image, transposed_img_in, batch_size,in_channels,image_width,image_height,group_size);
    std::fill(transposed_img_out, transposed_img_out+batch_size*out_channels*image_height*image_width, 0.f);
    auto start = std::chrono::system_clock::now();


    // Each batch is independent. A single element in the batch corresponds to an image.
#pragma omp parallel for collapse(3)
    for(int b = 0; b < batch_size; b++){
        // Each group corresponds to a kernel, could be potentially parallelized along group_size.
        for(int g = 0; g < group_size; g++){
            for(int og = 0; og < outs_per_group; og++){
                conv_help(
                    &transposed_img_in[(b*group_size+g)*image_height*image_width*in_group_size],
                    &transposed_kernel[(g*outs_per_group+og)*eff_out_group_size*in_group_size*kern_width*kern_height],
                    &transposed_img_out[((b*group_size+g)*outs_per_group+og)*eff_out_group_size*image_height*image_width],
                    in_group_size,
                    eff_out_group_size,
                    kern_width,
                    kern_height,
                    image_width,
                    image_height
                );
            }
        }
    }
    std::chrono::duration<double> wctduration = (std::chrono::system_clock ::now() - start);
    inv_transpose_image(transposed_img_out, out_image, batch_size,out_channels,image_width,image_height,eff_group_size_out);


    return wctduration.count();
}
