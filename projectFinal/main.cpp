#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char* WINDOWTITLE = "Final Project - Michael Hwang";
const char* GLUITITLE = "User Interface Window";

float Time = 0;
int pastMS;
#define MS_PER_CYCLE	700;
const int RANDOM_MOVES = 20;

float animTimeRemaining = 0;
enum ANIM {
	NONE,
	FRONT_FACE,
	BACK_FACE,
	RIGHT_FACE,
	LEFT_FACE,
	UP_FACE,
	DOWN_FACE,
	STANDING_SLICE,
	MIDDLE_SLICE,
	EQUATOR_SLICE,
	FRONT_FACE_I,
	BACK_FACE_I,
	RIGHT_FACE_I,
	LEFT_FACE_I,
	UP_FACE_I,
	DOWN_FACE_I,
	STANDING_SLICE_I,
	MIDDLE_SLICE_I,
	EQUATOR_SLICE_I
};

enum ANIM currentAnimation = NONE;

// what the glui package defines as true and false:
const int NUMPOINTS = { 10 };
const int NUMCURVES = { 10 };

const float UNIT = { .5 };

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0.3, 0.3, 0.3, 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char* ColorNames[] =
{
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

struct sColor {
	float r, g, b;
};

sColor red = { .8, 0., 0. };
sColor orange = { 1., .6, .1 };
sColor green = { 0., .8, 0. };
sColor blue = { 0., 0., 8. };
sColor white = { 1., 1., 1. };
sColor yellow = { .9, .9, .1 };
sColor black = { 0., 0., 0. };

struct block {
	sColor front, back, right, left, up, down;
};

struct position {
	float x, y, z;
};

struct rotation {
	float v, ax, ay, az;
};

struct state {
	int frontFace[9];
	int backFace[9];
	int rightFace[9];
	int leftFace[9];
	int upFace[9];
	int downFace[9];
	int standingSlice[8];
	int middleSlice[8];
	int equatorSlice[8];
};

block blockColors[] = {
	{orange, black, black, black, black, black},  // 0
	{black, red, black, black, black, black},			// 1
	{black, black, blue, black, black, black},	// 2
	{black, black, black, green, black, black},		// 3
	{black, black, black, black, yellow, black}, 	// 4
	{black, black, black, black, black, white},		// 5
	{black, red, black, green, yellow, black},			// 6
	{black, red, black, black, yellow, black}, 		// 7
	{black, red, blue, black, yellow, black},		// 8
	{black, black, blue, black, yellow, black},	// 9
	{orange, black, blue, black, yellow, black},	// 10
	{orange, black, black, black, yellow, black},	// 11
	{orange, black, black, green, yellow, black},	// 12
	{black, black, black, green, yellow, black},	 	// 13
	{black, red, black, green, black, black},			// 14
	{black, red, blue, black, black, black},		// 15
	{orange, black, blue, black, black, black},	// 16
	{orange, black, black, green, black, black},	// 17
	{black, red, black, green, black, white},			// 18
	{black, red, black, black, black, white},			// 19
	{black, red, blue, black, black, white},			// 20
	{black, black, blue, black, black, white},		// 21
	{orange, black, blue, black, black, white},	// 22
	{orange, black, black, black, black, white},		// 23
	{orange, black, black, green, black, white},		// 24
	{black, black, black, green, black, white}			// 25
};

//OOOF OUCH
position currentPositions[] = {
	{ 0.,      0.,      UNIT * 2 },  // front
	{ 0.,      0.,     -UNIT * 2 },  // back
	{ UNIT * 2,  0.,      0.     },  // right
	{-UNIT * 2,  0.,      0.     },  // left
	{ 0.,      UNIT * 2,  0.     },  // up
	{ 0.,     -UNIT * 2,  0.     },  // down
	{-UNIT * 2,  UNIT * 2, -UNIT * 2 },  // up, back, left 6
	{ 0.,      UNIT * 2, -UNIT * 2 },  // up, back 7
	{ UNIT * 2,  UNIT * 2, -UNIT * 2 },  // up, back, right 8
	{ UNIT * 2,  UNIT * 2,  0.     },  // up, right 9
	{ UNIT * 2,  UNIT * 2,  UNIT * 2 },  // up, right, front 10
	{ 0.,      UNIT * 2,  UNIT * 2 },  // up, front 11
	{-UNIT * 2,  UNIT * 2,  UNIT * 2 },  // up, front, left 12
	{-UNIT * 2,  UNIT * 2,  0.     },  // up, left 13
	{-UNIT * 2,  0.,     -UNIT * 2 },  // back, left 14
	{ UNIT * 2,  0.,     -UNIT * 2 },  // back, right 15
	{ UNIT * 2,  0.,      UNIT * 2 },  // right, front 16
	{-UNIT * 2,  0.,      UNIT * 2 },  // left, front 17
	{-UNIT * 2, -UNIT * 2, -UNIT * 2 },  // down, left, back 18
	{ 0.,     -UNIT * 2, -UNIT * 2 },  // down, back 19
	{ UNIT * 2, -UNIT * 2, -UNIT * 2 },  // down, right, back 20
	{ UNIT * 2, -UNIT * 2,  0.     },  // down, right 21
	{ UNIT * 2, -UNIT * 2,  UNIT * 2 },  // down, front, right 22
	{ 0.,     -UNIT * 2,  UNIT * 2 },  // down, front 23
	{-UNIT * 2, -UNIT * 2,  UNIT * 2 },  // down, front, left 24
	{-UNIT * 2, -UNIT * 2,  0.     }   // down, left 25
};

position originalPositions[] = {
	{ 0.,      0.,      UNIT * 2 },
	{ 0.,      0.,     -UNIT * 2 },
	{ UNIT * 2,  0.,      0.     },
	{-UNIT * 2,  0.,      0.     },
	{ 0.,      UNIT * 2,  0.     },
	{ 0.,     -UNIT * 2,  0.     },
	{-UNIT * 2,  UNIT * 2, -UNIT * 2 },
	{ 0.,      UNIT * 2, -UNIT * 2 },
	{ UNIT * 2,  UNIT * 2, -UNIT * 2 },
	{ UNIT * 2,  UNIT * 2,  0.     },
	{ UNIT * 2,  UNIT * 2,  UNIT * 2 },
	{ 0.,      UNIT * 2,  UNIT * 2 },
	{-UNIT * 2,  UNIT * 2,  UNIT * 2 },
	{-UNIT * 2,  UNIT * 2,  0.     },
	{-UNIT * 2,  0.,     -UNIT * 2 },
	{ UNIT * 2,  0.,     -UNIT * 2 },
	{ UNIT * 2,  0.,      UNIT * 2 },
	{-UNIT * 2,  0.,      UNIT * 2 },
	{-UNIT * 2, -UNIT * 2, -UNIT * 2 },
	{ 0.,     -UNIT * 2, -UNIT * 2 },
	{ UNIT * 2, -UNIT * 2, -UNIT * 2 },
	{ UNIT * 2, -UNIT * 2,  0.     },
	{ UNIT * 2, -UNIT * 2,  UNIT * 2 },
	{ 0.,     -UNIT * 2,  UNIT * 2 },
	{-UNIT * 2, -UNIT * 2,  UNIT * 2 },
	{-UNIT * 2, -UNIT * 2,  0.     }
};

rotation rotationAnimation[] = {
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.},
	{0., 0., 0., 0.}
};

