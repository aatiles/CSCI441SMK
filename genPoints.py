#!/usr/bin/env python

import sys

if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise TypeError("No command line arguments")
    else:
        print(int(sys.argv[1])*int(sys.argv[2]))
        for i in range(int(sys.argv[1])):
            for j in range(int(sys.argv[2])):
                print(i,0,j)

