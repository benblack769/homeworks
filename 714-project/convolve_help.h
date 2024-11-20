
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
);
void dot_add(float * p1, float * p2, float mul, int size);
