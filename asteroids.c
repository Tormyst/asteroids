/*
 *	asteroids.c
 *
 *	skeleton code for an OpenGL implementation of the Asteroids video game
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifdef __APPLE__
    #include <GLUT/glut.h> // Required on mac.  Was working with GL/glut.h in 10.10.5, but not in 10.11.2
#else
    #include <GL/glut.h> // Required elsewhere.
#endif

#include "displayFont.h"
#include "glutTime.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0

#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)

#define COLOR_GL_LINE glColor3f( 0.9, 0.9, 0.9 )
#define COLOR_GL_DOT glColor3f( 1.0, 1.0, 1.0 )

#define MAX_PHOTONS     8
#define MAX_ASTEROIDS	8

#define ASTEROIDS_MAX_VERTICES	16
#define ASTEROIDS_SPAWN_MIN 2
#define ASTEROIDS_SPAWN_MAX 10

#define SHIP_MAX_ROTATION 0.3
#define SHIP_ACCELERATION 0.2
#define SHIP_SPEEDUP_FRAME_INCREMENT 0.005
#define SHIP_MAX_SPEED    3
#define SHIP_SHOT_COOLDOWN_MAX 5


#define POINT_SETUP(out,phi,poly,index,pos) out.x = poly[index].x * cos(phi) + poly[index].y *-sin(phi) + pos->x; \
                                            out.y = poly[index].x * sin(phi) + poly[index].y * cos(phi) + pos->y;


/* -- type definitions ------------------------------------------------------ */

typedef struct Coords {
	double		x, y;
} Coords;

typedef struct {
    Coords pos, dpos;
	double	phi;
    int showFire, shotCooldown;
} Ship;

typedef struct {
    Coords pos, dpos;
	int	active;
} Photon;

typedef struct {
    Coords pos, dpos;
	int	active, nVertices;
	double	phi, dphi, maxCoord;
	Coords	coords[ASTEROIDS_MAX_VERTICES];
} Asteroid;

/* -- ship points ----------------------------------------------------------- */

static const Coords shipPoints[] = { { 0, 2}, {1.2, -1.6}, {0, -1}, {-1.2, -1.6} };

/* -- function prototypes --------------------------------------------------- */

static void	myDisplay(void);
static void	mainTime(int value);
static void	myKey(unsigned char key, int x, int y);
static void	keyPress(int key, int x, int y);
static void	keyRelease(int key, int x, int y);
static void	myReshape(int w, int h);

static void	init(void);
static void	initAsteroid(Asteroid *a, double x, double y, double size);
static void	drawShip(Ship *s);
static void	drawPhoton(Photon *p);
static void	drawAsteroid(Asteroid *a);

static void spawnAsteroidSpecific(int count, int x, int y, double size);
static void spawnAsteroid(int count);

static double myRandom(double min, double max);
static double sqrDistance(Coords* a, Coords* b);
static int polygonColision(Coords *poly1, int size1, double phi1, Coords *pos1,
                           Coords *poly2, int size2, double phi2, Coords *pos2);
static int screenWrap(Coords* position, int border);


/* -- global variables ------------------------------------------------------ */

static int	up=0, down=0, left=0, right=0, firing=0;	/* state of cursor keys */
static double	xMax, yMax;
static Ship	ship;
static Photon	photons[MAX_PHOTONS];
static Asteroid	asteroids[MAX_ASTEROIDS];

/* -- added effect on death ------------------------------------------------- */
static Coords hitA, hitB, hitC, hitD;
static int showHitLines;


/* -- main ------------------------------------------------------------------ */

int
main(int argc, char *argv[])
{
  srand((unsigned int) time(NULL));

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
  glutInitWindowSize(600, 600);
  glutCreateWindow("Asteroids");
  glutDisplayFunc(myDisplay);
  glutIgnoreKeyRepeat(1);
  glutKeyboardFunc(myKey);
  glutSpecialFunc(keyPress);
  glutSpecialUpFunc(keyRelease);
  glutReshapeFunc(myReshape);
  glutTimerFunc(33, mainTime, 0);
  SetupGlutTime();
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  init();

  glutMainLoop();

  return 0;
}

/* -- callback functions ---------------------------------------------------- */

void
myDisplay()
{
    /*
     *	display callback function
     */

    int	i;
	OnFrame();

  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  drawShip(&ship);

  for (i=0; i<MAX_PHOTONS; i++)
  	if (photons[i].active)
          drawPhoton(&photons[i]);

  for (i=0; i<MAX_ASTEROIDS; i++)
  	if (asteroids[i].active)
          drawAsteroid(&asteroids[i]);

  if(showHitLines)
  {
      glLoadIdentity();
      glBegin(GL_LINES);
      glColor3f(1.0,0.0,0.0);
      glVertex2d(hitA.x, hitA.y);
      glVertex2d(hitB.x, hitB.y);
      glVertex2d(hitC.x, hitC.y);
      glVertex2d(hitD.x, hitD.y);
      glEnd();
  }

	glLoadIdentity();
	COLOR_GL_LINE;
	DisplayString(GetFPS(),3,3,93,4);

    glutSwapBuffers();
}

