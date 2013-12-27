#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"


#include <ppl.h>
#include <random>

// The Width & Height have been reduced to help you debug
//	more quickly by reducing the resolution 
//  Your code should work for any dimension, and should be set back
//	to 640x480 for submission.

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#include <iostream>
#include "Scene.h"
#include "RayTrace.h"
#include "NormalRenderer.h"

//using namespace concurrency;			//THIS IS FOR PARALLELIZATION ATTEMPT
/* --- Global State Variables --- */
int scene=1;
/* - Menu State Identifier - */
int g_iMenuId;

/* - Mouse State Variables - */
int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

/* - RayTrace Variable for the RayTrace Image Generation - */
RayTrace g_RayTrace;
//RayTrace secondScene;
/* - NormalRenderer Variable for drawing with OpenGL calls instead of the RayTracer - */
NormalRenderer g_NormalRenderer;

/* - RayTrace Buffer - */
Vector g_ScreenBuffer[WINDOW_HEIGHT][WINDOW_WIDTH];

unsigned int g_X = 0, g_Y = 0;
bool g_bRayTrace = false;
bool g_bRenderNormal = true;
//bool g_secondScene=true;

void myinit()
{
	// Default to these camera settings
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, 1, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the Scene Variable for the NormalRenderer
	g_NormalRenderer.SetScene (&g_RayTrace.m_Scene);

	glClearColor(0, 0, 0, 0);
}

void display()
{
	if (g_bRenderNormal)
	{
		g_NormalRenderer.RenderScene ();
	}
	else
	{
		// Set up the camera to render pixel-by-pixel
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity ();
		glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, 1, -1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBegin(GL_POINTS);
		{
			for (int y = 0; y < WINDOW_HEIGHT; y++)
			{
				for (int x = 0; x < WINDOW_WIDTH; x++)
				{
					glColor3f(g_ScreenBuffer[y][x].x, g_ScreenBuffer[y][x].y, g_ScreenBuffer[y][x].z);
					glVertex2i(x, y);
				}
			}
		}

		glEnd();
	}

	glFlush();

	glutSwapBuffers ();
}

void menufunc(int value)
{
	switch (value)
	{
	case 0:
		// Start the Ray Tracing
		g_bRayTrace = true;
		g_bRenderNormal = false;
		//g_secondScene = false;
		break;
	case 1:
		// Render Normal
		//g_secondScene = false;

		g_bRayTrace = false;
		g_X = 0;
		g_Y = 0;
		g_bRenderNormal = true;
		glutPostRedisplay ();
		break;
	case 2:
		break;
		//Allen Hsia
		//For some reason g_secondScene is constantly set to false regardless if i assign it directly within this case statement true
		/*
		g_NormalRenderer.SetScene (&secondScene.m_Scene);
		if(g_secondScene==false)
		{
			printf("Hello");
			g_secondScene==true;
		}
		else
		{
			printf("else");
			g_secondScene==false;
		}
		g_bRayTrace = false;
		g_bRenderNormal = false;
		g_secondScene = false;
		break;*/
	case 3:
		// Quit Program
		exit(0);
		break;
	}
}

void doIdle()
{
	//THis is my failed attempt at making parallel_For loops for the extra credit for parallelization
	//I figured I should show my attempt since it does not work.
	/* 
	   	parallel_for (int(0), WINDOW_WIDTH,[&](int i){
		if(g_bRayTrace)
		{
			g_ScreenBuffer[g_Y][g_X];
			for (int g_X=0;g_X>=WINDOW_WIDTH;g_X++)
			{
				for(int g_Y=0;g_Y>=WINDOW_HEIGHT;g_Y++)
				{glutPostRedisplay();
				}
					g_bRayTrace = false;
					glutPostRedisplay ();
			}
		}
	});
   */

	if (g_bRayTrace)
	{
		g_ScreenBuffer[g_Y][g_X] = g_RayTrace.CalculatePixel (g_X, g_Y);
		// Move to the next pixel
		g_X++;
		if (g_X >= WINDOW_WIDTH)
		{
			// Move to the next row
			g_X = 0;
			g_Y++;

			//You can uncomment the next line to see the raytrace update each step
			glutPostRedisplay();
		}

		// Check for the end of the screen
		if (g_Y >= WINDOW_HEIGHT)
		{
			g_bRayTrace = false;
			glutPostRedisplay ();
		}
	}
	/*if (g_secondScene)
	{
		//the 2 scenes? OnRender();
		g_ScreenBuffer[g_Y][g_X] = secondScene.CalculatePixel (g_X, g_Y);
		
		/*parallel_for (unsigned int(0),WINDOW_WIDTH,[&](unsigned int i)
		{
		});
		
		// Move to the next pixel
		g_X++;
		if (g_X >= WINDOW_WIDTH)
		{
			// Move to the next row
			g_X = 0;
			g_Y++;

			//You can uncomment the next line to see the raytrace update each step
			glutPostRedisplay();
		}

		// Check for the end of the screen
		if (g_Y >= WINDOW_HEIGHT)
		{
			g_secondScene = false;
			glutPostRedisplay ();
		}
	}*/
	else
	{
		glutPostRedisplay ();
	}
}

void mousebutton(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state==GLUT_DOWN);
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

int main (int argc, char ** argv)
{

	//You will be creating a menu to load in scenes
	//The test.xml is the default scene and you will modify this code
	//if (!g_RayTrace.m_Scene.Load ("reflection.xml"))

	//if (!g_RayTrace.m_Scene.Load ("Light.xml"))
	if (!g_RayTrace.m_Scene.Load ("simple.xml"))
	{
		printf ("failed to load scene\n");
		exit(1);
	}
	/*if(!secondScene.m_Scene.Load("Light.xml"))
	{
		printf ("failed to load scene\n");
		exit(1);
	}*/
	glutInit(&argc,argv);

	/* create a window */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("Assignment 5 - Ray Tracer");

	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);

	/* create a right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Render RayTrace",0);
	glutAddMenuEntry("Render Normal",1);
	glutAddMenuEntry("Second Scene",2);
	glutAddMenuEntry("Quit",3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);

	/* callback for idle function */
	glutIdleFunc(doIdle);
	
	//glutKeyboardDownFunc(OnKeyDown);
	/* do initialization */
	myinit();

	glutMainLoop();
	return 0;
}
