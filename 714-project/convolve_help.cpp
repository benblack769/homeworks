#include <algorithm>

void convolve_help(
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
    int group_size,
    int b,
    int g,
    int ocg,
    int out_group_size,
    int in_group_size
){
    int oc = ocg + g*out_group_size;
    // printf("%d\n",omp_get_num_threads());
    // Initialization.
    for(int y = 0; y < image_height; y++){
        for(int x = 0; x < image_width; x++){
            out_image[((b*out_channels+oc)*image_height+y)*image_width+x] = 0;
        }
    }
    for(int ic = g*in_group_size; ic < (g+1)*in_group_size; ic++){
        int kic = ic - g*in_group_size;
        for(int fy = -kern_height/2; fy <= kern_height/2; fy++){
            for(int oy = std::max(0,fy); oy < std::min(image_height,image_height+fy); oy++){
                int ky = fy + kern_height/2;
                for(int fx = -kern_width/2; fx <= kern_width/2; fx++){
                    int kx = fx + kern_width/2;
                    float kernel_element = kernel[((oc*in_group_size+kic)*kern_height+ky)*kern_width+kx];
                    for(int ox = std::max(0,fx); ox < std::min(image_width,image_width+fx); ox++){
                        int iy = oy-fy;
                        int ix = ox-fx;
                        out_image[((b*out_channels+oc)*image_height+oy)*image_width+ox] +=  kernel_element * in_image[((b*in_channels+ic)*image_height+iy)*image_width+ix];
                    }
                }
            }
        }
    }
}