void
mainTime(int value)
{
    int i, j, asteroidsActiveCount = 0, recallTime = 33;
    /*
     *	timer callback function
     */

    /* reset some state values */

    showHitLines = 0;

    /* advance the ship */
    {
        double speed, da;
        ship.phi += (left * SHIP_MAX_ROTATION) - (right * SHIP_MAX_ROTATION);

		    da = (up * SHIP_ACCELERATION) - (down * SHIP_ACCELERATION);

        ship.dpos.x += da * -sin(ship.phi);
        ship.dpos.y += da * cos(ship.phi);

        speed = sqrt(ship.dpos.x*ship.dpos.x + ship.dpos.y*ship.dpos.y);

        if( speed > SHIP_MAX_SPEED ) // I am just going to add this here, add "&& !down" for big rigs mode.
        {
            ship.dpos.x = (ship.dpos.x / speed) * SHIP_MAX_SPEED;
            ship.dpos.y = (ship.dpos.y / speed) * SHIP_MAX_SPEED;
        }

        ship.pos.x += ship.dpos.x;
        ship.pos.y += ship.dpos.y;

        screenWrap(&ship.pos, 2);

        if(ship.shotCooldown > 0)
            ship.shotCooldown--;
    }
    /* advance photon laser shots, eliminating those that have gone past
      the window boundaries */
    for (i = 0; i < MAX_PHOTONS; i++) {
        if(photons[i].active >= 2)
        {
            Coords lineFromPoint[2], zero = {0,0};

            lineFromPoint[0] = photons[i].pos;

            photons[i].pos.x += photons[i].dpos.x;
            photons[i].pos.y += photons[i].dpos.y;

            lineFromPoint[1] = photons[i].pos;

            if( screenWrap(&photons[i].pos, 2) )
                photons[i].active = 0;
            for (j = 0; j < MAX_ASTEROIDS; j++)
            {
                if(asteroids[j].active && sqrDistance(&asteroids[j].pos, &photons[i]) < asteroids[j].maxCoord * asteroids[j].maxCoord * 1.1 && polygonColision(&asteroids[j].coords, asteroids[j].nVertices, asteroids[j].phi, &asteroids[j].pos, &lineFromPoint, 2, 0, &zero))
                {
                    asteroids[j].active = 0;
                    photons[i].active = 0;
                    if (asteroids[j].maxCoord > 5)
                        spawnAsteroidSpecific((rand() % 2 + 2), asteroids[j].pos.x, asteroids[j].pos.y, myRandom(2,5));
                }
            }

        }
        else if(photons[i].active == 1)
        {
            // Used to delay movement for 1 frame.
            // Which improves game feel by having the bullet spawn on your ship before moving.
            photons[i].active++;
            photons[i].pos.x = ship.pos.x + 2 * -sin(ship.phi);
            photons[i].pos.y = ship.pos.y + 2 *  cos(ship.phi);
            photons[i].dpos.x = 4 * -sin(ship.phi) + ship.dpos.x;
            photons[i].dpos.y = 4 * cos(ship.phi) + ship.dpos.y;
			// Recoil
			// ship.dpos.x += -sin(ship.phi) * -0.1;
			// ship.dpos.y += cos(ship.phi) * -0.1;
        }
    }

    /* advance asteroids */

    for (i = 0; i < MAX_ASTEROIDS; i++) {
        if(asteroids[i].active)
        {
            double squareMax = asteroids[i].maxCoord * asteroids[i].maxCoord;
            asteroidsActiveCount++;

            asteroids[i].pos.x += asteroids[i].dpos.x;
            asteroids[i].pos.y += asteroids[i].dpos.y;
            asteroids[i].phi += asteroids[i].dphi;

            screenWrap(&asteroids[i].pos, asteroids[i].maxCoord);

            // Collision detection.  Weak detection first by using distance. Check for ship.
            if ( sqrDistance(&asteroids[i].pos, &ship.pos) < squareMax * 1.1
                && polygonColision(&asteroids[i].coords, asteroids[i].nVertices, asteroids[i].phi, &asteroids[i].pos, &shipPoints, 4, ship.phi, &ship.pos))
            {
                recallTime = 66;
                showHitLines = 1;

                // Code here to stop player interaction and start game over.
            }
        }
    }

    if(asteroidsActiveCount < 1)
        spawnAsteroid(5);

    /* test for and handle collisions */

    glutPostRedisplay();

    glutTimerFunc(recallTime, mainTime, value);		/* 30 frames per second */
}

