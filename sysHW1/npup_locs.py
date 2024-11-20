#!/usr/bin/env python3
"""
    This program generates N random coordinates in space (3D), and N random
    velocity vectors. It then iterates M times to update the locations based
    on the velocity.
    Finally, it outputs the sum of all coordinates as a checksum of the computation.
    Coordinates start in the range [-1000:1000] per dimension.
    Velocities are chosen from the range [-1:1] per dimension.
"""

import random, sys, timeit, time
import numpy 

###############
# Create a list of 'size' floating point numbers in the range [-bound, bound]
def generate_random_list(size, bound):
    return [random.uniform(-bound, bound) for i in range(size)]

###############
# Update location by velocity, one time-step
def update_coords(x, y, z, vx, vy, vz):
    x += vx
    y += vy
    z += vz
    '''for i in range(size):
        x[i] = x[i] + vx[i]
        y[i] = y[i] + vy[i]
        z[i] = z[i] + vz[i]'''

############ Main:

if (len(sys.argv) != 3):
    print("Required arguments: vector_length(N) and iterations_num(M)")
    sys.exit(-1)

size = int(sys.argv[1])
iters = int(sys.argv[2])

random.seed(size)

floatty = numpy.float32
x = numpy.array(generate_random_list(size, 1000.),dtype=floatty)
y = numpy.array(generate_random_list(size, 1000.),dtype=floatty)
z = numpy.array(generate_random_list(size, 1000.),dtype=floatty)
vx = numpy.array(generate_random_list(size, 1.),dtype=floatty)
vy = numpy.array(generate_random_list(size, 1.),dtype=floatty)
vz = numpy.array(generate_random_list(size, 1.),dtype=floatty)



'''t = timeit.timeit(stmt = "update_coords(x, y, z, vx, vy, vz)",
                  setup = "from __main__ import update_coords, x, y, z, vx, vy, vz",
                  number = iters)'''
startt = time.clock()
for n in range(iters):
	update_coords(x,y,z,vx,vy,vz)
t = time.clock() - startt

#chksum = sum(x) + sum(y) + sum(z)
print(1000000 * t / (size * iters))
#print("Mean time per coordinate: " + str(1000000 * t / (size * iters)) + "us")
#print("Final checksum is: " + str(chksum))

