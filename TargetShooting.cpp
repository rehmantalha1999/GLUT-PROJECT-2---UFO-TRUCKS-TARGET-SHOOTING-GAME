/*********
   CTIS164 - Template Source Program
----------
STUDENT : Talha Rehman Abid
SECTION : 2
HOMEWORK: 2
----------
PROBLEMS: None
----------
ADDITIONAL FEATURES: 1) A loading Screen
					 2) Random Allocation of points per car where some cars have extra points to score
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 700
#define TIMER_PERIOD 30// Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define LOADING_COUNTER 20
#define D2R 0.0174532
#define RUNNING_COUNTER 20
#define MAXT 6
#define LOADING 1
#define RUNNING 2
#define OVER 3


/* Global Variables for Template File */
int load_timer = LOADING_COUNTER;
int load_increment = 0.2;
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int stat = LOADING; // stat is the state and set to initial state i.e. 0
int t_score = 0; // score of the user at the end of the game
bool missle_status = false; // missle movement
int count = 0; // timer counter
int increment = 0; // timer influencer
bool truck_visible = false; // truck visibility status in case of hit/not hit
int ufoX = 0, ufoY = 223;
int speed = TIMER_PERIOD;
//Structures 

typedef struct {
	float xCor, yCor;
}coordinate_struct;  //general blueprint for coordinates


typedef struct { //truck's infomartion structure
	bool truck_visible;
	coordinate_struct truck_position; // truck's coordinates
	int indiv_points; // each truck's potential points reward
	double red, green, blue; // color variables for random allocation of colors

}truck_info_struct;

typedef struct {
	coordinate_struct misslePos;
}missle_struct;


truck_info_struct* Truck;
missle_struct missle = { ufoX + 33,ufoY - 20 }; // missle initial position allocation

//
// to draw circle, center at (x,y)
// radius r
//

void initializeStructure(truck_info_struct* Truck) {
	int i;
	int x = -1000;
	for (i = 0; i < MAXT; i++) {
		(Truck + i)->truck_position.xCor = x;
		(Truck + i)->truck_visible = true;
		x += 200;
	}

}

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void displayMissle() {
	glColor3ub(17, 34, 34); // star 1
	glBegin(GL_TRIANGLES);
	glVertex2f(missle.misslePos.xCor, missle.misslePos.yCor);
	glVertex2f(missle.misslePos.xCor + 20, missle.misslePos.yCor);
	glVertex2f(missle.misslePos.xCor + 10, missle.misslePos.yCor - 20);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(missle.misslePos.xCor, missle.misslePos.yCor - 15);
	glVertex2f(missle.misslePos.xCor + 20, missle.misslePos.yCor - 15);
	glVertex2f(missle.misslePos.xCor + 10, missle.misslePos.yCor + 5);
	glEnd();
}


void displayover() {
	glColor3f(0, 0, 0);
	if (t_score > 30) { vprint2(-100, 100, 0.3, "Good Job!!"); }
	else
	{
		vprint2(-100, 100, 0.3, "Game Over!)");
	}
	vprint2(-80, 0, 0.2, "Your Score: %d", t_score);
	vprint(-60, -250, GLUT_BITMAP_8_BY_13, "F4 to Play Again");
}
void displayloading() {
	glColor3f(0, 0, 0);
	vprint2(-100, 100, 0.3, "LOADING");

	int width = (LOADING_COUNTER - load_timer) * 30;
	glColor3ub(170, 250, 140);
	glRectf(-300, -70, -300 + width, -20);
	vprint2(-100, 310, 0.1, "Truck POP by Talha Rehman Abid");
}

