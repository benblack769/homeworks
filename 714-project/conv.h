/*
Performs grouped convolution

Assumptions:

1. Padding is zero padded. Implemented this way because it is so common in resnets
2. kernel sizes are odd
3. input and output buffers are big enough
4. in_channels and out_channels are divisiible by group size
5. inputs are formatted in NCHW format

*/
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
);
