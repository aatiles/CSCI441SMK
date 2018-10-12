/* *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: A3
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2015
 *	Modified: Dr. Jeffrey Paone - Fall 2017 for GLFW
 *	Modified: Billy Brickner - Fall 2018 for HW
 *
 *  Description:
 *      Contains the base code for a basic flight simulator.
 *
 */

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
#include <limits>

#include <fstream>			// for file I/O
#include <vector>				// for vector

/* shh, this is temporary because I dunno what paone did w his makefile */
#include "Bezier.cpp"
#include "Faery.cpp"

using namespace std;

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.

// I/O Stuff
int windowWidth = 640, windowHeight = 480;

int leftMouseButton;    	 					// status of the mouse button
glm::vec2 mousePos;			              		  	// last known X and Y of the mouse

// Camera Stuff
float       cameraTheta, cameraPhi;               		// camera DIRECTION in spherical coordinates
glm::vec3   camDir; 			                    	// camera DIRECTION in cartesian coordinates
glm::vec3   cameraPos;                                          // camera POSITION in cartesian coordinates
float       camRadius   = 2;                // Distance of camera from vehicle
float       upper_ang   = 2*M_PI/3;         // Upper Bound of camera angle
float       lower_ang   = 4*M_PI/7;         // Lower Bound of camera angle
int         zoom        = 0;                // state of control to zoom
float       zoomScale   = 20;               // Rate at which zoom occurs based on change in mouse
int         veiwport    = 0;                // Veiwport of camerai
int         camera_mov  = 0;
float       camSpeed    = .08;

// Vehicle stuff
glm::vec3 vehicleLoc;
float   vehicleSpeed    = 0.05;             // Amount the vehicle changes position when moving
int     goingForward    = 0;                // State of control to go forward 
int     goingBackward   = 0;                // State of control to go backward
int     turnLeft        = 0;                // State of control to go left
int     turnRight       = 0;                // State of control to go right
float   vehiclePhi      = 0;                // Yaw of vehicle 
float   turnSpeed       = 0.05;             // Speed yaw of vehicle changes
float   wheelRotation   = 0;                // State of control to 
float   wheelSpeed      = 0.7;              // Speed wheels turn as the vehicle is moving
float   collisionBox    = 0.1;              // Size of box that collision occurs from
float   yaw;

glm::vec3 *envCurr, *envNext, *envAcross;
GLuint environmentDL;                       		// display list for the 'city'


// Fairy Stuff
int     resolution      = 30;       // The subdivisions of the Bezier Curve
int     fairy_seg       = 0;        // Segment of the Bezier curve that the fairy is on
float   fairy_time      = 0;        // Subdivision of the Segment of the Bezier curve that the fairy is at
float   fairy_speed     = .01;      // Speed that the fairy moves along hte curve
float   wing_speed      = 10;       // Speed that the wings flap
float   wing_angle      = .4;       // Angle that the wings flap between
float   wing_offset     = .7;       // Angle that the wings flap around
bool    toggle_cage     = false;    // Show the control cage
bool    toggle_curve    = false;    // Show the bezier curve

struct surfaceAttr sAttr;

//*************************************************************************************
//
// Helper Functions


// loadControlPoints() /////////////////////////////////////////////////////////
//
//  Load our control points from file and store them in
//	the global variable controlPoints
//
////////////////////////////////////////////////////////////////////////////////
bool loadControlPoints( char* filename ) {
    // TODO #02: read in control points from file.  Make sure the file can be
    // opened and handle it appropriately.
    float number, x, y, z;
    char garbage;
    ifstream points;
    points.open(filename);
    points >> number;
    fprintf(stdout, "Number of points: %f\n", number);
    for (int i = 0; i < number; i++){
        points >> x >> garbage >> y >> garbage >> z;
        fprintf(stdout, "Point %d: %f, %f, %f\n",i, x,y,z);
        controlPoints.push_back(glm::vec3(x,y,z));
    }
    points.close();
    return true;
}
// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution.
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution ) {
    // TODO #05: Draw the Bezier Curve!
    glm::vec3 point (0,0,0); 
    glBegin( GL_LINE_STRIP );
        glColor3f(0,1,0);
        for (int i = 0; i <= resolution; i++){
            point = evaluateBezierCurve(p0, p1, p2, p3, ((float) i /(float) resolution));
            glVertex3f(point.x, point.y, point.z);
        }
    glEnd();
}

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() { return rand() / (float)RAND_MAX; }

// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's direction in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta or cameraPhi is updated.
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
    // TODO #5: Convert spherical coordinates into a cartesian vector
    // see Wednesday's slides for equations.  Extra Hint: Slide #70
    camDir = glm::vec3(sin(cameraTheta)*sin(cameraPhi), 1*cos(cameraPhi), cos(cameraTheta)*sin(cameraPhi));

	// and NORMALIZE this directional vector!!!

}

//*************************************************************************************
//
// Event Callbacks

//
//	void error_callback( int error, const char* description )
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
static void error_callback( int error, const char* description ) {
	fprintf( stderr, "[ERROR]: %s\n", description );
}

static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
	if( action == GLFW_PRESS ) {
		switch( key ) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				exit(EXIT_SUCCESS);
		}
        }
        switch( key ){
            case GLFW_KEY_W: goingForward       = action;
                break;
            case GLFW_KEY_S: goingBackward      = action;
                break;
            case GLFW_KEY_A: turnLeft           = action;
                break;
            case GLFW_KEY_D: turnRight          = action;
                break;
            case GLFW_KEY_LEFT_CONTROL: zoom    = action;
                break;
            case GLFW_KEY_SPACE: camera_mov     = action;
                break;
            case GLFW_KEY_C: {
                if (action == GLFW_PRESS){
                    toggle_cage        = !toggle_cage;
                    break;
                }
            }
            case GLFW_KEY_B:
                if (action == GLFW_PRESS){
                    toggle_curve       = !toggle_curve;
                    break;
                }
            case GLFW_KEY_1:
                if (action == GLFW_PRESS){
                    veiwport = (veiwport + 1)%3;
                }
	}
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//  GLFW callback for mouse movement. We update cameraPhi and/or cameraTheta
//      based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback( GLFWwindow *window, double x, double y ) {
	int scale = 512;
        if( leftMouseButton == GLFW_PRESS ) {
            // Zoom in Camera
            if (zoom == GLFW_PRESS || zoom == GLFW_REPEAT){
                camRadius = camRadius*(1+(y-mousePos.y)/zoomScale);
                if (camRadius < 1){
                    camRadius = 1;
                }
                if (camRadius > 100){
                    camRadius = 100;
                }
            }
            // Rotate Camera
            else{
                cameraTheta = cameraTheta + (x - mousePos.x)/scale;
                cameraPhi   = cameraPhi   + (y - mousePos.y)/scale;
                if (cameraPhi < lower_ang){
                    cameraPhi = lower_ang;
                }
                if (cameraPhi > upper_ang){
                    cameraPhi = upper_ang;
                 }
		recomputeOrientation();     // update camera (x,y,z) based on (theta,phi)
            }
	}
	mousePos.x = x;
	mousePos.y = y;
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//  GLFW callback for mouse clicks. We save the state of the mouse button
//      when this is called so that we can check the status of the mouse
//      buttons inside the motion callback (whether they are up or down).
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
	if( button == GLFW_MOUSE_BUTTON_LEFT ) {
		leftMouseButton = action;
	}
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawGrid() {
	/*
     *	We will get to why we need to do this when we talk about lighting,
     *	but for now whenever we want to draw something with an OpenGL
     *	Primitive - like a line, quad, point - we need to disable lighting
     *	and then reenable it for use with the CSCI441 3D Objects.
     */
	glDisable( GL_LIGHTING );

        glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < (int)surfacePts.size()-1; i++) {
            glColor3f((float)i/10+.1, (float)i/10+.1, (float)i/10+.1);
            for (int j = 0; j < (int)surfacePts[i].size()-1; j++) {
                glVertex3f(surfacePts.at(i).at(j).x,surfacePts.at(i).at(j).y,surfacePts.at(i).at(j).z);
                glVertex3f(surfacePts.at(i+1).at(j+1).x,surfacePts.at(i+1).at(j+1).y,surfacePts.at(i+1).at(j+1).z);
            }
        }
        glEnd();
        /*
        glBegin( GL_LINES );
            glColor3f(1.0f, 1.0f, 1.0f);
            for(int i = -50; i <= 50; i++){
                glVertex3f(  i, 0, -50);
                glVertex3f(  i, 0,  50);
                glVertex3f(-50, 0,   i);
                glVertex3f( 50, 0,   i);
            }
        glEnd();
	*/
	glEnable( GL_LIGHTING );
}