void displayClouds(int x, int y) {

	glColor3ub(95, 95, 95); //white
	circle(0 + x, 0 + y, 30); // cloud 1
	circle(-25 + x, y, 20);
	circle(25 + x, -2 + y, 20);
	circle(21 + x, -19 + y, 10);

	circle(0 - 350 + x, 0 + 50 + y, 30);//cloud 2
	circle(-25 - 350 + x, +50 + y, 20);
	circle(25 - 350 + x, -2 + 50 + y, 20);
	circle(21 - 350 + x, -19 + 50 + y, 10);

	circle(0 + 150 + x, 0 + y, 30); //cloud 3
	circle(-25 + 150 + x, y, 20);
	circle(25 + 150 + x, -2 + y, 20);
	circle(21 + 150 + x, -19 + y, 10);

	circle(0 + 300 + x, 0 - 80 + y, 30); //cloud 4
	circle(-25 + 300 + x, -80 + y, 20);
	circle(25 + 300 + x, -80 - 2 + y, 20);
	circle(21 + 300 + x, -80 - 19 + y, 10);

	circle(0 + 480 + x, 0 + y, 30); //cloud 5
	circle(-25 + 480 + x, y, 20);
	circle(25 + 480 + x, -2 + y, 20);
	circle(21 + 480 + x, -19 + y, 10);

	circle(0 - 480 + x, 0 + 20 + y, 30); // cloud 6
	circle(-25 - 480 + x, 20 + y, 20);
	circle(25 - 480 + x, 20 - 2 + y, 20);
	circle(21 - 480 + x, 20 + -19 + y, 10);

	circle(0 - 250 + x, 0 + y, 30); // cloud 1
	circle(-25 - 250 + x, y, 20);
	circle(25 - 250 + x, -2 + y, 20);
	circle(21 - 250 + x, -19 + y, 10);
}

void displayrunning() {
	glColor3ub(204, 194, 183); // moon
	circle(190, 140, 70);

	glColor3ub(164, 154, 143); // moon spots
	circle(190, 120, 20);
	glColor3ub(174, 164, 153); // moon spots
	circle(220, 160, 15);
	glColor3ub(174, 164, 153); // moon spots
	circle(170, 170, 15);
	glColor3ub(174, 164, 153); // moon spots
	circle(155, 150, 10);
	glColor3ub(174, 164, 153); // moon spots
	circle(190, 170, 5);
}




void displayGradient(int x1, int y1, int w, int h, float r, float g, float b) {
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glColor3f(r + 0.2, g + 0.2, b + 0.2);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
}

void displayufo(int ufoX, int ufoY)
{

	//missles nozzle
	glColor3ub(150, 150, 150);
	glRectf(ufoX + 37, ufoY - 4, ufoX + 47, ufoY - 24);
	glColor3ub(0, 0, 0);
	glRectf(ufoX + 37, ufoY - 15, ufoX + 47, ufoY - 20);
	glEnd();

	//capsule
	glColor3ub(240, 196, 32);
	circle(ufoX + 44, ufoY + 35, 34);


	//ship frame
	glColor3ub(25, 25, 112);
	glBegin(GL_POLYGON);
	glVertex2f(ufoX, ufoY); // 1st left bottom
	glVertex2f(ufoX - 14, ufoY + 14); // side left bottom
	glVertex2f(ufoX, ufoY + 39); // one upper side  2nd vertex
	glVertex2f(ufoX + 19, ufoY + 19); // upper down 3rd


	glVertex2f(ufoX + 79, ufoY + 29); //5th new
	glVertex2f(ufoX + 89, ufoY + 39); //6th indent
	glVertex2f(ufoX + 104, ufoY + 14); // side left bottom
	glVertex2f(ufoX + 89, ufoY + 1); // 7th bottom
	glEnd();




	//bottom hover-enablers
	glColor3ub(47, 79, 79);
	circle(ufoX + 10, ufoY, 11);
	circle(ufoX + 33, ufoY, 11);
	circle(ufoX + 52, ufoY, 11);
	circle(ufoX + 74, ufoY, 11);

	glColor3ub(0.1, 0.1, 0.1);
	circle(ufoX + 10, ufoY, 5);
	circle(ufoX + 30, ufoY, 5);
	circle(ufoX + 52, ufoY, 5);
	circle(ufoX + 74, ufoY, 5);


	//outlines
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(ufoX + 17, ufoY + 24);
	glVertex2f(ufoX + 19, ufoY + 10);
	glVertex2f(ufoX + 71, ufoY + 27);
	glVertex2f(ufoX + 71, ufoY + 10);

	glEnd();

}

