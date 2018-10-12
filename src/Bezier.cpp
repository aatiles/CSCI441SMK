#ifndef BEZIER
#define BEZIER

// include the OpenGL library header
#ifdef __APPLE__					// if compiling on Mac OS
	#include <OpenGL/gl.h>
#else										// if compiling on Linux or Windows OS
	#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>	// include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>				// for cos(), sin() functionality
#include <stdio.h>			// for printf functionality
#include <stdlib.h>			// for exit functionality
#include <time.h>			  // for time() functionality

#include <fstream>			// for file I/O
#include <vector>				// for vector
#include <string>

using namespace std;

vector< vector<glm::vec3> > surfacePts;

struct surfaceAttr {
    int row; 
    int col;
    float step;
};

vector<vector<glm::vec3> > getControlPoints(char *filename, int *row, int *col) {
    vector<vector<glm::vec3> > vecs;
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen getControlPoints");
        exit(errno);
    }
    
    /* row x col */
    fscanf(f, "%d %d\n", row, col);
    
    vecs.resize(*row);
    for (int i = 0; i < *row; i++) {
        vecs[i].resize(*col);
        for (int j = 0; j < *col; j++) {
            glm::vec3 *vec = &vecs[i][j];
            fscanf(f, "%f %f %f ", &vec->x, &vec->y, &vec->z);
        }
    }
    return vecs;
}

// evaluateBezierCurve() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier Curve.
//
////////////////////////////////////////////////////////////////////////////////
glm::vec3 evaluateBezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
	glm::vec3 point(0, 0, 0);

    point = p0 + t*((-3.0f * p0 + 3.0f * p1) + t*((3.0f*p0 - 6.0f*p1 + 3.0f*p2)
               + t*(-1.0f*p0 + 3.0f*p1 - 3.0f*p2 + p3)));

	return point;
}

// evaluateBezierSurface() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier surface.
//
// pts should be a vector of 16 control points in the following position order
//  x ->
// 12 13 14 15
//  8  9 10 11  ^
//  4  5  6  7  |
//  0  1  2  3  y
//
////////////////////////////////////////////////////////////////////////////////
glm::vec3 evaluateBezierSurface(vector<glm::vec3> pts, float u, float v) {
    glm::vec3* pu = new glm::vec3[4];
    for (size_t i = 0; i < 4; i++) {
        pu[i] = evaluateBezierCurve(pts.at(i * 4), pts.at(i * 4 + 1), pts.at(i * 4 + 2), pts.at(i * 4 + 3), u);
    }
    return evaluateBezierCurve(pu[0], pu[1], pu[2], pu[3], v);
}

void loadTerrain(char *filename, struct surfaceAttr *sAttr) {
    int row, col;
    float step = 0.05f;
    vector<vector<glm::vec3> > vecs; 
    vecs = getControlPoints(filename, &row, &col); 
    puts("LOAD");
    //surfacePts.resize((row+1)/step);

   /*
    for (int i = 0; i <= row/step; i++) {
            surfacePts[i].resize((col+1)/step);
    }
    */

    for (int i = 0; i <= row-3; i += 3) {
        surfacePts.push_back(vector<glm::vec3>());
        for (int j = 0; j <= col-3; j += 3) {

            vector<glm::vec3> pts;

            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 4; l++) {
                    pts.push_back(vecs.at(i+l).at(j+k));
                }
            }

            for (float k = 0; k-1 < 0.001; k += step) {
                for (float l = 0; l-1 < 0.001; l += step) {
                    /*surfacePts.at(i+k/step).at(j+l/step)*/ 
                    surfacePts.at(surfacePts.size()-1).push_back(evaluateBezierSurface(pts, k, l));
                }
            }

        }
    } 

    sAttr->row = row;
    sAttr->col = col;
    sAttr->step = step;
}

#endif