void drawScene() {
	// Code from Lab 2
	for (int x = -50; x <= 50; x += 5) {
		for (int z = -50; z <= 50; z += 5) {
			float random = getRand();
			if (random < 0.4 && x % 2 == 0 && z % 2 == 0) {
				glColor3f(getRand(), getRand(), getRand());
				int height = rand() % 10 + 1;
				glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(x, height / 2, z));
				glMultMatrixf(&transMtx[0][0]); {
					glm::mat4 scalMtx = glm::scale(glm::mat4(), glm::vec3(1, height, 1));
					glMultMatrixf(&scalMtx[0][0]); {
						CSCI441::drawSolidCube(1);
						CSCI441::drawSolidCone();
					};
					glMultMatrixf(&(glm::inverse(scalMtx))[0][0]);
				};
				glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
			}

		}
	}


}

// generateEnvironmentDL() /////////////////////////////////////////////////////
//
//  This function creates a display list with the code to draw a simple
//      environment for the user to navigate through.
//
//  And yes, it uses a global variable for the display list.
//  I know, I know! Kids: don't try this at home. There's something to be said
//      for object-oriented programming after all.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironmentDL() {
    // TODO #1 Create a Display List & Call our Drawing Functions
    environmentDL = glGenLists(1);
    glNewList( environmentDL , GL_COMPILE );
        drawGrid();
		drawScene();
       glEndList();
}

glm::vec3 *evalGround(float x, float z) {
    float min = numeric_limits<float>::max();
    glm::vec3 *min_vec = NULL;

    for (int i = 0; i < (int)surfacePts.size(); i++) {
        for (int j = 0; j < (int)surfacePts[i].size(); j++) {
            glm::vec3 *vec_p = &surfacePts.at(i).at(j);
            float res = sqrt(pow(vec_p->x-x, 2) + pow(vec_p->z-z, 2));
            if (res < min) {
                min = res;
                min_vec = vec_p;
            }
        }
    }
    return min_vec;
}