void display_truck(int xB, int yB, int red, int green, int blue) {
	glColor3ub(red, green, blue); // back bonnet
	glRectf(xB + 214, yB + 1, xB + 272, yB + 30);


	glRectf(xB + 304, yB + 1, xB + 357, yB + 37); // front bonnet


	glRectf(xB + 272, yB + 1, xB + 304, yB + 55); // door 

	glColor3ub(0, 0, 0);
	circle(xB + 243, yB + 2, 22); // left outer tire

	glColor3ub(105, 105, 105);
	circle(xB + 243, yB + 2, 12); // left inner tire

	glColor3ub(105, 105, 105);
	glRectf(xB + 208, yB + 9, xB + 215, yB + 15); // exhaust

	glColor3ub(128, 128, 128);
	circle(xB + 199, yB + 12, 5); // smoke 1

	glColor3ub(128, 128, 128);
	circle(xB + 180, yB + 15, 10); // smoke 2

	//glColor3ub(128, 128, 128);
	//circle(xB + 150, yB + 35, 16); //smoke 3



	glColor3ub(119, 136, 153);
	glRectf(xB + 277, yB + 33, xB + 300, yB + 47); // door window

	glLineWidth(2);
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(xB + 278, yB + 25); // door handle
	glVertex2f(xB + 286, yB + 25);
	glEnd();



	glColor3ub(0, 0, 0);
	circle(xB + 329, yB + 5, 26); // outer tire

	glColor3ub(105, 105, 105);
	circle(xB + 329, yB + 5, 12); // inner tire

	glColor3ub(105, 105, 105);
	circle(xB + 352, yB + 33, 3); // headlight circle

	glColor3ub(240, 196, 32);
	glBegin(GL_TRIANGLES);
	glVertex2f(xB + 352, yB + 33);
	glVertex2f(xB + 480, yB + 45);
	glVertex2f(xB + 480, yB + 13);
	glEnd();

}