state cubeState = {
	{0, 10, 11, 12, 16, 17, 22, 23, 24}, 				//Front
	{1, 6, 7, 8, 14, 15, 18, 19, 20}, 					//Back
	{2, 8, 9, 10, 15, 16, 20, 21, 22}, 					//Right
	{3, 6, 12, 13, 14, 17, 18, 24, 25}, 				//Left
	{4, 6, 7, 8, 9, 10, 11, 12, 13}, 						//Top
	{5, 18, 19, 20, 21, 22, 23, 24, 25}, 				//Bottom
	{2, 3, 4, 5, 9, 13, 21, 25}, 								//Standing
	{0, 1, 4, 5, 7, 11, 19, 23}, 								//Middle
	{0, 1, 2, 3, 14, 15, 16, 17}  							//Equator
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong

//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

bool freezeAnimation = false;
bool enableLables = true;
bool displayPoints = false;
bool displayEdges = false;

// function prototypes:

void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);

void			Axes(float);

unsigned char* BmpToTexture(char*, int*, int*);
int				ReadInt(FILE*);
short			ReadShort(FILE*);

void			HsvRgb(float[3], float[3]);

void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);

void  DoDisplayLabelMenu(int);
void  DoDisplayPointMenu(int);
void  DoDisplayEdgeMenu(int);

void DrawCube(block);
void drawRubikCube();
void myTranslate(position);
void myRotate(rotation);
void clearAnimations();

void front(bool inverse);
void back(bool inverse);
void right(bool inverse);
void left(bool inverse);
void up(bool inverse);
void down(bool inverse);
void standing(bool inverse);
void middle(bool inverse);
void equator(bool inverse);

void frontColorSwitch(bool inverse);
void backColorSwitch(bool inverse);
void rightColorSwitch(bool inverse);
void leftColorSwitch(bool inverse);
void upColorSwitch(bool inverse);
void downColorSwitch(bool inverse);
void standingColorSwitch(bool inverse);
void middleColorSwitch(bool inverse);
void equatorColorSwitch(bool inverse);

void randomizer();
void clearColors();

// main program:

