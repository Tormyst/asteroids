/*
 * glutTime.c
 *
 * A simple class made to do some glut based time calculations for frame independent events.
 * eventualy.  Currently it is being used as an FPS counter only.
 * Author: Raphael BN
 */

#ifdef __APPLE__
    #include <GLUT/glut.h> // Required on mac.  Was working with GL/glut.h in 10.10.5, but not in 10.11.2
#else
    #include <GL/glut.h> // Required elsewhere.
#endif

#include "glutTime.h"
#include <stdio.h>


// int lastTime;
int currentFps;
int lastFps;
char* strFps;

void OnCallback(int value)
{

	if(lastFps != currentFps)
	{
		lastFps = currentFps;
		sprintf(strFps, "%d", lastFps);
	}
	currentFps = 0;
	glutTimerFunc(1000, OnCallback, 0);
}

void SetupGlutTime()
{
    // lastTime = glutGet(GLUT_ELAPSED_TIME);
	lastFps = 0;
	currentFps = 0;
	strFps = (char*) malloc(sizeof(char) * 3);
	strFps[0] = 0;
	strFps[1] = 0;
	strFps[2] = 0;
	glutTimerFunc(1000, OnCallback, 0);
}
// int GetDeltaTime()
// {
//     int currentTime = glutGet(GLUT_ELAPSED_TIME);
//     int deltaTime = currentTime - lastTime;
//
//     lastTime = currentTime;
//     return deltaTime;
// }

void OnFrame()
{
    currentFps++;
}
char* GetFPS()
{
    return strFps;
}