//      void updateState()
//
//              This is a helper method to update the state of internal variables once per inner loop
//
void updateState(){
    glm::vec3 vehicleDir = glm::vec3(cos(vehiclePhi), 0, sin(vehiclePhi)); 
    // Check if the vehicle is turning left
    if (turnLeft == GLFW_PRESS || turnLeft == GLFW_REPEAT){
        vehiclePhi = vehiclePhi - turnSpeed; 
    }
    // cated at vehicleicle is turning right
    if (turnRight == GLFW_PRESS || turnRight == GLFW_REPEAT){
        vehiclePhi = vehiclePhi + turnSpeed; 
    }
    // Check if the vehicle is going forwards
    if (goingForward == GLFW_PRESS || goingForward == GLFW_REPEAT){
         vehicleLoc     = vehicleLoc - vehicleSpeed*vehicleDir;
         wheelRotation  = wheelRotation + wheelSpeed; 

    }
    // Check if the vehicle is going backwards
    if (goingBackward == GLFW_PRESS || goingBackward == GLFW_REPEAT){
         vehicleLoc = vehicleLoc + vehicleSpeed*vehicleDir;
         wheelRotation  = wheelRotation - wheelSpeed; 
    }
    
    // Bound the vehicle location
    // Create a collision box around the vehicle
    if (vehicleLoc.x < -50 + collisionBox){
        vehicleLoc.x = -50 + collisionBox;
    }
    if (vehicleLoc.x >  50 - collisionBox){
        vehicleLoc.x =  50 - collisionBox;
    }
    if (vehicleLoc.z < -50 + collisionBox){
        vehicleLoc.z = -50 + collisionBox;
    }
    if (vehicleLoc.z >  50 - collisionBox){
        vehicleLoc.z =  50 - collisionBox;
    }

    glm::vec3 nextPos = vehicleLoc - (5.f)*vehicleDir;
    glm::vec3 acrossPos = vehicleLoc - (25.f)*glm::vec3(sin(vehiclePhi), 0, cos(vehiclePhi));
    envCurr = evalGround(vehicleLoc.x, vehicleLoc.z);
    envNext = evalGround(nextPos.x, nextPos.z);
    envAcross = evalGround(acrossPos.x, acrossPos.z);

    // Keep Vehicle on the ground
    vehicleLoc.y = envCurr->y;

    glm::vec3 vehicleNorm = glm::cross(*envNext - *envCurr, *envAcross - *envNext);
    vehicleNorm = glm::normalize(vehicleNorm);
    //printf("norm is %f %f %f\n", vehicleNorm.x, vehicleNorm.y, vehicleNorm.z);

    glm::vec3 tang = *envNext-*envCurr;
    yaw = acos(glm::dot(tang, glm::vec3(0,1,0))/(glm::length(tang)))-(float)M_PI/2.f;

    printf("yaw is %f\n", yaw);
    // Move Fairy
    fairy_time = fairy_time + fairy_speed;
    if (fairy_time > 1) {
        fairy_time = fairy_time - 1;
        fairy_seg  = fairy_seg + 1;
    }
    fairy_seg = fairy_seg%(controlPoints.size()/3);
    if (camera_mov == GLFW_PRESS || camera_mov == GLFW_REPEAT){
        cameraPos = cameraPos + camSpeed*camDir;
    }

    updateSpectators();
}

