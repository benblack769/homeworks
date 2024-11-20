kernel void pad_image(
    global float * in_image,
    global float * padded_img,
    global int * params
){
    const int in_channels = params[1];
    const int kern_width = params[3];
    const int kern_height = params[4];


    const int b = get_global_id(0);
    const int x = get_global_id(1);
    const int y = get_global_id(2);

    const int batch_size = get_global_size(0);
    const int image_width = get_global_size(1);
    const int image_height = get_global_size(2);

    const int pad_width = kern_width/2;
    const int pad_height = kern_height/2;

    const int padded_img_width = image_width+pad_width*2;
    const int padded_img_height = image_height+pad_height*2;

    for(int ic = 0; ic < in_channels; ic++){
        padded_img[((b*in_channels+ic)*padded_img_height+y+pad_height)*padded_img_width+x+pad_width] = in_image[((b*in_channels+ic)*image_height+y)*image_width+x];
    }
}
#define max_channel_block_size 16

kernel void compute_conv(
        global float * padded_img,
        global float * kern,
        global float * out_img,
        global int * params
    ){
    const int group_size = params[0];
    const int in_channels = params[1];
    const int out_channels = params[2];
    const int kern_width = params[3];
    const int kern_height = params[4];
    const int batch_size = params[5];

    const int image_height = get_global_size(1);
    const int image_width = get_global_size(2);

    const int in_group_size = in_channels/group_size;
    const int out_group_size = out_channels/group_size;

    const int padded_img_width = image_width+kern_width/2;
    const int padded_img_height = image_height+kern_height/2;

    const int b = get_global_id(0);
    const int y = get_global_id(1);
    const int x = get_global_id(2);

    const int ly = get_local_id(1);
    const int lx = get_local_id(2);

    const int lysize = get_local_size(1);
    const int lxsize = get_local_size(2);

    local float kern_buf[max_channel_block_size*max_channel_block_size*9];

    for(int g = 0; g < group_size; g++){
        for(int ocb = g*out_group_size; ocb < (g+1)*out_group_size; ocb += max_channel_block_size){
            int ocb_size = min(max_channel_block_size, (g+1)*out_group_size-ocb);

            float16 sum_out1 = 0;
            for(int icb = g*out_group_size; icb < (g+1)*in_group_size; icb += max_channel_block_size){
                int icb_size = min(max_channel_block_size, (g+1)*in_group_size-icb);

                for(int ky = 0; ky < kern_height; ky++){
                    for(int kx = 0; kx < kern_width; kx++){
                        for(int oco = lx; oco < ocb_size; oco += lxsize){
                            for(int ico = ly; ico < icb_size; ico += lysize){
                                int oc = oco + ocb;
                                int ic = ico + icb;
                                int kic = ic - g*in_group_size;
                                kern_buf[((ky*kern_width+kx)*max_channel_block_size+ico)*max_channel_block_size+oco] = kern[((oc*in_group_size+kic)*kern_height+ky)*kern_width+kx];
                            }
                        }
                    }
                }
                barrier(CLK_LOCAL_MEM_FENCE);

                for(int ico = 0; ico < icb_size; ico++){
                    for(int ky = 0; ky < kern_height; ky++){
                        for(int kx = 0; kx < kern_width; kx++){
                            int iy = y + ky;
                            int ix = x + kx;
                            int ic = ico + icb;
                            float16 kern_val = vload16(0, &kern_buf[((ky*kern_width+kx)*max_channel_block_size+ico)*max_channel_block_size]);
                            float in_img_val = padded_img[((b*in_channels+ico)*padded_img_height+iy)*padded_img_width+ix];
                            sum_out1 += kern_val * in_img_val;
                        }
                    }
                }
            }

            float out_b[16];
            vstore16(sum_out1, 0, out_b);
            for(int oco = 0; oco < ocb_size; oco++){
                int oc = oco + ocb;
                out_img[((b*out_channels+oc)*image_height+y)*image_width+x] = out_b[oco];
            }
        }
    }
}
