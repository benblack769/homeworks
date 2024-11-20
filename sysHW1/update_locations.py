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

###############
# Create a list of 'size' floating point numbers in the range [-bound, bound]
def generate_random_list(size, bound):
    return [random.uniform(-bound, bound) for i in range(size)]

###############
# Update location by velocity, one time-step
def update_coords(x, y, z, vx, vy, vz):
    for i in range(len(x)):
        x[i] = x[i] + vx[i]
        y[i] = y[i] + vy[i]
        z[i] = z[i] + vz[i]

def time_fn(fn,*args):
    startt = time.clock()
    fn(*args)
    return time.clock() - startt
        
############ Main:
def main():
    if (len(sys.argv) != 3):
        print("Required arguments: vector_length(N) and iterations_num(M)")
        sys.exit(-1)

    size = int(sys.argv[1])
    iters = int(sys.argv[2])

    random.seed(size)

    x = generate_random_list(size, 1000.)
    y = generate_random_list(size, 1000.)
    z = generate_random_list(size, 1000.)
    vx = generate_random_list(size, 1.)
    vy = generate_random_list(size, 1.)
    vz = generate_random_list(size, 1.)

    def update_all(num_iters):
        for n in range(num_iters):
            update_coords(x,y,z,vx,vy,vz)
            
    t = time_fn(update_all,iters)
    
    #chksum = sum(x) + sum(y) + sum(z)
    print(1000000 * t / (size * iters))
    #print("Mean time per coordinate: " + str(1000000 * t / (size * iters)) + "us")
    #print("Final checksum is: " + str(chksum))
    
    
if __name__ == "__main__":
    main()