// Draw the handlebars of the bike
void drawHandleBars() {
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(-0.3, 0.2, 0));
        transMatrix = glm::rotate(      transMatrix,
                                        -0.1f,
                                        glm::vec3(0,0,1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawSolidTeapot(.3);
    glPopMatrix();


}

// Draw the connecting bar of the bike
void drawLowerBar(){
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(-.5, .1, 0));
        transMatrix = glm::rotate(      transMatrix,
                                        -1.9f,
                                        glm::vec3(0,0,1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawSolidCylinder(0.06f,0.06f,1.0f,6,6);
    glPopMatrix();
    
}
// Draw the seat of the bike
void drawSeat(){
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(0.3, -0.2, 0));
        transMatrix = glm::rotate(      transMatrix,
                                        -3.14f,
                                        glm::vec3(0,0,1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawSolidTeapot(.2);
    glPopMatrix();

}
// Draw the chasis of the bike
void drawChasis(){
    glColor3f(0,1,0);
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(0, 0.5, 0));
        glMultMatrixf( &transMatrix[0][0] );
        drawHandleBars();
        drawLowerBar();
        drawSeat();
    glPopMatrix();
    
}
// Draw the front wheel
void drawFrontWheel(){
    glColor3f(0,1,1);
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(-0.35, 0.3, 0));
        // Rotate based on movement
        transMatrix = glm::rotate(      transMatrix,
                                        wheelRotation,
                                        glm::vec3(0,0,1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawWireSphere(.3,6,6);
    glPopMatrix();
    
}
// Draw the back wheel
void drawBackWheel(){
    glColor3f(0,1,1);
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(0.3, 0.15, 0));
        // Rotate based on movement
        transMatrix = glm::rotate(      transMatrix,
                                        wheelRotation,
                                        glm::vec3(0,0,1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawWireSphere(.15,6,6);
    glPopMatrix();
    
}

// Draw the left wing of the fairy
void fairyWingLeft(){
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::rotate(      transMatrix,
                                        (float) (wing_offset + wing_angle*sin(2*M_PI*fairy_time*wing_speed)),
                                        glm::vec3(0,1,0));
        transMatrix = glm::rotate(      transMatrix,
                                        1.0f,
                                        glm::vec3(0,0,-1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawSolidPartialDisk(.1, 2.4, 16,16, 0, 125);
    glPopMatrix();
}

// Draw the right wing of the fairy
void fairyWingRight(){
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::rotate(      transMatrix,
                                        (float) (-wing_offset - wing_angle*sin(2*M_PI*fairy_time*wing_speed)),
                                        glm::vec3(0,1,0));
        transMatrix = glm::rotate(      transMatrix,
                                        1.0f,
                                        glm::vec3(0,0,-1));
        glMultMatrixf( &transMatrix[0][0] );
        CSCI441::drawSolidPartialDisk(.1, 2.4, 16,16, 0, 125);
    glPopMatrix();
}

// Draw the wings of the fairy
void fairyWings(){
    glm::mat4 transMatrix;
    glPushMatrix();
        /*
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(location.x, location.y, location.z));
        transMatrix = glm::scale(       transMatrix,
                                        glm::vec3(0.3f, 0.3f, 0.3f));
          */
        glMultMatrixf( &transMatrix[0][0] );
        fairyWingLeft();
        fairyWingRight();
    glPopMatrix();
}

// Draw the fairy body
void fairyBody(){
    glColor3f(1,0,1);
    fairyWings();
    CSCI441::drawSolidTeapot(1);
}

// Draw the bezier curve and control points
void drawCurve(){
        glm::mat4 trans;
        glm::vec3 point;
        if (toggle_cage){
            for (int i = 0; i < (int)controlPoints.size(); i++){
                glPushMatrix();
                    trans = glm::translate( glm::mat4(),
                                            controlPoints[i]);
                    glMultMatrixf( &trans[0][0] ); 
                        glColor3f(0,1,0);
                        glLoadName( i );
                        CSCI441::drawSolidSphere(.15,10,10);
                glPopMatrix();
            }
            glLineWidth(3.0f);
            glBegin( GL_LINES );
                glColor3f(1.0f,1.0f,0.0f);
                for (int i = 1; i < (int)controlPoints.size(); i++){
                    point = controlPoints[i-1];
                    glVertex3f(point.x, point.y, point.z);
                    point = controlPoints[i];
                    glVertex3f(point.x, point.y, point.z);
                }
            glEnd();
        }
        if (toggle_curve){
            glLineWidth(3.0f);
            for (int i = 0; i < (int)controlPoints.size()/3; i++){
                renderBezierCurve( controlPoints[3*i+0],  controlPoints[3*i+1], controlPoints[3*i+2], controlPoints[3*i+3],  resolution); 
            }
        }
        glLineWidth(0.1f);
}

// Draw the fairy and it's curve
void drawFairy(){
    glm::mat4 transMatrix;
    glm::vec3 location = evaluateBezierCurve(   controlPoints[3*fairy_seg+0],  
                                                controlPoints[3*fairy_seg+1], 
                                                controlPoints[3*fairy_seg+2], 
                                                controlPoints[3*fairy_seg+3], 
                                                fairy_time); 
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(0, .5, 0));
        transMatrix = glm::scale(       transMatrix,
                                         glm::vec3(0.4f, 0.4f, 0.4f));
        glMultMatrixf( &transMatrix[0][0] );
        drawCurve();
        glPushMatrix();
            transMatrix = glm::translate(   glm::mat4(),
                                            glm::vec3(location.x, location.y, location.z));
            // Rotate based on user input
            /*
            transMatrix = glm::rotate(      transMatrix,
                                            vehiclePhi,
                                            glm::vec3(0,-1,0));
            */
            transMatrix = glm::scale(       transMatrix,
                                            glm::vec3(0.5f, 0.5f, 0.5f));
            glMultMatrixf( &transMatrix[0][0] );
            fairyBody();
        glPopMatrix();
    glPopMatrix();
}

// Draw the vehicle
void drawVehicle(){
    glm::mat4 transMatrix;
    glPushMatrix();
        transMatrix = glm::translate(   glm::mat4(),
                                        glm::vec3(vehicleLoc.x, vehicleLoc.y, vehicleLoc.z));
        // Rotate based on user input
        transMatrix = glm::rotate(      transMatrix,
                                        vehiclePhi,
                                        glm::vec3(0,-1,0));
        transMatrix = glm::rotate(      transMatrix,
                                        yaw,
                                        glm::vec3(0,0,1));
        
        transMatrix = glm::scale(       transMatrix,
                                        glm::vec3(0.3f, 0.3f, 0.3f));
        glMultMatrixf( &transMatrix[0][0] );
        drawChasis();
        drawFrontWheel();
        drawBackWheel();

        // Add a helper
        drawFairy();
    glPopMatrix();
}

//
//	void renderScene()
//
//		This method will contain all of the objects to be drawn.
//
//

void renderScene(void)  {
    glCallList( environmentDL );
    drawVehicle();
    drawSpectators();
    drawTrack();
}

void genCamera(){
    switch( veiwport){
        case 0:{
            camRadius   = 2;                // Distance of camera from vehicle
            upper_ang   = 2*M_PI/3;         // Upper Bound of camera angle
            lower_ang   = 4*M_PI/7;         // Lower Bound of camera angle
            zoom        = 0;                // state of control to zoom
            glm::mat4 viewMtx = glm::lookAt(    (vehicleLoc-(camRadius*camDir)),		// camera is located on an arcball
	    	    	        		    vehicleLoc,		// camera is looking at vehilce
	        				    glm::vec3(  0,  1,  0 ) );		// up vector is (0, 1, 0) - positive Y
            // multiply by the look at matrix - this is the same as our view martix
            glMultMatrixf( &viewMtx[0][0] );
            cameraPos = vehicleLoc;
            break;
        }
        case 1:{
            camRadius   = 5;                // Distance of camera from vehicle
            upper_ang   = 2*M_PI/3;         // Upper Bound of camera angle
            lower_ang   = 0;         // Lower Bound of camera angle
            zoom        = 0;                // state of control to zoom
            glm::mat4 viewMtx = glm::lookAt(        vehicleLoc + glm::vec3(0,1,0),		// camera is located at vehicle
                                                    vehicleLoc + camRadius*camDir + glm::vec3(0,1,0),		// camera is looking infront of vehicle
	        				    glm::vec3(  0,  1,  0 ) );		// up vector is (0, 1, 0) - positive Y
            // multiply by the look at matrix - this is the same as our view martix
            glMultMatrixf( &viewMtx[0][0] );
            cameraPos = vehicleLoc;
            break;

        }
        case 2:{
            camRadius   = 5;                // Distance of camera from vehicle
            upper_ang   = 2*M_PI/3;         // Upper Bound of camera angle
            lower_ang   = 0;         // Lower Bound of camera angle
            zoom        = 0;                // state of control to zoom
            glm::mat4 viewMtx = glm::lookAt(        cameraPos,		// camera is located at vehicle
                                                    cameraPos + camRadius*camDir,		// camera is looking infront of vehicle
	        				    glm::vec3(  0,  1,  0 ) );		// up vector is (0, 1, 0) - positive Y
            // multiply by the look at matrix - this is the same as our view martix
            glMultMatrixf( &viewMtx[0][0] );
            break;

        }
    }
}

//*************************************************************************************
//
// Setup Functions

//
//  void setupGLFW()
//
//      Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
GLFWwindow* setupGLFW() {
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback( error_callback );

	// initialize GLFW
	if( !glfwInit() ) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW initialized\n" );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );	// request OpenGL v2.X
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	// request OpenGL v2.1
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );		// do not allow our window to be able to be resized

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, "Teapot Biking v 1.1.1.1.1", NULL, NULL );
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(window);		// make the created window the current window
	glfwSwapInterval(1);				     	// update our screen after at least 1 screen refresh

	glfwSetKeyCallback( window, keyboard_callback );							// set our keyboard callback function
	glfwSetCursorPosCallback( window, cursor_callback );					// set our cursor position callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function

	return window;						       // return the window that was created
}

//
//  void setupOpenGL()
//
//      Used to setup everything OpenGL related.  For now, the only setting
//	we need is what color to make the background of our window when we clear
//	the window.  In the future we will be adding many more settings to this
//	function.
//
void setupOpenGL() {
	// tell OpenGL to perform depth testing with the Z-Buffer to perform hidden
	//		surface removal.  We will discuss this more very soon.
	glEnable( GL_DEPTH_TEST );

	//******************************************************************
	// this is some code to enable a default light for the scene;
	// feel free to play around with this, but we won't talk about
	// lighting in OpenGL for another couple of weeks yet.
	float lightCol[4] = { 1, 1, 1, 1};
	float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
	float lPosition[4] = { 10, 10, 10, 1 };
	glLightfv( GL_LIGHT0, GL_POSITION,lPosition );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,lightCol );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	// tell OpenGL not to use the material system; just use whatever we
	// pass with glColor*()
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	//******************************************************************

	// tells OpenGL to blend colors across triangles. Once lighting is
	// enabled, this means that objects will appear smoother - if your object
	// is rounded or has a smooth surface, this is probably a good idea;
	// if your object has a blocky surface, you probably want to disable this.
	glShadeModel( GL_SMOOTH );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// set the clear color to black
}

//
//  void setupScene()
//
//      Used to setup everything scene related.  Give our camera an
//	initial starting point and generate the display list for our city
//
void setupScene() {
	// give the camera a scenic starting point.
	vehicleLoc.x = 0;
	vehicleLoc.y = 0;
	vehicleLoc.z = 0;
	cameraTheta = -M_PI / 3.0f;
	cameraPhi = 4*M_PI / 7;
	recomputeOrientation();

	srand( time(NULL) );	// seed our random number generator
	generateEnvironmentDL();


}

///*************************************************************************************
//
// Our main function

//
//	int main( int argc, char *argv[] )
//
//		Really you should know what this is by now.  We will make use of the parameters later
//
int main( int argc, char *argv[] ) {
        // Load control points
        if (argc < 3){
            fprintf(stdout, "No File Loaded. Use Command line arguments to load a file\n");
        }
        else{
            fprintf(stdout, "File %s successfully loaded\n", argv[1]);
            loadTerrain(argv[2], &sAttr);
            loadFaeryControlPoints(argv[1]);
            loadControlPoints(argv[1]);
        }
	// GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();										// initialize all of the OpenGL specific information
	setupScene();											// initialize ovehicleLoc+camDir
	//  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.
	while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
		updateState();                                          // Update internal variables
		glDrawBuffer( GL_BACK );				// work with our back frame buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

		// update the projection matrix based on the window size
		// the GL_PROJECTION matrix governs properties of the view coordinates;
		// i.e. what gets seen - use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projMtx = glm::perspective( 45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.001f, 1000.0f );
		glMatrixMode( GL_PROJECTION );	// change to the Projection matrix
		glLoadIdentity();				// set the matrix to be the identity
		glMultMatrixf( &projMtx[0][0] );// load our orthographic projection matrix into OpenGL's projection matrix state

		// Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.
		GLint framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

		// update the viewport - tell OpenGL we want to render to the whole window
		glViewport( 0, 0, framebufferWidth, framebufferHeight );

		glMatrixMode( GL_MODELVIEW );	// make the ModelView matrix current to be modified by any transformations
		glLoadIdentity();							// set the matrix to be the identity

		// set up our look at matrix to position our camera
		// TODO #6: Change how our lookAt matrix gets constructed
                genCamera();
                renderScene();					// draw everything to the window

		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen
	}

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context
}