int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics();

	// create the display structures that will not change:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface stuff:

	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:
	int ms = glutGet(GLUT_ELAPSED_TIME);
	Time += (float)(ms - pastMS) / (float)MS_PER_CYCLE;
	pastMS = ms;
	// force a call to Display( ) next time it is convenient:
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}


	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (DepthBufferOn != 0)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	// specify shading to be flat:

	glShadeModel(GL_FLAT);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);


	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// glLightModelfv( GL_LIGHT_MODEL_AMBIENT, MulArray3( .2, White ) );
	// glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

	// Give us some light
	// Point light
	// SetPointLight(GL_LIGHT0, 0., 3., 2., .5, .5, .5);

	// set the eye position, look-at position, and up-vector:
	glEnable(GL_NORMALIZE);
	gluLookAt(0., 2., 5., 0., 0., 0., 0., 1., 0.);
	//
	// glEnable( GL_LIGHTING );
	//
	// glEnable( GL_LIGHT0 );


	// rotate the scene:

	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);


	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}

	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable(GL_NORMALIZE);

	glEnable(GL_TEXTURE_2D);

	// Do animation calculation
	// Update any faces and positions
	// Render and excute animations
	if (currentAnimation != NONE) {
		if (Time > .99) {
			// Out of time on animation
			// Switch colors and sup animation
			clearAnimations();
			glutIdleFunc(NULL);
			Time = 0;
			switch (currentAnimation) {
			case FRONT_FACE:
				frontColorSwitch(false);
				break;
			case FRONT_FACE_I:
				frontColorSwitch(true);
				break;
			case BACK_FACE:
				backColorSwitch(false);
				break;
			case BACK_FACE_I:
				backColorSwitch(true);
				break;
			case RIGHT_FACE:
				rightColorSwitch(false);
				break;
			case RIGHT_FACE_I:
				rightColorSwitch(true);
				break;
			case LEFT_FACE:
				leftColorSwitch(false);
				break;
			case LEFT_FACE_I:
				leftColorSwitch(true);
				break;
			case UP_FACE:
				upColorSwitch(false);
				break;
			case UP_FACE_I:
				upColorSwitch(true);
				break;
			case DOWN_FACE:
				downColorSwitch(false);
				break;
			case DOWN_FACE_I:
				downColorSwitch(true);
				break;
			case STANDING_SLICE:
				standingColorSwitch(false);
				break;
			case STANDING_SLICE_I:
				standingColorSwitch(true);
				break;
			case MIDDLE_SLICE:
				middleColorSwitch(false);
				break;
			case MIDDLE_SLICE_I:
				middleColorSwitch(true);
				break;
			case EQUATOR_SLICE:
				equatorColorSwitch(false);
				break;
			case EQUATOR_SLICE_I:
				equatorColorSwitch(true);
				break;
			}
			currentAnimation = NONE;
		}
		else {
			// Animation continues
			switch (currentAnimation) {
			case FRONT_FACE:
				front(false);
				break;
			case FRONT_FACE_I:
				front(true);
				break;
			case BACK_FACE:
				back(false);
				break;
			case BACK_FACE_I:
				back(true);
				break;
			case RIGHT_FACE:
				right(false);
				break;
			case RIGHT_FACE_I:
				right(true);
				break;
			case LEFT_FACE:
				left(false);
				break;
			case LEFT_FACE_I:
				left(true);
				break;
			case UP_FACE:
				up(false);
				break;
			case UP_FACE_I:
				up(true);
				break;
			case DOWN_FACE:
				down(false);
				break;
			case DOWN_FACE_I:
				down(true);
				break;
			case STANDING_SLICE:
				standing(false);
				break;
			case STANDING_SLICE_I:
				standing(true);
				break;
			case MIDDLE_SLICE:
				middle(false);
				break;
			case MIDDLE_SLICE_I:
				middle(true);
				break;
			case EQUATOR_SLICE:
				equator(false);
				break;
			case EQUATOR_SLICE_I:
				equator(true);
				break;
			}
		}
	}

	drawRubikCube();
	// front, back, right, left, up, down



	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90., 0., 1., 0.);
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);

	if (enableLables) {
		glDisable(GL_DEPTH_TEST);
		glColor3f(0., 0., 0.);
		DoRasterString(0., UNIT * 3, 0., "Up face");
		DoRasterString(UNIT * 3, 0., 0., "Right face");
		DoRasterString(0., 0., UNIT * 3, "Front face");
	}


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., 100., 0., 100.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (enableLables) {
		glColor3f(1., 1., 1.);
		DoRasterString(5., 5., 0., "'[' for randomize, ']' for reset");
	}


	// swap the double-buffered framebuffers:

	glutSwapBuffers();


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	WhichColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoDisplayLabelMenu(int id) {
	enableLables = id;
	glutPostRedisplay();
}

void
DoDisplayPointMenu(int id) {
	displayPoints = id;
	glutPostRedisplay();
}

void
DoDisplayEdgeMenu(int id) {
	displayEdges = id;
	glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(int));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int labelmenu = glutCreateMenu(DoDisplayLabelMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int edgemenu = glutCreateMenu(DoDisplayEdgeMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int pointmenu = glutCreateMenu(DoDisplayPointMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Colors", colormenu);
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddSubMenu("Display Labels", labelmenu);
	glutAddSubMenu("Display Points", pointmenu);
	glutAddSubMenu("Display Edges", edgemenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);
	glutIdleFunc(NULL);


	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
	glutSetWindow(MainWindow);

	float r = 1.5;
	int slices = 32;
	int stacks = 32;

	// create the axes:

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'f':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = FRONT_FACE;
		}
		break;
	case 'F':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = FRONT_FACE_I;
		}
		break;
	case 'b':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = BACK_FACE;
		}
		break;
	case 'B':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = BACK_FACE_I;
		}
		break;
	case 'r':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = RIGHT_FACE;
		}
		break;
	case 'R':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = RIGHT_FACE_I;
		}
		break;
	case 'l':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = LEFT_FACE;
		}
		break;
	case 'L':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = LEFT_FACE_I;
		}
		break;
	case 'u':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = UP_FACE;
		}
		break;
	case 'U':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = UP_FACE_I;
		}
		break;
	case 'd':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = DOWN_FACE;
		}
		break;
	case 'D':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = DOWN_FACE_I;
		}
		break;
	case 's':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = STANDING_SLICE;
		}
		break;
	case 'S':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = STANDING_SLICE_I;
		}
		break;
	case 'm':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = MIDDLE_SLICE;
		}
		break;
	case 'M':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = MIDDLE_SLICE_I;
		}
		break;
	case 'e':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = EQUATOR_SLICE;
		}
		break;
	case 'E':
		if (currentAnimation == NONE) {
			pastMS = glutGet(GLUT_ELAPSED_TIME);
			glutIdleFunc(Animate);
			currentAnimation = EQUATOR_SLICE_I;
		}
		break;

	case '[':
		if (currentAnimation == NONE)
			randomizer();
		break;

	case ']':
		if (currentAnimation == NONE)
			clearColors();
		break;

	case '`':
		freezeAnimation = !freezeAnimation;
		if (freezeAnimation)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////
void myTranslate(struct position t) {
	glTranslatef(t.x, t.y, t.z);
}

void myRotate(struct rotation r) {
	glRotatef(r.v, r.ax, r.ay, r.az);
}

