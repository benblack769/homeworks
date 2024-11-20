#include <algorithm>
#include <cassert>
void convolve_gold(
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
    for(int b = 0; b < batch_size; b++){
        for(int g = 0; g < group_size; g++){
            for(int oc = g*out_group_size; oc < (g+1)*out_group_size; oc++){
                // Initialize the output image.
                for(int y = 0; y < image_height; y++){
                    for(int x = 0; x < image_width; x++){
                        out_image[((b*out_channels+oc)*image_height+y)*image_width+x] = 0;
                    }
                }
                for(int ic = g*in_group_size; ic < (g+1)*in_group_size; ic++){
                    int kic = ic - g*in_group_size;
                    for(int fy = -kern_height/2; fy <= kern_height/2; fy++){
                        for(int fx = -kern_width/2; fx <= kern_width/2; fx++){
                            for(int oy = 0; oy < image_height; oy++){
                                for(int ox = 0; ox < image_width; ox++){
                                    int iy = oy-fy;
                                    int ix = ox-fx;
                                    if(iy >= 0 && iy < image_height && ix >= 0 && ix < image_width){
                                        int ky = fy + kern_height/2;
                                        int kx = fx + kern_width/2;
                                        // The kernel index is meant to calculate the new output image.
                                        // The output image is directly assigned in the same line.
                                        out_image[((b*out_channels+oc)*image_height+oy)*image_width+ox] += kernel[((oc*in_group_size+kic)*kern_height+ky)*kern_width+kx] * in_image[((b*in_channels+ic)*image_height+iy)*image_width+ix];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