void
myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case ' ':
            if(ship.shotCooldown <= 0)
            {
                int i;
                for (i = 0; i < MAX_PHOTONS; i++) {
                    if (!photons[i].active) {
                        photons[i].active = 1;
                        ship.shotCooldown = SHIP_SHOT_COOLDOWN_MAX;

                        break;
                    }
                }
            }
            break;
        case 'a':
        {
            int i;
            Coords thing;
            for (i = 0; i <4; i++) {
                POINT_SETUP(thing,ship.phi,shipPoints,i,(&ship.pos));
                printf("Point %d: %f %f\n", i, thing.x, thing.y);
            }
            break;
        }
        default:
            break;
    }
}

void
keyPress(int key, int x, int y)
{
    /*
     *	this function is called when a special key is pressed; we are
     *	interested in the cursor keys only
     */

    switch (key)
    {
        case 100:
            left = 1; break;
        case 101:
            up = 1; break;
        case 102:
            right = 1; break;
        case 103:
            down = 1; break;
    }
}

void
keyRelease(int key, int x, int y)
{
    /*
     *	this function is called when a special key is released; we are
     *	interested in the cursor keys only
     */

    switch (key)
    {
        case 100:
            left = 0; break;
        case 101:
            up = 0; break;
        case 102:
            right = 0; break;
        case 103:
            down = 0; break;
    }
}

void
myReshape(int w, int h)
{
    /*
     *	reshape callback function; the upper and lower boundaries of the
     *	window are at 100.0 and 0.0, respectively; the aspect ratio is
     *  determined by the aspect ratio of the viewport
     */

    xMax = 100.0*w/h;
    yMax = 100.0;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, xMax, 0.0, yMax, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
}


/* -- other functions ------------------------------------------------------- */

void
init()
{
    ship.pos.x = 50;
    ship.pos.y = 50;
    glPointSize(2);
    glEnable(GL_POINT_SMOOTH);
    /*
     * set parameters including the numbers of asteroids and photons present,
     * the maximum velocity of the ship, the velocity of the laser shots, the
     * ship's coordinates and velocity, etc.
     */
}

void
initAsteroid(
	Asteroid *a,
	double x, double y, double size)
{
    /*
     *	generate an asteroid at the given position; velocity, rotational
     *	velocity, and shape are generated randomly; size serves as a scale
     *	parameter that allows generating asteroids of different sizes; feel
     *	free to adjust the parameters according to your needs
     */

    double	theta, r;
    int		i;

    a->pos.x = x;
    a->pos.y = y;
    a->phi = 0.0;
    a->maxCoord = 0.0;
    a->dpos.x = myRandom(-0.8, 0.8);
    a->dpos.y = myRandom(-0.8, 0.8);
    a->dphi = myRandom(-0.2, 0.2);

    a->nVertices = 6+rand()%(ASTEROIDS_MAX_VERTICES-6);
    for (i=0; i<a->nVertices; i++)
    {
        theta = 2.0*M_PI*i/a->nVertices;
        r = size*myRandom(0.5, 1.0);
        if(r > a->maxCoord) a->maxCoord = r;
        a->coords[i].x = -r*sin(theta);
        a->coords[i].y = r*cos(theta);
    }

    a->active = 1;
}

void
drawShip(Ship *s)
{
    int i;

    COLOR_GL_LINE;
    glLoadIdentity();
    myTranslate2D(s->pos.x,s->pos.y);
    myRotate2D(s->phi);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 4; i++) {
        glVertex2f(shipPoints[i].x, shipPoints[i].y);
    }
    glEnd();
    if(up || down)
    {
        s->showFire = !s->showFire;
        if(s->showFire)
        {
            glBegin(GL_LINE_STRIP);
            glVertex2f(-0.3, -0.65);
            glVertex2f(0, - 1);
            glVertex2f(0.3, -0.65);
            glEnd();
        }
    }
}

void
drawPhoton(Photon *p)
{
    COLOR_GL_DOT;
    glLoadIdentity();
    glBegin(GL_POINTS);
        glVertex2f(p->pos.x,p->pos.y);
    glEnd();
}

void
drawAsteroid(Asteroid *a)
{
    int i;

    COLOR_GL_LINE;
    glLoadIdentity();
    myTranslate2D(a->pos.x,a->pos.y);
    myRotate2D(a->phi);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < a->nVertices; i++) {
        glVertex2d(a->coords[i].x, a->coords[i].y);
    }
    glEnd();
}

/* -- spawning function ----------------------------------------------------- */
/*
    Spawning function for asteroids needs to be done right.  So first we find an empty spawn slot,
    Decide how many vertices will be used.  In
*/
static void spawnAsteroidSpecific(int count, int x, int y, double size)
{
    int index;
    for (index = 0; index < MAX_ASTEROIDS && count > 0; index++)
    {
        if (!asteroids[index].active) {
                initAsteroid(&asteroids[index], x, y, size);
            count--;
        }
    }
}