void
DrawCube(struct block i) {
	glColor3f(i.front.r, i.front.g, i.front.b);
	glNormal3f(0., 1., 0.);
	glBegin(GL_QUADS);
	glVertex3f(-UNIT, UNIT, UNIT);
	glVertex3f(UNIT, UNIT, UNIT);
	glVertex3f(UNIT, -UNIT, UNIT);
	glVertex3f(-UNIT, -UNIT, UNIT);
	glEnd();

	glColor3f(i.back.r, i.back.g, i.back.b);
	glNormal3f(0., -1., 0.);
	glBegin(GL_QUADS);
	glVertex3f(-UNIT, UNIT, -UNIT);
	glVertex3f(UNIT, UNIT, -UNIT);
	glVertex3f(UNIT, -UNIT, -UNIT);
	glVertex3f(-UNIT, -UNIT, -UNIT);
	glEnd();

	glColor3f(i.left.r, i.left.g, i.left.b);
	glNormal3f(-1., 0., 0.);
	glBegin(GL_QUADS);
	glVertex3f(-UNIT, UNIT, -UNIT);
	glVertex3f(-UNIT, UNIT, UNIT);
	glVertex3f(-UNIT, -UNIT, UNIT);
	glVertex3f(-UNIT, -UNIT, -UNIT);
	glEnd();

	glColor3f(i.right.r, i.right.g, i.right.b);
	glNormal3f(1., 0., 0.);
	glBegin(GL_QUADS);
	glVertex3f(UNIT, UNIT, -UNIT);
	glVertex3f(UNIT, UNIT, UNIT);
	glVertex3f(UNIT, -UNIT, UNIT);
	glVertex3f(UNIT, -UNIT, -UNIT);
	glEnd();

	glColor3f(i.up.r, i.up.g, i.up.b);
	glNormal3f(0., 1., 0.);
	glBegin(GL_QUADS);
	glVertex3f(-UNIT, UNIT, -UNIT);
	glVertex3f(UNIT, UNIT, -UNIT);
	glVertex3f(UNIT, UNIT, UNIT);
	glVertex3f(-UNIT, UNIT, UNIT);
	glEnd();

	glColor3f(i.down.r, i.down.g, i.down.b);
	glNormal3f(0., -1., 0.);
	glBegin(GL_QUADS);
	glVertex3f(-UNIT, -UNIT, -UNIT);
	glVertex3f(UNIT, -UNIT, -UNIT);
	glVertex3f(UNIT, -UNIT, UNIT);
	glVertex3f(-UNIT, -UNIT, UNIT);
	glEnd();

}

void drawRubikCube()
{
	// centers
	for (int i = 0; i < 26; i++) {
		glPushMatrix();
		myRotate(rotationAnimation[i]);
		myTranslate(currentPositions[i]);
		DrawCube(blockColors[i]);
		glPopMatrix();
	}
}

