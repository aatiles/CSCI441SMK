#!/usr/bin/env python

import sys
import random
if __name__ == "__main__":
    random.seed(123)
    if len(sys.argv) == 1:
        raise TypeError("No command line arguments")
    else:
        width = int(sys.argv[1])*3 + 1
        height = int(sys.argv[2])*3 + 1
        print(width, height)
        grid = [[[i,round(random.uniform(0,3),2),j] for j in range(width)] for i in range(int(height))]
        for i in range(3,height-1,3):
            for j in range(width):
                grid[i][j][1] = round((grid[i+1][j][1] + grid[i-1][j][1])/2,2)
        for j in range(3,width-1,3):
            for i in range(height):
                grid[i][j][1] = round((grid[i][j+1][1] + grid[i][j-1][1])/2,2)

                            

                
        
        for row in grid:
            for point in row:
                print(" ".join(map(str, point)),end=" ") 
            print() 
        
