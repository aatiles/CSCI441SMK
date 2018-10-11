#!/usr/bin/env python

import sys
import random
import numpy as np

def grid_corner(p1,p2,p3):
    # Find center of points
    max_x = max(p1[0], p2[0], p3[0])
    min_x = min(p1[0], p2[0], p3[0])
    max_z = max(p1[2], p2[2], p3[2])
    min_z = min(p1[2], p2[2], p3[2])
    cen_x = (min_x + max_x)//2
    cen_z = (min_z + max_z)//2
    # Convert to NP arrays
    p1 = np.array(p1)
    p2 = np.array(p2)
    p3 = np.array(p3)
    #Find Normal
    normal = np.cross(p2 - p1, p3 - p1)
    normal = normal / np.linalg.norm(normal)
    # Find planar points
    for x in [-1, 0 , 1]:
        for z in [-1, 0, 1]:
            pos = np.array([cen_x, 0, cen_z]) + np.array([x,0,z])
            pos_y = np.dot(normal, p1 - pos)/normal[1]
            yield pos + pos_y*np.array([0,1,0])


def main():
    # Determine the number of control points from a given number of patches
    width = int(sys.argv[1])*3 + 1
    height = int(sys.argv[2])*3 + 1
    # Show the number of expected points in the x and y directions
    print(width, height)
    # Randomly generate control points
    grid = [[[i,round(random.uniform(0,3),2),j] for j in range(width)] for i in range(int(height))]
    # Smooth Seems
    for i in range(3,height-1,3):
        for j in range(width):
            grid[i][j][1] = round((grid[i+1][j][1] + grid[i-1][j][1])/2,2)
    for j in range(3,width-1,3):
        for i in range(height):
            grid[i][j][1] = round((grid[i][j+1][1] + grid[i][j-1][1])/2,2)
    # Smooth corners
    for i in range(3, height-1,3):
        for j in range(3, width-1, 3):
            if i+1 != height and j+1 != width:
                p1 = grid[i-1][j-1]
                p2 = grid[i-1][j+1]
                p3 = grid[i+1][j-1]
                if i%6 == 3 and j%6 == 0:
                    p2 = grid[i+1][j+1]
                if i%6 == 0 and j%6 == 3:
                    p3 = grid[i+1][j+1]
                if i%6 == 0 and j%6 == 0:
                    p1 = grid[i+1][j+1]
                for point in grid_corner(p1, p2, p3):
                    x = int(point[0])
                    z = int(point[2])
                    grid[x][z][1] = round(point[1],2)
    # Generate Control Points
    for row in grid:
        for point in row:
            print(" ".join(map(str,point)),end=" ") 
            #print(point[1],end=",")
        print() 

if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise TypeError("No command line arguments")
    random.seed(123)
    main()
