#ifndef FAERY
#define FAERY

// include the OpenGL library header
#ifdef __APPLE__                    // if compiling on Mac OS
    #include <OpenGL/gl.h>
#else                                       // if compiling on Linux or Windows OS
    #include <GL/gl.h>
#endif

#include <GLFW/glfw3.h> // include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>               // for cos(), sin() functionality
#include <stdio.h>          // for printf functionality
#include <stdlib.h>         // for exit functionality
#include <time.h>             // for time() functionality
#include <errno.h>

#include <vector>

#define RESOLUTION 1000

#define MOVE_RES 250
#define MOVE_PARAM_RES (MOVE_RES*(numSegments/3))
#define PARAM_RES (RESOLUTION*(numSegments/3))
#include "Faery.cpp"

int tick = 0;
int g_tick = 0;
int numSegments = 0;
bool leftThis = false;

glm::vec3 paramFaeryPos;
glm::vec3 eqFaeryPos;
vector<glm::vec3> controlPoints;    // Control points for the bezier curve loaded from a file

bool loadFaeryControlPoints( char *filename ) {
    int len;
    int x,y,z;
    FILE *f = fopen(filename, "r");

    if (!f) {
        perror("fopen");
        return errno;
    }
    fscanf(f, "%d\n", &len);
    controlPoints.reserve(len);

    for (int i = 0; i < len; i++) {
        fscanf(f, "%d,%d,%d\n", &x, &y, &z);
        controlPoints[i] = glm::vec3(x, y, z);
    }

    fclose(f);
    numSegments = len;
    return true;
}

double getBezier(double p0, double p1, double p2, double p3, float t) {
    return pow(1-t, 3)*p0 + 3*pow(1-t, 2)*t*p1 + 3*(1-t)*t*t*p2 + t*t*t*p3;
}


void drawHead() {
	//base of head
	glColor3f(.94, .85, .09);
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 4, 0));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidSphere(1.0, 50, 50);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

	//hair bun
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, 4.25, 1));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidTorus(.25, .3, 50, 50);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
}
void drawArms() {
	//arms
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 3, 0));
	glMultMatrixf(&transMtx[0][0]); {
		glm::mat4 scalMtx = glm::scale(glm::mat4(), glm::vec3(3, .5, .5));
		glMultMatrixf(&scalMtx[0][0]);
			CSCI441::drawSolidCube(1);
		glMultMatrixf(&(glm::inverse(scalMtx))[0][0]);
	};
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
}
void drawBody() {
	glColor3f(.11, .27, .53);
	//chest
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidCone(2.0, 4.0, 50, 50); //for body of character
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

	transMtx = glm::translate(glm::mat4(), glm::vec3(0, 3, 0));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidCube(1.25); //for body of character
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

}
void drawCharacter() {
	drawBody();
	drawHead();
	float shift;
	if (leftThis)
		shift = .55;
	else
		shift = -.55;
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(shift, 0, 0));
	glMultMatrixf(&transMtx[0][0]);
	drawArms();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
}
/*
glm::vec3 evaluateBezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t ) {
    glm::vec3 point(0,0,0);

    point.x = getBezier(p0.x, p1.x, p2.x, p3.x, t);
    point.y = getBezier(p0.y, p1.y, p2.y, p3.y, t); 
    point.z = getBezier(p0.z, p1.z, p2.z, p3.z, t);
    return point;
}
*/


double getVecDist(glm::vec3 a, glm::vec3 b) {
    return sqrt(pow(b.x-a.x, 2)
              + pow(b.y-a.y, 2)
              + pow(b.z-a.z, 2));
}

double getBezierLength(std::vector<glm::vec3> vecs) {
    double len = 0.0;
    for (int i = 0; i < (int)vecs.size(); i++) {
        if (!i) continue;
        len += getVecDist(vecs[i-1], vecs[i]);
    }
    return len;
}

/* evaluates curve at specified resolution */
std::vector<glm::vec3> getBezierCurve (glm::vec3 p0, glm::vec3 p1, glm::vec3
        p2, glm::vec3 p3, int resolution) {
    std::vector<glm::vec3> vecs;

    for (int i = 0; i <= resolution; i++) {
        vecs.push_back(evaluateBezierCurve(p0, p1, p2, p3,
                    (float)i/resolution));
    }
    return vecs;
}