static void spawnAsteroid(int count)
{
    int index;
    int snap, vary;
    for (index = 0; index < MAX_ASTEROIDS && count > 0; index++)
    {
        if (!asteroids[index].active) {
            /*
             Pick one of 4 spawning zones.
             Top, Bottom, Left, Right.
             Ether x or y is going to be zero or 100 (snap), the other is random between those points (vary).
             We just flip a coin to pick which is which.
             */
            snap = (rand() % 2) * 100;
            vary = myRandom(0, 100);
            if(rand() % 2)
            {
                initAsteroid(&asteroids[index], snap, vary, myRandom(ASTEROIDS_SPAWN_MIN, ASTEROIDS_SPAWN_MAX));
            }
            else
            {
                initAsteroid(&asteroids[index], vary, snap, myRandom(ASTEROIDS_SPAWN_MIN, ASTEROIDS_SPAWN_MAX));
            }
            count--;
        }
    }
}

/* -- helper functions ------------------------------------------------------ */

/*
 Preforms screenWraping and returns 0 if no wrap was done, and a positive number if it was.
 */
int
screenWrap(Coords* position, int border)
{
    int retVal = 2;
    if(position->x < -border)
        position->x = 100 + border;
    else if(position->x > 100 + border)
        position->x = -border;
    else
        retVal--;


    if(position->y < -border)
        position->y = 100 + border;
    else if(position->y > 100 + border)
        position->y = -border;
    else
        retVal--;

    return retVal;
}

double
sqrDistance(Coords* a, Coords* b)
{
    double x, y;
    x = (a->x - b->x);
    y = (a->y - b->y);
    return x*x + y*y;
}

int sign(int x)
{
    if( x > 0)
        return 1;
    else if( x < 0)
        return -1;
    else
        return 0;
}

/*
 Quick intersection check used to avoid heavy calculations in later steps of poly colision.
 Bounding box check for line ab, and cd.
*/
int boxIntersect(Coords *a, Coords *b, Coords *c, Coords *d)
{
    double min1, max1, min2, max2;

    if(a->x < b->x)
    {
        min1 = a->x;
        max1 = b->x;
    }
    else
    {
        max1 = a->x;
        min1 = b->x;
    }

    if(c->x < d->x)
    {
        min2 = c->x;
        max2 = d->x;
    }
    else
    {
        max2 = c->x;
        min2 = d->x;
    }

    if (max1 < min2 || max2 < min1) return 0;

    if(a->y < b->y)
    {
        min1 = a->y;
        max1 = b->y;
    }
    else
    {
        max1 = a->y;
        min1 = b->y;
    }

    if(c->y < d->y)
    {
        min2 = c->y;
        max2 = d->y;
    }
    else
    {
        max2 = c->y;
        min2 = d->y;
    }

    return !(max1 < min2 || max2 < min1);

}

int lineColision(Coords *a, Coords *b, Coords *c, Coords *d)
{
    double slope, constant;
    slope = (a->y - b->y) / (a->x - b->x);
    constant = a->y - slope * a->x;

    return sign(slope * c->x + constant - c->y) != sign(slope * d->x + constant - d->y);
}

/*
 This is a slow function and should only be used if we are sure the points are close enough.
 This will calculate each line segment in poly1, with rotation and position, and compare it
 to each line segment in poly2.
 To speed things up, we are going to try and avoid spending more time in each part of the loops.
*/
int polygonColision(Coords *poly1, int size1, double phi1, Coords *pos1,
                    Coords *poly2, int size2, double phi2, Coords *pos2)
{
    int i, j;
    Coords a, b, c, d;
    for (i = 1; i <= size1; i++) {
        POINT_SETUP(a,phi1,poly1,i-1,pos1);
        if(i == size1)
        {
            POINT_SETUP(b,phi1,poly1,0,pos1);
        }
        else
        {
            POINT_SETUP(b,phi1,poly1,i,pos1);
        }

        for (j = 1; j <= size2; j++) {
            POINT_SETUP(c,phi2,poly2,j-1,pos2);
            if(j == size2)
            {
                POINT_SETUP(d,phi2,poly2,0,pos2);
            }
            else
            {
                POINT_SETUP(d,phi2,poly2,j,pos2);
            }
            if ( boxIntersect(&a,&b,&c,&d) && lineColision(&a,&b,&c,&d) )
            {
                hitA = a;
                hitB = b;
                hitC = c;
                hitD = d;
                return 1;
            }
        }
    }
               return 0;
}

double
myRandom(double min, double max)
{
	double	d;

	/* return a random number uniformly draw from [min,max] */
	d = min+(max-min)*(rand()%0x7fff)/32767.0;

	return d;
}
