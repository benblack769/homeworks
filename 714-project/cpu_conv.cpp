#include <algorithm>
#include <cassert>
#include "omp.h"
#include <cstdio>
#include <chrono>
#include "convolve_help.h"


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
    auto start = std::chrono::system_clock::now();
    assert(in_channels%group_size == 0 && out_channels%group_size == 0);
    assert(kern_width%2 == 1 && kern_height%2 == 1);
    int out_group_size = out_channels/group_size;
    int in_group_size = in_channels/group_size;
#pragma omp parallel for collapse(3)
    // Each batch is independent. A single element in the batch corresponds to an image.
    for(int b = 0; b < batch_size; b++){
        // Each group corresponds to a kernel, could be potentially parallelized along group_size.
        for(int g = 0; g < group_size; g++){
            // Processing per iteration.
            for(int ocg = 0; ocg < out_group_size; ocg++){
                convolve_help(
                    in_image,
                    kernel,
                    out_image,
                    batch_size,
                    in_channels,
                    out_channels,
                    kern_width,
                    kern_height,
                    image_width,
                    image_height,
                    group_size,
                    b,
                    g,
                    ocg,
                    out_group_size,
                    in_group_size
                );
            }
        }
    }
    std::chrono::duration<double> wctduration = (std::chrono::system_clock ::now() - start);

    return wctduration.count();
}