/* evaluates all curves specified by control points */
std::vector<glm::vec3> getBezierCurves() {
    std::vector<glm::vec3> vecs;
    std::vector<glm::vec3> newvecs;

    for (int i = 0; i < numSegments-1; i+=3) {
        newvecs = getBezierCurve(controlPoints[i], controlPoints[i+1],
                controlPoints[i+2], controlPoints[i+3], RESOLUTION);
        vecs.insert(vecs.end(), newvecs.begin(), newvecs.end());
    }
    return vecs;
}

/* bezier is herefore rendered */
void renderBezierCurve() {
    std::vector<glm::vec3> vecs = getBezierCurves();
    getBezierLength(vecs);

    glDisable( GL_LIGHTING );
    glBegin(GL_LINE_STRIP);
    glLineWidth(3);
    glColor3f(0.f, 0.f, 1.f);
	int vecSize = vecs.size();
	for (int k = 0; k < vecSize; k++) {
		glm::vec3 temp = vecs.at(k);
		glVertex3f(temp.x, temp.y, temp.z);
    }

    glEnd();
    glEnable( GL_LIGHTING );
}

/* evaluates bezier at given t (1st curve: [0,1], 2nd curve: [1,2], etc) */
glm::vec3 getBezierAtT(int t) {
    std::vector<glm::vec3> vecs = getBezierCurves();
    return vecs[t%(RESOLUTION*(numSegments/3))];
}

/* returns a lower and upper bound, and weight to interpolate them between */
void findInterpolants(int *a, int *b, double *weight) {
    std::vector<glm::vec3> vecs = getBezierCurves();
    double len = getBezierLength(vecs);
    double target = len / MOVE_RES;

    *a = -1; *b = -1; 
    double dista = 0.0, distb = 0.0;
    for (int i = tick; i < (int)vecs.size(); i++) {
        double dist = getVecDist(paramFaeryPos, vecs[i]);
        if (dist < target) {
            *a = i;
            dista = dist;
        }   
        else if (dist >= target) {
            *b = i;
            distb = dist;
            break;
        }   
    }   

    if (*a < 0) {
        *a = tick%PARAM_RES;
        dista = getVecDist(paramFaeryPos, vecs[*a]);
    }   
    if (*b < 0) {
        *b = (int)vecs.size() -1; 
        distb = getVecDist(paramFaeryPos, vecs[*b]);
    }   
    *weight = (distb-dista)/target;
}

/* provides the linear interpolant between two vectors given a weight */
glm::vec3 getWeightedVec(glm::vec3 a, glm::vec3 b, double weight) {
    glm::vec3 diff;
    diff.x += (b.x - a.x)*weight;
    diff.y += (b.y - a.y)*weight;
    diff.z += (b.z - a.z)*weight;
    return a+diff;
}

/* updates the position of the faeries */
void updateParamFaery() {
    int a, b;
    double weight;
    std::vector<glm::vec3> vecs = getBezierCurves();

    findInterpolants(&a, &b, &weight);
    paramFaeryPos = getWeightedVec(vecs[a%(PARAM_RES)],vecs[b%(PARAM_RES)],weight);
    tick = (weight < 1) ? (a%PARAM_RES) : (b%PARAM_RES);
}

void updateEqFaery() {
    eqFaeryPos = getBezierAtT(g_tick*4);
}

void updateSpectators() {
    updateParamFaery();
    updateEqFaery();
    g_tick++;
	if (g_tick % 10 == 0)
		leftThis = !leftThis;
}

void drawSpectators() {
    glm::mat4 mtx;

    glColor3f(0,1,0);
    mtx = glm::translate(glm::mat4(), paramFaeryPos);
    glMultMatrixf(&mtx[0][0]);
	drawCharacter();
    glMultMatrixf(&(glm::inverse(mtx))[0][0]); 

    glColor3f(1,0,0);
    mtx = glm::translate(glm::mat4(), eqFaeryPos);
    glMultMatrixf(&mtx[0][0]);
    CSCI441::drawSolidSphere(0.5, 10, 10);
    glMultMatrixf(&(glm::inverse(mtx))[0][0]); 

}

void drawTrack() {
    renderBezierCurve();
}


#endif
