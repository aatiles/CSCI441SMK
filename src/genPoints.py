#!/usr/bin/env python

import sys
import random
if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise TypeError("No command line arguments")
    else:
        width = int(sys.argv[1])*3 + 1
        height = int(sys.argv[2])*3 + 1
        print(width, height)
        grid = [[[i,random.uniform(0,3),j] for j in range(width)] for i in range(int(height))]
        for row in grid:
            for point in row:
                print(" ".join(map(str, point)),end=" ") 
            print() 
        
