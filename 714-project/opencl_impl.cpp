#include "opencl_executor.h"
#include <chrono>

OpenCLExecutor executor("opencl_kern.cl",0,1);
CLBuffer<float> in_img_buf;
CLBuffer<float> in_padded_img_buf;
CLBuffer<float> out_img_buf;
CLBuffer<float> kern_img_buf;
CLBuffer<int> parameter_buf;
bool has_called = false;

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

    int pad_width = kern_width/2;
    int pad_height = kern_height/2;

    size_t input_size = batch_size*in_channels*image_width*image_height;
    int in_buf_size = batch_size*in_channels*(pad_width*2+image_width)*(pad_height*2+image_height);
    size_t output_size = batch_size*out_channels*image_width*image_height;
    size_t kern_size = out_channels*(in_channels/group_size)*kern_width*kern_height;


    int params[] = {
        group_size,
        in_channels,
        out_channels,
        kern_width,
        kern_height,
        batch_size,
    };
    int num_params = sizeof(params) / sizeof(params[0]);
    if(!has_called){
        in_img_buf = executor.new_clbuffer<float>(input_size);
        in_padded_img_buf = executor.new_clbuffer<float>(in_buf_size);
        out_img_buf = executor.new_clbuffer<float>(output_size);
        parameter_buf = executor.new_clbuffer<int>(num_params);
        has_called = true;
    }
    kern_img_buf = executor.new_clbuffer<float>(kern_size);


    int in_buf_offset = batch_size*in_channels*(pad_width*2+image_width)*(pad_height*2+image_height);
    in_img_buf.write_buffer(in_image, input_size);
    kern_img_buf.write_buffer(kernel, kern_size);
    parameter_buf.write_buffer(params, num_params);

    CL_NDRange run_range_pad(batch_size, image_height, image_width);
    CL_NDRange run_range(batch_size, image_height, image_width);
    CL_NDRange local_range(1,16,32);
    CLKernel pad_image = executor.new_clkernel("pad_image",run_range_pad,CL_NDRange(),CL_NDRange(),{in_img_buf.k_arg(),in_padded_img_buf.k_arg(),parameter_buf.k_arg()});
    CLKernel compute_conv = executor.new_clkernel("compute_conv",run_range,local_range,CL_NDRange(),{in_padded_img_buf.k_arg(),kern_img_buf.k_arg(),out_img_buf.k_arg(),parameter_buf.k_arg()});

    pad_image.run();
    in_padded_img_buf.read_buffer(out_image, 1);
    auto start = std::chrono::system_clock::now();

    compute_conv.run();
    // compute_conv.barrier();
    //read one value to ensure that it is doing something
    out_img_buf.read_buffer(out_image, 1);

    std::chrono::duration<double> wctduration = (std::chrono::system_clock ::now() - start);

    out_img_buf.read_buffer(out_image, output_size);


    return wctduration.count();
}