//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0.17, 0.17, 0.17, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (stat)
	{
	case LOADING: displayloading(); break;
	case RUNNING: {
		glClearColor(0, 0.52, 0.52, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		displayGradient(-900, -50, 2000, 350, 0.24, 0.19, 0.15);
		displayGradient(-900, 450, 2000, 510, 0.17, 0.17, 0.17);
		displayrunning();
		displayClouds(0, 220);
		displayMissle();
		displayufo(ufoX, ufoY);
		for (int k = 0; k < MAXT; k++)
		{
			if ((Truck + k)->truck_visible == true)
			{
				display_truck(((Truck + k)->truck_position.xCor), (Truck + k)->truck_position.yCor, (Truck + k)->red, (Truck + k)->green, (Truck + k)->blue);
				glColor3f(0, 0, 1);
				vprint(295 + ((Truck + k)->truck_position.xCor), -5 + (Truck + k)->truck_position.yCor + 20, GLUT_BITMAP_8_BY_13, "%d", (Truck + k)->indiv_points);
			}
		}
		glColor3f(1, 0, 0);
		vprint(-600, 310, GLUT_BITMAP_8_BY_13, "Total Score: %d", t_score);
		vprint(-600, 290, GLUT_BITMAP_8_BY_13, "Timer: %d", count);
	}break;

	case OVER: displayover();
	}

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ' && stat == RUNNING)
		missle_status = true;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {

	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}
	if (key == GLUT_KEY_LEFT && stat == RUNNING) {
		ufoX -= 15;
		if (missle_status == false)
			missle.misslePos.xCor = ufoX + 33;
	}

	if (key == GLUT_KEY_RIGHT && stat == RUNNING) {
		ufoX += 15;
		if (missle_status == false)
			missle.misslePos.xCor = ufoX + 33;
	}

	if (key == GLUT_KEY_F4 && stat == OVER) //to restart game when it is over
	{
		missle.misslePos.xCor = ufoX + 33;
		missle.misslePos.yCor = ufoY - 20;
		stat = RUNNING;
		t_score = 0;
		increment = 0;
		missle_status = false;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) {

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
	// Write your codes here.
	glutTimerFunc(speed, onTimer, 0);
	if (stat == LOADING) {
		load_timer -= 0.01;
		if (load_timer > 0) {
			speed = 100;
		}
		else {
			// time expires

			stat = RUNNING;
			speed = TIMER_PERIOD;
		}
	}
	increment += 10;
	count = increment / 350;

	if (stat == RUNNING && count == RUNNING_COUNTER) // to end at 20 seconds
		stat = OVER;
	if (missle.misslePos.yCor > -560 && missle_status == true && stat == RUNNING)
		missle.misslePos.yCor -= 10;
	if (missle.misslePos.yCor <= -560 && missle_status == true && stat == RUNNING) {
		missle.misslePos.yCor = ufoY - 20;
		missle.misslePos.xCor = ufoX + 30;
		missle_status = false;

	}

	for (int count_t = 0; count_t < MAXT; count_t++) //movement of each Truck
	{
		if ((Truck + count_t)->truck_position.xCor <= 500)
			(((Truck + count_t)->truck_position).xCor) += 5;
		if ((Truck + count_t)->truck_position.xCor > 500)
		{
			(Truck + count_t)->truck_position.xCor = -1100;
			(Truck + count_t)->truck_position.yCor = -350 + rand() % -300;
			(Truck + count_t)->indiv_points = rand() % 4 + 1;
			(Truck + count_t)->red = rand() % 256;
			(Truck + count_t)->green = rand() % 256;
			(Truck + count_t)->blue = rand() % 256;
		}
	}
	for (int count_k = 0; count_k < MAXT; count_k++) { //check if Truck gets hit
		if (((Truck + count_k)->truck_position.xCor + 250 <= missle.misslePos.xCor + 40) && (((Truck + count_k)->truck_position.xCor + 310 >= missle.misslePos.xCor - 40)) && ((Truck + count_k)->truck_position.yCor + 60 >= (missle.misslePos.yCor + 15) && (Truck + count_k)->truck_position.yCor - 40 <= (missle.misslePos.yCor + 15)))
		{
			(Truck + count_k)->truck_visible = false;
			(missle.misslePos.xCor) = ufoX + 34;
			missle_status = false;
			missle.misslePos.yCor = ufoY - 25;
			t_score += (Truck + count_k)->indiv_points;
			if ((Truck + count_k)->truck_visible == false) {
				(Truck + count_k)->truck_visible = true;

				(Truck + count_k)->truck_position.xCor = -1100;
				(Truck + count_k)->truck_position.yCor = -350 + rand() % -300;
				(Truck + count_k)->indiv_points = rand() % 4 + 1;
				(Truck + count_k)->red = rand() % 256;
				(Truck + count_k)->green = rand() % 256;
				(Truck + count_k)->blue = rand() % 256;
			}
		}
	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initializeStructure(Truck);
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("TALHA REHMAN ABID HOMEWORK # 2");

	int i;
	Truck = (truck_info_struct*)malloc(sizeof(truck_info_struct) * MAXT);
	srand(time(0));

	for (i = 0; i < MAXT; i++) {

		(Truck + i)->red = rand() % 256;
		(Truck + i)->green = rand() % 256;
		(Truck + i)->blue = rand() % 256;
		(Truck + i)->indiv_points = rand() % 5 + 1;
		(Truck + i)->truck_position.yCor = -350 + rand() % -300;
	}

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}