import time
import torch
import sys


def run_conv(batch_size, input_channels, output_channels, kernel_width, kernel_height, image_width, image_height, group_size):
    input = torch.randn(batch_size, input_channels, image_width, image_height).to(device)
    nn = torch.nn.Conv2d(input_channels, output_channels, (kernel_width, kernel_height), padding=(kernel_width//2, kernel_height//2), groups=group_size, bias=False).to(device)
    start = time.time()
    n_tests = 10
    for i in range(n_tests):
        output = nn(input)
        assert output.shape == input.shape
        output[0,0,0,0].item()
    print((time.time() - start) / n_tests)

if len(sys.argv) > 1:
    device = sys.argv[1]
else:
    device = "cpu"

print("times for group sizes 1-128, in powers of 2:")
# print('group_size=1')
run_conv(16, 128, 128, 3, 3, 64, 64, 1)
# print('group_size=2')
run_conv(16, 128, 128, 3, 3, 64, 64, 2)
# print('group_size=4')
run_conv(16, 128, 128, 3, 3, 64, 64, 4)
# print('group_size=8')
run_conv(16, 128, 128, 3, 3, 64, 64, 8)
# print('group_size=16')
run_conv(16, 128, 128, 3, 3, 64, 64, 16)
# print('group_size=32')
run_conv(16, 128, 128, 3, 3, 64, 64, 32)
# print('group_size=64')
run_conv(16, 128, 128, 3, 3, 64, 64, 64)
# print('group_size=128')
run_conv(16, 128, 128, 3, 3, 64, 64, 128)
