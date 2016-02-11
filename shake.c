#ifdef __APPLE__
    #include <GLUT/glut.h> // Required on mac.  Was working with GL/glut.h in 10.10.5, but not in 10.11.2
#else
    #include <GL/glut.h> // Required elsewhere.
#endif

#include "util.h"

double MaxShake = 5;
double shakeX;
double shakeY;

int rc = 0;

void useRandomColor(int use)
{
  rc = use;
}

void setShake()
{
    if( MaxShake !=0 )
    {
        shakeX = myRandom(-MaxShake , MaxShake);
        shakeY = myRandom(-MaxShake , MaxShake);
    }
    else
    {
        shakeX = shakeY = 0;
    }
    if(rc)
      glColor3f(myRandom(0.1,1),myRandom(0.1,1),myRandom(0.1,1));
}

void setMaxShake(double newValue)
{
	MaxShake = newValue;
}

void drawLineWithShake(double x1, double y1, double x2, double y2)
{
    setShake();
    glVertex2f( x1 + shakeX, y1 + shakeY);
    glVertex2f( x2 + shakeX, y2 + shakeY);
}
