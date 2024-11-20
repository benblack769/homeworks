/*
code heavily borrows from https://gist.github.com/goldsborough/865e6717e64fbae75cdaf6c9914a130d
*/
#include <cudnn.h>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>

#define checkCUDNN(expression)                               \
  {                                                          \
    cudnnStatus_t status = (expression);                     \
    if (status != CUDNN_STATUS_SUCCESS) {                    \
      std::cerr << "Error on line " << __LINE__ << ": "      \
                << cudnnGetErrorString(status) << std::endl; \
      std::exit(EXIT_FAILURE);                               \
    }                                                        \
  }

bool has_initted = false;

cudnnHandle_t cudnn;


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
    if (!has_initted){
        cudnnCreate(&cudnn);
        has_initted = true;
    }
    // printf("cuda_out(%d,%d,%d,%d)\n",batch_size_out, channels_out, height_out, width_out);

    assert(in_channels%group_size == 0 && out_channels%group_size == 0);
    assert(kern_width%2 == 1 && kern_height%2 == 1);

      cudnnTensorDescriptor_t input_descriptor;
      checkCUDNN(cudnnCreateTensorDescriptor(&input_descriptor));
      checkCUDNN(cudnnSetTensor4dDescriptor(input_descriptor,
                                            /*format=*/CUDNN_TENSOR_NCHW,
                                            /*dataType=*/CUDNN_DATA_FLOAT,
                                            /*batch_size=*/batch_size,
                                            /*channels=*/in_channels,
                                            /*image_height=*/image_height,
                                            /*image_width=*/image_width));

      cudnnFilterDescriptor_t kernel_descriptor;
      checkCUDNN(cudnnCreateFilterDescriptor(&kernel_descriptor));
      checkCUDNN(cudnnSetFilter4dDescriptor(kernel_descriptor,
                                            /*dataType=*/CUDNN_DATA_FLOAT,
                                            /*format=*/CUDNN_TENSOR_NCHW,
                                            /*out_channels=*/out_channels,
                                            /*in_channels=*/in_channels/group_size,
                                            /*kernel_height=*/kern_height,
                                            /*kernel_width=*/kern_width));

      cudnnConvolutionDescriptor_t convolution_descriptor;
      checkCUDNN(cudnnCreateConvolutionDescriptor(&convolution_descriptor));
      checkCUDNN(cudnnSetConvolution2dDescriptor(convolution_descriptor,
                                                 /*pad_height=*/kern_height/2,
                                                 /*pad_width=*/kern_width/2,
                                                 /*vertical_stride=*/1,
                                                 /*horizontal_stride=*/1,
                                                 /*dilation_height=*/1,
                                                 /*dilation_width=*/1,
                                                 /*mode=*/CUDNN_CROSS_CORRELATION,
                                                 /*computeType=*/CUDNN_DATA_FLOAT));
      checkCUDNN(cudnnSetConvolutionGroupCount(convolution_descriptor,
                                                /*group_size*/group_size));

      int batch_size_out{0}, channels_out{0}, height_out{0}, width_out{0};
      checkCUDNN(cudnnGetConvolution2dForwardOutputDim(convolution_descriptor,
                                                       input_descriptor,
                                                       kernel_descriptor,
                                                       &batch_size_out,
                                                       &channels_out,
                                                       &height_out,
                                                       &width_out));
    // fflush(stdout);
    assert(
        batch_size == batch_size_out &&
        out_channels == channels_out &&
        image_height == height_out &&
        image_width == width_out
    );

      cudnnTensorDescriptor_t output_descriptor;
      checkCUDNN(cudnnCreateTensorDescriptor(&output_descriptor));
      checkCUDNN(cudnnSetTensor4dDescriptor(output_descriptor,
                                            /*format=*/CUDNN_TENSOR_NHWC,
                                            /*dataType=*/CUDNN_DATA_FLOAT,
                                            /*batch_size=*/batch_size,
                                            /*channels=*/out_channels,
                                            /*image_height=*/image_height,
                                            /*image_width=*/image_width));


    cudnnConvolutionFwdAlgo_t convolution_algorithm;
    checkCUDNN(
      cudnnGetConvolutionForwardAlgorithm(cudnn,
                                          input_descriptor,
                                          kernel_descriptor,
                                          convolution_descriptor,
                                          output_descriptor,
                                          CUDNN_CONVOLUTION_FWD_PREFER_FASTEST,
                                          /*memoryLimitInBytes=*/0,
                                          &convolution_algorithm));

    size_t workspace_bytes{0};
    checkCUDNN(cudnnGetConvolutionForwardWorkspaceSize(cudnn,
                                                       input_descriptor,
                                                       kernel_descriptor,
                                                       convolution_descriptor,
                                                       output_descriptor,
                                                       convolution_algorithm,
                                                       &workspace_bytes));

     void* d_workspace{nullptr};
     cudaMalloc(&d_workspace, workspace_bytes);

     int input_bytes = batch_size*in_channels*image_width*image_height*sizeof(float);
       float* d_input{nullptr};
       cudaMalloc(&d_input, input_bytes);
       cudaMemcpy(d_input, in_image, input_bytes, cudaMemcpyHostToDevice);

              int output_bytes = batch_size*out_channels*image_width*image_height*sizeof(float);
              float* d_output{nullptr};
              cudaMalloc(&d_output, output_bytes);
              cudaMemset(d_output, 0, output_bytes);

     int kernel_bytes = in_channels*out_channels*kern_width*kern_height*sizeof(float);
     float* d_kernel{nullptr};
     cudaMalloc(&d_kernel, kernel_bytes);
     cudaMemcpy(d_kernel, kernel, kernel_bytes, cudaMemcpyHostToDevice);

     auto start = std::chrono::system_clock::now();
       const float alpha = 1.0f, beta = 0.0f;
       checkCUDNN(cudnnConvolutionForward(cudnn,
                                          &alpha,
                                          input_descriptor,
                                          d_input,
                                          kernel_descriptor,
                                          d_kernel,
                                          convolution_descriptor,
                                          convolution_algorithm,
                                          d_workspace,
                                          workspace_bytes,
                                          &beta,
                                          output_descriptor,
                                          d_output));

     // copy the first element to make sure the result is computed...
      cudaMemcpy(out_image, d_output, 1, cudaMemcpyDeviceToHost);

      std::chrono::duration<double> wctduration = (std::chrono::system_clock ::now() - start);

      cudaMemcpy(out_image, d_output, output_bytes, cudaMemcpyDeviceToHost);

      double time_result = wctduration.count();

      cudaFree(d_kernel);
      cudaFree(d_input);
      cudaFree(d_output);
      cudaFree(d_workspace);

      cudnnDestroyTensorDescriptor(input_descriptor);
      cudnnDestroyTensorDescriptor(output_descriptor);
      cudnnDestroyFilterDescriptor(kernel_descriptor);
      cudnnDestroyConvolutionDescriptor(convolution_descriptor);
      return time_result;
}
