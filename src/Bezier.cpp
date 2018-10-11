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

vector<vector<glm::vec3>> surfacePts;


vector<vector<glm::vec3>> getControlPoints(char *filename, int *row, int *col) {
    int i;

    vector<vector<glm::vec3>> vecs;
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

void loadTerrain() {
    int row, col;
    float step = 0.05f;
    vector<vector<glm::vec3>> vecs; 
    vecs = getControlPoints("flat_terrain.csv", &row, &col); 

    surfacePts.resize(row*(1.f/step+1));

    for (int i = 0; i < row/step; i++) {
        for (int j = 0; j < col/step; j++) {
            surfacePts[i].resize(col*(1.f/step+1));
        }
    }

    for (int i = 0; i < row-3; i += 3) {
        for (int j = 0; j < col; j += 3) {

            vector<glm::vec3> pts = {
                vecs[i][j],   vecs[i+1][j],   vecs[i+2][j],   vecs[i+3][j],
                vecs[i][j+1], vecs[i+1][j+1], vecs[i+2][j+1], vecs[i+3][j+1],
                vecs[i][j+2], vecs[i+1][j+2], vecs[i+2][j+2], vecs[i+3][j+2],
                vecs[i][j+3], vecs[i+1][j+3], vecs[i+2][j+3], vecs[i+3][j+3],
            };
            for (float k = 0; k <= 1; k += step) {
                for (float l = 0; l <= 1; l += step) {
                    surfacePts.at(i+k/step).at(j+l/step) = 
                        evaluateBezierSurface(pts, k, l);
                }
            }
        }
    } 
}