void front(bool inverse)
{
	// make animation
	int inversor = (inverse) ? 1 : -1;
	for (int i = 0; i < 9; i++) {
		int blockIndex = cubeState.frontFace[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 0.;
		rotationAnimation[blockIndex].ay = 0.;
		rotationAnimation[blockIndex].az = 1.;
	}
}

void back(bool inverse)
{
	int inversor = (inverse) ? -1 : 1;
	for (int i = 0; i < 9; i++) {
		int blockIndex = cubeState.backFace[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 0.;
		rotationAnimation[blockIndex].ay = 0.;
		rotationAnimation[blockIndex].az = 1.;
	}
}

void right(bool inverse) {
	int inversor = (inverse) ? 1 : -1;
	for (int i = 0; i < 9; i++) {
		int blockIndex = cubeState.rightFace[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 1.;
		rotationAnimation[blockIndex].ay = 0.;
		rotationAnimation[blockIndex].az = 0.;
	}
}

void left(bool inverse) {
	int inversor = (inverse) ? -1 : 1;
	for (int i = 0; i < 9; i++) {
		int blockIndex = cubeState.leftFace[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 1.;
		rotationAnimation[blockIndex].ay = 0.;
		rotationAnimation[blockIndex].az = 0.;
	}
}

void up(bool inverse) {
	int inversor = (inverse) ? 1 : -1;
	for (int i = 0; i < 9; i++) {
		int blockIndex = cubeState.upFace[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 0.;
		rotationAnimation[blockIndex].ay = 1.;
		rotationAnimation[blockIndex].az = 0.;
	}
}

void down(bool inverse) {
	int inversor = (inverse) ? -1 : 1;
	for (int i = 0; i < 9; i++) {
		int blockIndex = cubeState.downFace[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 0.;
		rotationAnimation[blockIndex].ay = 1.;
		rotationAnimation[blockIndex].az = 0.;
	}
}

void standing(bool inverse)
{
	// make animation
	int inversor = (inverse) ? 1 : -1;
	for (int i = 0; i < 8; i++) {
		int blockIndex = cubeState.standingSlice[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 0.;
		rotationAnimation[blockIndex].ay = 0.;
		rotationAnimation[blockIndex].az = 1.;
	}
}

void middle(bool inverse) {
	int inversor = (inverse) ? 1 : -1;
	for (int i = 0; i < 8; i++) {
		int blockIndex = cubeState.middleSlice[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 1.;
		rotationAnimation[blockIndex].ay = 0.;
		rotationAnimation[blockIndex].az = 0.;
	}
}

void equator(bool inverse) {
	int inversor = (inverse) ? 1 : -1;
	for (int i = 0; i < 8; i++) {
		int blockIndex = cubeState.equatorSlice[i];
		rotationAnimation[blockIndex].v = 90. * Time * inversor;
		rotationAnimation[blockIndex].ax = 0.;
		rotationAnimation[blockIndex].ay = 1.;
		rotationAnimation[blockIndex].az = 0.;
	}
}

void clearAnimations() {
	// stop animation
	for (int i = 0; i < 26; i++) {
		rotationAnimation[i].v = 0.;
		rotationAnimation[i].ax = 0.;
		rotationAnimation[i].ay = 0.;
		rotationAnimation[i].az = 0.;
	}
}

void clearColors() {
	for (int i = 0; i < 26; i++) {
		blockColors[i].front = black;
		blockColors[i].back = black;
		blockColors[i].right = black;
		blockColors[i].left = black;
		blockColors[i].up = black;
		blockColors[i].down = black;
	}

	for (int i = 0; i < 9; i++) {
		blockColors[cubeState.frontFace[i]].front = orange;
		blockColors[cubeState.backFace[i]].back = red;
		blockColors[cubeState.rightFace[i]].right = blue;
		blockColors[cubeState.leftFace[i]].left = green;
		blockColors[cubeState.upFace[i]].up = yellow;
		blockColors[cubeState.downFace[i]].down = white;
	}
}

void randomizer() {
	for (size_t i = 0; i < RANDOM_MOVES; i++) {
		int r = rand() % 18;
		int r2 = rand() % 2;
		while (r2 < 2) {
			switch (r) {
			case 0:
			case 1:
				frontColorSwitch(r);
				break;
			case 2:
			case 3:
				backColorSwitch(r % 2);
				break;
			case 4:
			case 5:
				rightColorSwitch(r % 2);
				break;
			case 6:
			case 7:
				leftColorSwitch(r % 2);
				break;
			case 8:
			case 9:
				upColorSwitch(r % 2);
				break;
			case 10:
			case 11:
				downColorSwitch(r % 2);
				break;
			case 12:
			case 13:
				standingColorSwitch(r % 2);
				break;
			case 14:
			case 15:
				middleColorSwitch(r % 2);
				break;
			case 16:
			case 17:
				equatorColorSwitch(r % 2);
				break;
			}
			r2++;
		}
	}
}

void applyColor(int index, sColor front, sColor back, sColor right, sColor left, sColor up, sColor down) {
	blockColors[index].front.r = front.r;
	blockColors[index].front.g = front.g;
	blockColors[index].front.b = front.b;

	blockColors[index].back.r = back.r;
	blockColors[index].back.g = back.g;
	blockColors[index].back.b = back.b;

	blockColors[index].right.r = right.r;
	blockColors[index].right.g = right.g;
	blockColors[index].right.b = right.b;

	blockColors[index].left.r = left.r;
	blockColors[index].left.g = left.g;
	blockColors[index].left.b = left.b;

	blockColors[index].up.r = up.r;
	blockColors[index].up.g = up.g;
	blockColors[index].up.b = up.b;

	blockColors[index].down.r = down.r;
	blockColors[index].down.g = down.g;
	blockColors[index].down.b = down.b;
}

void frontColorSwitch(bool inverse) {
	if (!inverse) {
		sColor temp1 = blockColors[10].front;
		sColor temp2 = blockColors[10].right;
		sColor temp3 = blockColors[10].up;

		// Corners
		applyColor(10, blockColors[12].front, black, blockColors[12].up, black, blockColors[12].left, black);
		applyColor(12, blockColors[24].front, black, black, blockColors[24].down, blockColors[24].left, black);
		applyColor(24, blockColors[22].front, black, black, blockColors[22].down, black, blockColors[22].right);
		applyColor(22, temp1, black, temp3, black, black, temp2);

		// Sides
		sColor sideTemp1 = blockColors[11].front;
		sColor sideTemp2 = blockColors[11].up;

		applyColor(11, blockColors[17].front, black, black, black, blockColors[17].left, black);
		applyColor(17, blockColors[23].front, black, black, blockColors[23].down, black, black);
		applyColor(23, blockColors[16].front, black, black, black, black, blockColors[16].right);
		applyColor(16, sideTemp1, black, sideTemp2, black, black, black);
	}
	else {
		sColor temp1 = blockColors[10].front;
		sColor temp2 = blockColors[10].right;
		sColor temp3 = blockColors[10].up;

		// Corners
		applyColor(10, blockColors[22].front, black, blockColors[22].down, black, blockColors[22].right, black);
		applyColor(22, blockColors[24].front, black, blockColors[24].down, black, black, blockColors[24].left);
		applyColor(24, blockColors[12].front, black, black, blockColors[12].up, black, blockColors[12].left);
		applyColor(12, temp1, black, black, temp3, temp2, black);

		// Sides
		sColor sideTemp1 = blockColors[11].front;
		sColor sideTemp2 = blockColors[11].up;

		applyColor(11, blockColors[16].front, black, black, black, blockColors[16].right, black);
		applyColor(16, blockColors[23].front, black, blockColors[23].down, black, black, black);
		applyColor(23, blockColors[17].front, black, black, black, black, blockColors[17].left);
		applyColor(17, sideTemp1, black, black, sideTemp2, black, black);
	}
}

void backColorSwitch(bool inverse)
{
	if (!inverse) {
		sColor temp1 = blockColors[6].back;
		sColor temp2 = blockColors[6].left;
		sColor temp3 = blockColors[6].up;

		// Corners
		applyColor(6, black, blockColors[8].back, black, blockColors[8].up, blockColors[8].right, black);
		applyColor(8, black, blockColors[20].back, blockColors[20].down, black, blockColors[20].right, black);
		applyColor(20, black, blockColors[18].back, blockColors[18].down, black, black, blockColors[18].left);
		applyColor(18, black, temp1, black, temp3, black, temp2);

		// Sides
		sColor sideTemp1 = blockColors[7].back;
		sColor sideTemp2 = blockColors[7].up;

		applyColor(7, black, blockColors[15].back, black, black, blockColors[15].right, black);
		applyColor(15, black, blockColors[19].back, blockColors[19].down, black, black, black);
		applyColor(19, black, blockColors[14].back, black, black, black, blockColors[14].left);
		applyColor(14, black, sideTemp1, black, sideTemp2, black, black);
	}
	else {
		sColor temp1 = blockColors[6].back;
		sColor temp2 = blockColors[6].left;
		sColor temp3 = blockColors[6].up;

		// Corners
		applyColor(6, black, blockColors[18].back, black, blockColors[18].down, blockColors[18].left, black);
		applyColor(18, black, blockColors[20].back, black, blockColors[20].down, black, blockColors[20].right);
		applyColor(20, black, blockColors[8].back, blockColors[8].up, black, black, blockColors[8].right);
		applyColor(8, black, temp1, temp3, black, temp2, black);

		// Sides
		sColor sideTemp1 = blockColors[7].back;
		sColor sideTemp2 = blockColors[7].up;

		applyColor(7, black, blockColors[14].back, black, black, blockColors[14].left, black);
		applyColor(14, black, blockColors[19].back, black, blockColors[19].down, black, black);
		applyColor(19, black, blockColors[15].back, black, black, black, blockColors[15].right);
		applyColor(15, black, sideTemp1, sideTemp2, black, black, black);
	}
}

void rightColorSwitch(bool inverse) {
	if (!inverse) {
		sColor temp1 = blockColors[10].front;
		sColor temp2 = blockColors[10].right;
		sColor temp3 = blockColors[10].up;

		// Corners
		applyColor(10, blockColors[22].down, black, blockColors[22].right, black, blockColors[22].front, black);
		applyColor(22, blockColors[20].down, black, blockColors[20].right, black, black, blockColors[20].back);
		applyColor(20, black, blockColors[8].up, blockColors[8].right, black, black, blockColors[8].back);
		applyColor(8, black, temp3, temp2, black, temp1, black);

		// Sides
		sColor sideTemp1 = blockColors[9].right;
		sColor sideTemp2 = blockColors[9].up;

		applyColor(9, black, black, blockColors[16].right, black, blockColors[16].front, black);
		applyColor(16, blockColors[21].down, black, blockColors[21].right, black, black, black);
		applyColor(21, black, black, blockColors[15].right, black, black, blockColors[15].back);
		applyColor(15, black, sideTemp2, sideTemp1, black, black, black);
	}
	else {
		sColor temp1 = blockColors[10].front;
		sColor temp2 = blockColors[10].right;
		sColor temp3 = blockColors[10].up;

		// Corners
		applyColor(10, blockColors[8].up, black, blockColors[8].right, black, blockColors[8].back, black);
		applyColor(8, black, blockColors[20].down, blockColors[20].right, black, blockColors[20].back, black);
		applyColor(20, black, blockColors[22].down, blockColors[22].right, black, black, blockColors[22].front);
		applyColor(22, temp3, black, temp2, black, black, temp1);

		// Sides
		sColor sideTemp1 = blockColors[9].right;
		sColor sideTemp2 = blockColors[9].up;

		applyColor(9, black, black, blockColors[15].right, black, blockColors[15].back, black);
		applyColor(15, black, blockColors[21].down, blockColors[21].right, black, black, black);
		applyColor(21, black, black, blockColors[16].right, black, black, blockColors[16].front);
		applyColor(16, sideTemp2, black, sideTemp1, black, black, black);
	}
}

void leftColorSwitch(bool inverse)
{
	if (!inverse) {
		sColor temp1 = blockColors[12].front;
		sColor temp2 = blockColors[12].left;
		sColor temp3 = blockColors[12].up;

		// Corners
		applyColor(12, blockColors[6].up, black, black, blockColors[6].left, blockColors[6].back, black);
		applyColor(6, black, blockColors[18].down, black, blockColors[18].left, blockColors[18].back, black);
		applyColor(18, black, blockColors[24].down, black, blockColors[24].left, black, blockColors[24].front);
		applyColor(24, temp3, black, black, temp2, black, temp1);

		// Sides
		sColor sideTemp1 = blockColors[13].left;
		sColor sideTemp2 = blockColors[13].up;

		applyColor(13, black, black, black, blockColors[14].left, blockColors[14].back, black);
		applyColor(14, black, blockColors[25].down, black, blockColors[25].left, black, black);
		applyColor(25, black, black, black, blockColors[17].left, black, blockColors[17].front);
		applyColor(17, sideTemp2, black, black, sideTemp1, black, black);
	}
	else {
		sColor temp1 = blockColors[12].front;
		sColor temp2 = blockColors[12].left;
		sColor temp3 = blockColors[12].up;

		// Corners
		applyColor(12, blockColors[24].down, black, black, blockColors[24].left, blockColors[24].front, black);
		applyColor(24, blockColors[18].down, black, black, blockColors[18].left, black, blockColors[18].back);
		applyColor(18, black, blockColors[6].up, black, blockColors[6].left, black, blockColors[6].back);
		applyColor(6, black, temp3, black, temp2, temp1, black);

		// Sides
		sColor sideTemp1 = blockColors[13].left;
		sColor sideTemp2 = blockColors[13].up;

		applyColor(13, black, black, black, blockColors[17].left, blockColors[17].front, black);
		applyColor(17, blockColors[25].down, black, black, blockColors[25].left, black, black);
		applyColor(25, black, black, black, blockColors[14].left, black, blockColors[14].back);
		applyColor(14, black, sideTemp2, black, sideTemp1, black, black);
	}
}

void upColorSwitch(bool inverse)
{
	if (!inverse) {
		sColor temp1 = blockColors[10].front;
		sColor temp2 = blockColors[10].right;
		sColor temp3 = blockColors[10].up;

		// Corners
		applyColor(10, blockColors[8].right, black, blockColors[8].back, black, blockColors[8].up, black);
		applyColor(8, black, blockColors[6].left, blockColors[6].back, black, blockColors[6].up, black);
		applyColor(6, black, blockColors[12].left, black, blockColors[12].front, blockColors[12].up, black);
		applyColor(12, temp2, black, black, temp1, temp3, black);

		// Sides
		sColor sideTemp1 = blockColors[11].front;
		sColor sideTemp2 = blockColors[11].up;

		applyColor(11, blockColors[9].right, black, black, black, blockColors[9].up, black);
		applyColor(9, black, black, blockColors[7].back, black, blockColors[7].up, black);
		applyColor(7, black, blockColors[13].left, black, black, blockColors[13].up, black);
		applyColor(13, black, black, black, sideTemp1, sideTemp2, black);
	}
	else {
		sColor temp1 = blockColors[10].front;
		sColor temp2 = blockColors[10].right;
		sColor temp3 = blockColors[10].up;

		// Corners
		applyColor(10, blockColors[12].left, black, blockColors[12].front, black, blockColors[12].up, black);
		applyColor(12, blockColors[6].left, black, black, blockColors[6].back, blockColors[6].up, black);
		applyColor(6, black, blockColors[8].right, black, blockColors[8].back, blockColors[8].up, black);
		applyColor(8, black, temp2, temp1, black, temp3, black);

		// Sides
		sColor sideTemp1 = blockColors[11].front;
		sColor sideTemp2 = blockColors[11].up;

		applyColor(11, blockColors[13].left, black, black, black, blockColors[13].up, black);
		applyColor(13, black, black, black, blockColors[7].back, blockColors[7].up, black);
		applyColor(7, black, blockColors[9].right, black, black, blockColors[9].up, black);
		applyColor(9, black, black, sideTemp1, black, sideTemp2, black);
	}
}

void downColorSwitch(bool inverse)
{
	if (!inverse) {
		sColor temp1 = blockColors[22].front;
		sColor temp2 = blockColors[22].right;
		sColor temp3 = blockColors[22].down;

		// Corners
		applyColor(22, blockColors[24].left, black, blockColors[24].front, black, black, blockColors[24].down);
		applyColor(24, blockColors[18].left, black, black, blockColors[18].back, black, blockColors[18].down);
		applyColor(18, black, blockColors[20].right, black, blockColors[20].back, black, blockColors[20].down);
		applyColor(20, black, temp2, temp1, black, black, temp3);

		// Sides
		sColor sideTemp1 = blockColors[23].front;
		sColor sideTemp2 = blockColors[23].down;

		applyColor(23, blockColors[25].left, black, black, black, black, blockColors[25].down);
		applyColor(25, black, black, black, blockColors[19].back, black, blockColors[19].down);
		applyColor(19, black, blockColors[21].right, black, black, black, blockColors[21].down);
		applyColor(21, black, black, sideTemp1, black, black, sideTemp2);
	}
	else {
		sColor temp1 = blockColors[22].front;
		sColor temp2 = blockColors[22].right;
		sColor temp3 = blockColors[22].down;

		// Corners
		applyColor(22, blockColors[20].right, black, blockColors[20].back, black, black, blockColors[20].down);
		applyColor(20, black, blockColors[18].left, blockColors[18].back, black, black, blockColors[18].down);
		applyColor(18, black, blockColors[24].left, black, blockColors[24].front, black, blockColors[24].down);
		applyColor(24, temp2, black, black, temp1, black, temp3);

		// Sides
		sColor sideTemp1 = blockColors[23].front;
		sColor sideTemp2 = blockColors[23].down;

		applyColor(23, blockColors[21].right, black, black, black, black, blockColors[21].down);
		applyColor(21, black, black, blockColors[19].back, black, black, blockColors[19].down);
		applyColor(19, black, blockColors[25].left, black, black, black, blockColors[25].down);
		applyColor(25, black, black, black, sideTemp1, black, sideTemp2);
	}
}

void standingColorSwitch(bool inverse)
{
	if (!inverse)
	{
		//Middles
		sColor temp1 = blockColors[4].up;

		applyColor(4, black, black, black, black, blockColors[3].left, black);
		applyColor(3, black, black, black, blockColors[5].down, black, black);
		applyColor(5, black, black, black, black, black, blockColors[2].right);
		applyColor(2, black, black, temp1, black, black, black);

		// Edges
		sColor tmp1 = blockColors[9].right;
		sColor tmp2 = blockColors[9].up;

		applyColor(9, black, black, blockColors[13].up, black, blockColors[13].left, black);
		applyColor(13, black, black, black, blockColors[25].down, blockColors[25].left, black);
		applyColor(25, black, black, black, blockColors[21].down, black, blockColors[21].right);
		applyColor(21, black, black, tmp2, black, black, tmp1);
	}
	else {
		//Middles
		sColor temp1 = blockColors[4].up;

		applyColor(4, black, black, black, black, blockColors[2].right, black);
		applyColor(2, black, black, blockColors[5].down, black, black, black);
		applyColor(5, black, black, black, black, black, blockColors[3].left);
		applyColor(3, black, black, black, temp1, black, black);

		// Edges
		sColor tmp1 = blockColors[9].right;
		sColor tmp2 = blockColors[9].up;

		applyColor(9, black, black, blockColors[21].down, black, blockColors[21].right, black);
		applyColor(21, black, black, blockColors[25].down, black, black, blockColors[25].left);
		applyColor(25, black, black, black, blockColors[13].up, black, blockColors[13].left);
		applyColor(13, black, black, black, tmp2, tmp1, black);
	}
}

void middleColorSwitch(bool inverse)
{
	if (!inverse)
	{
		//Middles
		sColor temp1 = blockColors[4].up;

		applyColor(4, black, black, black, black, blockColors[0].front, black);
		applyColor(0, blockColors[5].down, black, black, black, black, black);
		applyColor(5, black, black, black, black, black, blockColors[1].back);
		applyColor(1, black, temp1, black, black, black, black);

		// Edges
		sColor tmp1 = blockColors[11].front;
		sColor tmp2 = blockColors[11].up;

		applyColor(11, blockColors[23].down, black, black, black, blockColors[23].front, black);
		applyColor(23, blockColors[19].down, black, black, black, black, blockColors[19].back);
		applyColor(19, black, blockColors[7].up, black, black, black, blockColors[7].back);
		applyColor(7, black, tmp2, black, black, tmp1, black);
	}
	else {
		//Middles
		sColor temp1 = blockColors[4].up;

		applyColor(4, black, black, black, black, blockColors[1].back, black);
		applyColor(1, black, blockColors[5].down, black, black, black, black);
		applyColor(5, black, black, black, black, black, blockColors[0].front);
		applyColor(0, temp1, black, black, black, black, black);

		// // Edges
		sColor tmp1 = blockColors[11].front;
		sColor tmp2 = blockColors[11].up;

		applyColor(11, blockColors[7].up, black, black, black, blockColors[7].back, black);
		applyColor(7, black, blockColors[19].down, black, black, blockColors[19].back, black);
		applyColor(19, black, blockColors[23].down, black, black, black, blockColors[23].front);
		applyColor(23, tmp2, black, black, black, black, tmp1);
	}
}

void equatorColorSwitch(bool inverse)
{
	if (!inverse)
	{
		//Middles
		sColor temp1 = blockColors[0].front;

		applyColor(0, blockColors[2].right, black, black, black, black, black);
		applyColor(2, black, black, blockColors[1].back, black, black, black);
		applyColor(1, black, blockColors[3].left, black, black, black, black);
		applyColor(3, black, black, black, temp1, black, black);

		// Edges
		sColor tmp1 = blockColors[16].front;
		sColor tmp2 = blockColors[16].right;

		applyColor(16, blockColors[15].right, black, blockColors[15].back, black, black, black);
		applyColor(15, black, blockColors[14].left, blockColors[14].back, black, black, black);
		applyColor(14, black, blockColors[17].left, black, blockColors[17].front, black, black);
		applyColor(17, tmp2, black, black, tmp1, black, black);
	}
	else {
		//Middles
		sColor temp1 = blockColors[0].front;

		applyColor(0, blockColors[3].left, black, black, black, black, black);
		applyColor(3, black, black, black, blockColors[1].back, black, black);
		applyColor(1, black, blockColors[2].right, black, black, black, black);
		applyColor(2, black, black, temp1, black, black, black);

		// Edges
		sColor tmp1 = blockColors[16].front;
		sColor tmp2 = blockColors[16].right;

		applyColor(16, blockColors[17].left, black, blockColors[17].front, black, black, black);
		applyColor(17, blockColors[14].left, black, black, blockColors[14].back, black, black);
		applyColor(14, black, blockColors[15].right, black, blockColors[15].back, black, black);
		applyColor(15, black, tmp2, tmp1, black, black, black);
	}
}


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}

// read a BMP file into a Texture:

#define VERBOSE				false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB				0
#define BI_RLE8				1
#define BI_RLE4				2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;



// read a BMP file into a Texture:

unsigned char*
BmpToTexture(char* filename, int* width, int* height)
{
	FILE* fp;
#ifdef _WIN32
	errno_t err = fopen_s(&fp, filename, "rb");
	if (err != 0)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}
#else
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort(fp);


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if (VERBOSE) fprintf(stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
		FileHeader.bfType, FileHeader.bfType & 0xff, (FileHeader.bfType >> 8) & 0xff);
	if (FileHeader.bfType != BMP_MAGIC_NUMBER)
	{
		fprintf(stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType);
		fclose(fp);
		return NULL;
	}


	FileHeader.bfSize = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize);

	FileHeader.bfReserved1 = ReadShort(fp);
	FileHeader.bfReserved2 = ReadShort(fp);

	FileHeader.bfOffBytes = ReadInt(fp);


	InfoHeader.biSize = ReadInt(fp);
	InfoHeader.biWidth = ReadInt(fp);
	InfoHeader.biHeight = ReadInt(fp);

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort(fp);

	InfoHeader.biBitCount = ReadShort(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount);

	InfoHeader.biCompression = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression);

	InfoHeader.biSizeImage = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage);

	InfoHeader.biXPixelsPerMeter = ReadInt(fp);
	InfoHeader.biYPixelsPerMeter = ReadInt(fp);

	InfoHeader.biClrUsed = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed);

	InfoHeader.biClrImportant = ReadInt(fp);

	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );

	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char* texture = new unsigned char[3 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\n");
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ((InfoHeader.biBitCount * InfoHeader.biWidth + 31) / 32);
	if (VERBOSE)	fprintf(stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes);

	int myRowSizeInBytes = (InfoHeader.biBitCount * InfoHeader.biWidth + 7) / 8;
	if (VERBOSE)	fprintf(stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes);

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if (VERBOSE)	fprintf(stderr, "New NumExtra padding = %d\n", numExtra);


	// this function does not support compression:

	if (InfoHeader.biCompression != 0)
	{
		fprintf(stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression);
		fclose(fp);
		return NULL;
	}

	// we can handle 24 bits of direct color:
	if (InfoHeader.biBitCount == 24)
	{
		rewind(fp);
		fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
		int t;
		unsigned char* tp;
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				*(tp + 2) = fgetc(fp);		// b
				*(tp + 1) = fgetc(fp);		// g
				*(tp + 0) = fgetc(fp);		// r
			}

			for (int e = 0; e < numExtra; e++)
			{
				fgetc(fp);
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if (InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256)
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32* colorTable = new struct rgba32[InfoHeader.biClrUsed];

		rewind(fp);
		fseek(fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET);
		for (int c = 0; c < InfoHeader.biClrUsed; c++)
		{
			colorTable[c].r = fgetc(fp);
			colorTable[c].g = fgetc(fp);
			colorTable[c].b = fgetc(fp);
			colorTable[c].a = fgetc(fp);
			if (VERBOSE)	fprintf(stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a);
		}

		rewind(fp);
		fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
		int t;
		unsigned char* tp;
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				int index = fgetc(fp);
				*(tp + 0) = colorTable[index].r;	// r
				*(tp + 1) = colorTable[index].g;	// g
				*(tp + 2) = colorTable[index].b;	// b
			}

			for (int e = 0; e < numExtra; e++)
			{
				fgetc(fp);
			}
		}

		delete[] colorTable;
	}

	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt(FILE* fp)
{
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	const unsigned char b2 = fgetc(fp);
	const unsigned char b3 = fgetc(fp);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

short
ReadShort(FILE* fp)
{
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	return (b1 << 8) | b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}
