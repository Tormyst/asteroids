/*
 *	asteroids.c
 *
 *	Main file for asteroid game.
 *  Created by Raphael Bronfman-Nadas from skeleton code provided by Dirk Arnold
 * Author: Raphael BN
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
#include "shake.h"
#include "util.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0

#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)

// Target FPS locked to 30 as physics is locked to FPS right now.  
// TODO: Make gameplay linked not to frames.  
// TODO: Gameplay must be slowable independent of framerate, as slowdown should impact gamespeed
#define TARGET_FPS 30
#define SLOW_FPS 15

// Base colors for lines.  Points should be brighter, so we lower liens insted.
#define COLOR_GL_LINE glColor3f( 0.9, 0.9, 0.9 )
#define COLOR_GL_DOT glColor3f( 1.0, 1.0, 1.0 )
// Max elements in pools.
#define MAX_PHOTONS     8
#define MAX_ASTEROIDS	 15
#define MAX_DUSTS      150
// Time before dust despawns and returnes to the pool
#define DUST_FRAME_DESPAWN 60
// Constants for asteroids
#define ASTEROIDS_MAX_VERTICES	16
#define ASTEROIDS_SPAWN_MIN 2
#define ASTEROIDS_SPAWN_MAX 10
// Constants for ships
#define SHIP_MAX_ROTATION 0.3
#define SHIP_ACCELERATION 0.2
#define SHIP_MAX_SPEED    3
#define SHIP_SHOT_COOLDOWN_MAX 5
#define SHIP_INVINCIBILITY_FRAMES 60
#define SHIP_STARTING_LIVES    3
// Time text for wave is on screen.
#define WAVE_MAX_FRAMES 30
// Making hyper more usable
#define HYPER_CAST 10
#define HYPER_COOLDOWN 20
#define HYPER_DUST_INIT 1
// Amount screenshake goes away per frame.
#define SCREENSHAKE_DECAY 0.1
// Constants having to do with ship death.
#define DYING_FRAMES 60
#define DYING_DUST_FRAMES 20
// Preforms rotation and translation on a point.
#define POINT_SETUP(out,phi,poly,index,pos) out.x = poly[index].x * cos(phi) + poly[index].y *-sin(phi) + pos->x; \
                                            out.y = poly[index].x * sin(phi) + poly[index].y * cos(phi) + pos->y;
/* -- type definitions ------------------------------------------------------ */
typedef struct Coords { double		x, y;} Coords;

typedef struct {
  Coords pos, dpos, hyperPos;double	phi;
  int showFire, shotCooldown, lives, invincible, hyper;
} Ship;

typedef struct {Coords pos, dpos; int	active;} Photon;

typedef struct {Coords pos, dpos; int	active, nVertices;
  double	phi, dphi, maxCoord; Coords	coords[ASTEROIDS_MAX_VERTICES];
} Asteroid;

typedef struct {
  Coords pos1, pos2, dpos;
  int active, frame;
} Dust; // Not the Dust from His Dark Materials. Ha, jokes no one will get.

/*
  State explanations:
    GameState_StartScreen Title screen.
    GameState_Playing As long as in this state, the player has control.
    GameState_Dying animation of the player dying.
    GameState_Dead game over screen.
*/
typedef enum {GameState_StartScreen, GameState_Playing, GameState_Paused, GameState_Dying, GameState_Dead} GameState;

/* -- ship points ----------------------------------------------------------- */
static const Coords shipPoints[] = {{ 0, 2},{1.2, -1.6},{0, -1},{-1.2, -1.6}};
/* -- function prototypes --------------------------------------------------- */
static void	myDisplay(void);
static void	mainTime(int value);
static void activateShip(Ship *s);
static void activatePhoton(Photon *p);
static void activateAsteroid(Asteroid *a);
static void activateDust(Dust *d);
static void	myKey(unsigned char key, int x, int y);
static void myKeyUp(unsigned char key, int x, int y);
static void	keyPress(int key, int x, int y);
static void	keyRelease(int key, int x, int y);
static void	myReshape(int w, int h);
static void	init(void);
static void initPlay(void);
static void	initAsteroid(Asteroid *a, double x, double y, double size);
static void initDustFromAsteroid(Asteroid *a);
static void initDustFromShip(Ship *s);
static void initDustFromShipHyperTarget(Ship *s);
static void	drawShip(Ship *s);
static void	drawPhoton(Photon *p);
static void	drawAsteroid(Asteroid *a);
static void drawDust(Dust *d);
static void drawUI();
static void setScore(int s);
static void spawnAsteroidSpecific(int count, int x, int y, double size);
static void spawnAsteroidLevel(int level);
static double sqrDistance(Coords* a, Coords* b);
static int polygonColision(Coords *poly1, int size1, double phi1, Coords *pos1,
                           Coords *poly2, int size2, double phi2, Coords *pos2);
static int screenWrap(Coords* position, int border);
/* -- global variables ------------------------------------------------------ */
static int recallTime_target = 1000 / TARGET_FPS; 
static int recallTime_slow = 1000 / SLOW_FPS; 
static double screenShake = 0;
static int	up=0, down=0, left=0, right=0, firing=0, hyper = 0;	/* state of cursor keys */
static double	xMax, yMax;
static Ship	ship;
static Photon	photons[MAX_PHOTONS];
static Asteroid	asteroids[MAX_ASTEROIDS];
static Dust dusts[MAX_DUSTS];
static GameState state;
static int stateCounter, score, level, waveCounter;
static char scoreStr[5], waveStr[8]; // Strings for dynamic text.
/* -- main ------------------------------------------------------------------ */
int
main(int argc, char *argv[])
{
  srand((unsigned int) time(NULL));

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
  glutInitWindowSize(600, 600); // Intendid ratio is 1:1
  glutCreateWindow("Asteroids");
  glutDisplayFunc(myDisplay);
  glutIgnoreKeyRepeat(1);
  glutKeyboardFunc(myKey);
  glutKeyboardUpFunc(myKeyUp);
  glutSpecialFunc(keyPress);
  glutSpecialUpFunc(keyRelease);
  glutReshapeFunc(myReshape);
  glutTimerFunc(33, mainTime, 0);
  SetupGlutTime();
  glClearColor(0.0, 0.0, 0.0, 1.0);

  init();

  glutMainLoop();

  return 0;
}

/* -- callback functions ---------------------------------------------------- */

void
myDisplay() // Display callback.
{
  int	i, dShip = 0;
  OnFrame(); // On Frame is for the fps counter to count a frame has gone by.

  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  // Never load the Identity Matrix after this.  It will stop the shake.
  //This is the screen shake.  Very simple, just applies this matrix onto eveything.
  myTranslate2D(myRandom(-screenShake, screenShake), myRandom(-screenShake, screenShake));
  glPushMatrix();

  // Display text and ship switch statment.
  switch (state) {
    case GameState_StartScreen:
      // Text which is scaled, needs a much larger value of shake.
      setMaxShake(5);
      DisplayString("Asteroids",8,8,7,70);
      setMaxShake(7);
      DisplayString("Press space to start",2.5,2.5,17.5,30);
      break;
    case GameState_Dead:
      setMaxShake(5);
      DisplayString("You are dead", 5,5,12.5,60);
      setMaxShake(7);
      DisplayString("Final score: ", 2.5, 2.5, 25, 45);
      DisplayString(scoreStr,2.5, 2.5, 65, 45);
      DisplayString("Press space to not be dead",2.5,2.5,8,30);
      break;
    case GameState_Paused:
      setMaxShake(5);
      DisplayString("Paused",8,8,25,60);
      // This is right here.
    case GameState_Playing:
      dShip = 1;
      break;
  }
  // Photons
  for (i=0; i<MAX_PHOTONS; i++)
  	if (photons[i].active)
      drawPhoton(&photons[i]);
  // Asteroids.
  // They don't look good with shake.
  setMaxShake(0);
  for (i=0; i<MAX_ASTEROIDS; i++)
  	if (asteroids[i].active)
      drawAsteroid(&asteroids[i]);
  // Dust is my particle pool.
  setMaxShake(1);
  for (i = 0; i < MAX_DUSTS; i++)
    if(dusts[i].active)
      drawDust(&dusts[i]);

  // Make ship is drawn after dust.
  if (dShip) {
    setMaxShake(hyper ? 0.5 : 0.3);
    drawShip(&ship);
  }

  // Draws FPS counter, score, and lives.
  drawUI();
  glPopMatrix();

  glutSwapBuffers();
}

void
mainTime(int value)
{
    int i, j, asteroidsActiveCount = 0, recallTime = recallTime_target;
    /*
     *	timer callback function
     */
     // If paused, we should do almsot nothing, so I just return
     if(state == GameState_Paused)
     {
       glutTimerFunc(recallTime, mainTime, value);
       glutPostRedisplay();
       return;
     }
    /* advance counters */
    screenShake -= SCREENSHAKE_DECAY;
    if(screenShake < 0) screenShake = 0;
    if(waveCounter > 0) waveCounter--;
    /* advance the ship */
    if(state == GameState_Playing) {
      activateShip(&ship);
      if (hyper) {
	initDustFromShipHyperTarget(&ship);
	if (ship.hyper == 1) {
          for(i=0;i<HYPER_DUST_INIT;i++)
            initDustFromShip(&ship);
	}
      }
    }
    /* advance asteroids and collide with ship */
    for (i = 0; i < MAX_ASTEROIDS; i++)
      if(asteroids[i].active)
      {
        activateAsteroid(&asteroids[i]);
        asteroidsActiveCount++;
      }

      /* advance photon laser shots, eliminating those that have gone past
        the window boundaries and check for collisions with asteroids */
      for (i = 0; i < MAX_PHOTONS; i++)
        if (photons[i].active)
          activatePhoton(&photons[i]);

    // Dust is purly a visual.
    for (i = 0; i < MAX_DUSTS; i++)
      if(dusts[i].active)
        activateDust(&dusts[i]);
    // If there are no more asteroids, create the next lv.
    if(asteroidsActiveCount < 1)
        spawnAsteroidLevel(++level);
    // If dying, there are two substates of slowdown, with spawning dust,
    // And just some waiting frames.  At the end of which, we either respawn,
    // Or move on to the great beyond.
    if(state == GameState_Dying)
    {
      if(stateCounter <= 0)
      {
        ship.lives--;
        // Respawn
        if(ship.lives > 0)
        {
          state = GameState_Playing;
          ship.pos.x = 50;
          ship.pos.y = 50;
          ship.dpos.x = 0;
          ship.dpos.y = 0;
          ship.phi = 0;
          ship.invincible = SHIP_INVINCIBILITY_FRAMES;
        }
        // You ran out of lives and go to the dead state.
        else
          state = GameState_Dead;
      }
      // This is the effects for the ship being destroyed.
      if(stateCounter > DYING_FRAMES - DYING_DUST_FRAMES)
      {
        initDustFromShip(&ship);
        recallTime = recallTime_slow;
      }
      stateCounter--;
    }
    glutPostRedisplay();
    glutTimerFunc(recallTime, mainTime, value);		/* 30 frames per second */
}
// Moves the ship each frame.
void activateShip(Ship *s)
{
  double speed, da;
  // Input and set rotation.
  s->phi += (left * SHIP_MAX_ROTATION) - (right * SHIP_MAX_ROTATION);
  da = (up * SHIP_ACCELERATION) - (down * SHIP_ACCELERATION);
  // Update dx and dy
  s->dpos.x += da * -sin(s->phi);
  s->dpos.y += da * cos(s->phi);
  speed = sqrt(s->dpos.x*s->dpos.x + s->dpos.y*s->dpos.y);
  // Set Delta
  if( speed > SHIP_MAX_SPEED )
  {
      s->dpos.x = (s->dpos.x / speed) * SHIP_MAX_SPEED;
      s->dpos.y = (s->dpos.y / speed) * SHIP_MAX_SPEED;
  }
  // Set position
  if(hyper && s->hyper == 0 )
  {
    s->pos.x = s->hyperPos.x;
    s->pos.y = s->hyperPos.y;
    s->hyper = HYPER_CAST + HYPER_COOLDOWN;
    s->hyperPos.x = myRandom(0,xMax);
    s->hyperPos.y = myRandom(0,yMax);
  }
  else
  {
    s->pos.x += s->dpos.x;
    s->pos.y += s->dpos.y;
  }
  // wrap around the screen.
  screenWrap(&s->pos, 2);

  // Fix cooldowns.
  if(s->shotCooldown > 0)
    s->shotCooldown--;
  if(s->invincible > 0)
    s->invincible--;
  if(s->hyper > HYPER_CAST || (s->hyper > 0 && hyper))
    s->hyper--;

  // Make sure we sit at HYPER_CAST if hyper isn't pressed
  if(!hyper && s->hyper < HYPER_CAST)
    s->hyper = HYPER_CAST;	
}
static void activatePhoton(Photon *p)
{ // Photons are activated 1 frame late, for effect, and for game feel.
  if(p->active >= 2)
  { // Main update of a photon.
    int j;
    Coords lineFromPoint[2], zero = {0,0};
    // We make a line of the current and last place of the photon.
    lineFromPoint[0] = p->pos;
    // Update here.
    p->pos.x += p->dpos.x;
    p->pos.y += p->dpos.y;

    lineFromPoint[1] = p->pos;
    // returns true if a wrap was done.
    if( screenWrap(&p->pos, 2) )
        p->active = 0;
    for (j = 0; j < MAX_ASTEROIDS; j++)
    {
        // Collision Ya this is an terrible looking line.  But it works, so I am just going to avoid changing it more.
        if(asteroids[j].active && sqrDistance(&asteroids[j].pos, p) < asteroids[j].maxCoord * asteroids[j].maxCoord * 1.5
        && polygonColision(&asteroids[j].coords, asteroids[j].nVertices, asteroids[j].phi, &asteroids[j].pos, &lineFromPoint, 2, 0, &zero))
        {
          // Collision happened, kill the asteroid.
            asteroids[j].active = 0;
            initDustFromAsteroid(&asteroids[j]);
            p->active = 0; // And deactivate this
            if (asteroids[j].maxCoord > 5)
                spawnAsteroidSpecific((rand() % 2 + 2), asteroids[j].pos.x, asteroids[j].pos.y, myRandom(2,5));
            setScore(score + 1);
            screenShake = 2;
        }
    }
  }
  else if(p->active == 1)
  {
      // Used to delay movement for 1 frame.
      // Which improves game feel by having the bullet spawn on your ship before moving.
      // But we need to set up some values here.
      p->active++;
      p->pos.x = ship.pos.x + 2 * -sin(ship.phi);
      p->pos.y = ship.pos.y + 2 *  cos(ship.phi);
      p->dpos.x = 4 * -sin(ship.phi) + ship.dpos.x;
      p->dpos.y = 4 * cos(ship.phi) + ship.dpos.y;
      // Recoil This idea dose not feel good.
      // ship.dpos.x += -sin(ship.phi) * -0.1;
      // ship.dpos.y += cos(ship.phi) * -0.1;
  }
}
static void activateAsteroid(Asteroid *a)
{ // Moves the asteroid given.
  double squareMax = a->maxCoord * a->maxCoord;
  // Just moves it.
  a->pos.x += a->dpos.x;
  a->pos.y += a->dpos.y;
  a->phi += a->dphi;
  //Wraps around.
  screenWrap(&a->pos, a->maxCoord);

  // Collision detection.  Weak detection first by using distance. Check for ship.
  if ( state == GameState_Playing
      && ship.invincible <= 0
      && sqrDistance(&a->pos, &ship.pos) < squareMax * 1.5
      && polygonColision(&a->coords, a->nVertices, a->phi, &a->pos, &shipPoints, 4, ship.phi, &ship.pos))
  {
      screenShake += 5;
      state = GameState_Dying;
      stateCounter = DYING_FRAMES;
  }
}

// Simple, very simple. Dust update.
void activateDust(Dust *d)
{
  d->frame++;
  if(d->frame > DUST_FRAME_DESPAWN)
  {
    d->active = 0;
    return;
  }
  d->pos1.x += d->dpos.x;
  d->pos1.y += d->dpos.y;
  d->pos2.x += d->dpos.x;
  d->pos2.y += d->dpos.y;
}

// Deals with space, p and x.
void myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case ' ':
          switch (state) {
            case GameState_Playing:
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
            case GameState_StartScreen:
              initPlay();
              break;
            case GameState_Dead:
             state = GameState_StartScreen;
             break;
          }
          break;
        case 'x':
          hyper = 1;
	  ship.hyperPos.x = myRandom(0,xMax);
	  ship.hyperPos.y = myRandom(0,yMax);
          useRandomColor(1);
          break;
        // case 'a':
        // {
        //     int i;
        //     Coords thing;
        //     for (i = 0; i <4; i++) {
        //         POINT_SETUP(thing,ship.phi,shipPoints,i,(&ship.pos));
        //         printf("Point %d: %f %f\n", i, thing.x, thing.y);
        //     }
        //     break;
        // }
        case 'p':
          if(state == GameState_Playing)
            state = GameState_Paused;
          else if(state == GameState_Paused)
            state = GameState_Playing;
        default:
            break;
    }
}

// Deals with letting go of x.
void myKeyUp(unsigned char key, int x, int y)
{
  switch (key) {
      case 'x':
        hyper = 0;
        useRandomColor(0);
        break;
      }

}

// Deals with up down left right.
void keyPress(int key, int x, int y)
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
// Deals with up down left right.
void keyRelease(int key, int x, int y)
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

// While I don't advice changing the size of the screen, this is what deals with it.
void myReshape(int w, int h)
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
void init()
{
  // Init function.  sets up start things.  Not full game things.
  state = GameState_StartScreen;
  glPointSize(2);
  glLineWidth(2);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  setScore(0);
}

void initPlay()
{ // This starts the game.
  int i;
  ship.pos.x = 50;
  ship.pos.y = 50;
  ship.dpos.x = 0;
  ship.dpos.y = 0;
  ship.phi = 0;
  ship.shotCooldown = 0;
  ship.lives = SHIP_STARTING_LIVES;
  state = GameState_Playing;
  level = 0;
  setScore(0);

  for (i = 0; i < MAX_PHOTONS; i++)
    photons[i].active = 0;
  for (i = 0; i < MAX_ASTEROIDS; i++)
    asteroids[i].active = 0;
}

void
initAsteroid(
	Asteroid *a,
	double x, double y, double size)
{
    // This is largly unchagned, But I did not like the change in size, so I changed that.

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

// Turns an asteroid to dust.
void initDustFromAsteroid(Asteroid *a)
{
  int dustCounter, polyCounter = 1;

  for(dustCounter = 0; dustCounter < MAX_DUSTS; dustCounter++)
  {
    if(!dusts[dustCounter].active)
    {
      dusts[dustCounter].active = 1;
      dusts[dustCounter].frame = 0;
      if(polyCounter < a->nVertices)
      {
        POINT_SETUP(dusts[dustCounter].pos1,a->phi, a->coords,polyCounter - 1, (&a->pos));
        POINT_SETUP(dusts[dustCounter].pos2,a->phi, a->coords,polyCounter, (&a->pos));
      }
      else
      {
        POINT_SETUP(dusts[dustCounter].pos1,a->phi, a->coords,polyCounter - 1, (&a->pos));
        POINT_SETUP(dusts[dustCounter].pos2,a->phi, a->coords,0, (&a->pos));
      }
      dusts[dustCounter].dpos.x = a->dpos.x + myRandom(-0.5, 0.5);
      dusts[dustCounter].dpos.y = a->dpos.y + myRandom(-0.5, 0.5);
      polyCounter++;
    }
    if(polyCounter > a->nVertices)
      return;
  }
  fprintf(stderr, "Ran out of dust");
}

// Turns the ship to dust.
void initDustFromShip(Ship *s)
{
  int dustCounter, polyCounter = 1;

  for(dustCounter = 0; dustCounter < MAX_DUSTS; dustCounter++)
  {
    if(!dusts[dustCounter].active)
    {
      dusts[dustCounter].active = 1;
      dusts[dustCounter].frame = -10; // I put this in later and it just worked to set this at -10 to extend it.
      if(polyCounter < 4)
      {
        POINT_SETUP(dusts[dustCounter].pos1,s->phi, shipPoints,polyCounter - 1, (&s->pos));
        POINT_SETUP(dusts[dustCounter].pos2,s->phi, shipPoints,polyCounter, (&s->pos));
      }
      else
      {
        POINT_SETUP(dusts[dustCounter].pos1,s->phi, shipPoints,polyCounter - 1, (&s->pos));
        POINT_SETUP(dusts[dustCounter].pos2,s->phi, shipPoints,0, (&s->pos));
      }
      dusts[dustCounter].dpos.x = s->dpos.x + myRandom(-0.5, 0.5);
      dusts[dustCounter].dpos.y = s->dpos.y + myRandom(-0.5, 0.5);
      polyCounter++;
    }
    if(polyCounter > 4)
      return;
  }
  fprintf(stderr, "Ran out of dust");
}

void initDustFromShipHyperTarget(Ship *s)
{
  int dustCounter;
  float phi = myRandom(0, M_PI);
  static const Coords line[] = {{-5,-5}, {5,5}};

  for(dustCounter = 0; dustCounter < MAX_DUSTS; dustCounter++)
  {
    if(!dusts[dustCounter].active)
    {
      dusts[dustCounter].active = 1;
      dusts[dustCounter].frame = 30;
      POINT_SETUP(dusts[dustCounter].pos1, phi, line, 0, (&s->hyperPos));
      POINT_SETUP(dusts[dustCounter].pos2, phi, line, 1, (&s->hyperPos));

      dusts[dustCounter].dpos.x = myRandom(-0.1, 0.1);
      dusts[dustCounter].dpos.y = myRandom(-0.1, 0.1);
      return;
    }
  }
  fprintf(stderr, "Ran out of dust");
}

// Draws the ship.  Wow simple.
void drawShip(Ship *s)
{
    int i;

    glPushMatrix();
    COLOR_GL_LINE;
    myTranslate2D(s->pos.x,s->pos.y);
    myRotate2D(s->phi);
    if(s->invincible/10 % 2)
      glLineWidth(4);
    glBegin(GL_LINES);
    for (i = 1; i < 4; i++) {
        drawLineWithShake(shipPoints[i-1].x, shipPoints[i-1].y,shipPoints[i].x, shipPoints[i].y);
    }
    drawLineWithShake(shipPoints[3].x, shipPoints[3].y,shipPoints[0].x, shipPoints[0].y);
    // Shows fire.
    if(up || down)
    {
        s->showFire = !s->showFire;
        if(s->showFire)
        {
            drawLineWithShake(-0.6, -1.3, 0, -3);
            drawLineWithShake(0, -3, 0.6, -1.3);
        }
    }
    glEnd();
    glLineWidth(2);
    glPopMatrix();
}
// Draws the photons.
void drawPhoton(Photon *p)
{
    COLOR_GL_DOT;
    glBegin(GL_POINTS);
        glVertex2f(p->pos.x,p->pos.y);
    glEnd();
}
// Draws the asteroids.
void drawAsteroid(Asteroid *a)
{
    int i;

    COLOR_GL_LINE;
    glPushMatrix();
    myTranslate2D(a->pos.x,a->pos.y);
    myRotate2D(a->phi);
    glBegin(GL_LINES);
    for (i = 1; i < a->nVertices; i++) {
        drawLineWithShake(a->coords[i-1].x, a->coords[i-1].y, a->coords[i].x, a->coords[i].y);
    }
    drawLineWithShake(a->coords[a->nVertices-1].x, a->coords[a->nVertices-1].y, a->coords[0].x, a->coords[0].y);
    glEnd();
    glPopMatrix();
}
// Draws dust.
void drawDust(Dust *d)
{
  double colorToDisplay = 0.9 - d->frame * 0.02;
  // Disable random color if it was on for dust drawing
  int rc = getRandomColor();
  useRandomColor(0);
  glColor3f( colorToDisplay, colorToDisplay, colorToDisplay );
  glBegin(GL_LINES);
  drawLineWithShake(d->pos1.x, d->pos1.y, d->pos2.x, d->pos2.y);
  glEnd();
  useRandomColor(rc);
}
// Draws FPS counter, Score, and lives.
void drawUI()
{
  int i, j;
  COLOR_GL_LINE;
  setMaxShake(5);
  DisplayString(GetFPS(),3,3,93,4);
  if(state == GameState_Playing || state == GameState_Dying || state == GameState_Paused)
  {
    DisplayString(scoreStr,2,3,2,90);
    if(waveCounter > 0)
      DisplayString(waveStr, 5, 5, 30, 50);
  }
  setMaxShake(0.3);
  for(i = 1; i < ship.lives; i++)
  {
    glPushMatrix();
    myTranslate2D(3+3*i,95);
    glBegin(GL_LINES);
    for (j = 1; j < 4; j++) {
        drawLineWithShake(shipPoints[j-1].x, shipPoints[j-1].y,shipPoints[j].x, shipPoints[j].y);
    }
    drawLineWithShake(shipPoints[3].x, shipPoints[3].y,shipPoints[0].x, shipPoints[0].y);
    glEnd();
    glPopMatrix();
  }
}

// Sets a score, and the string to display it.
void setScore(int s)
{
  score = s;
  if(score > 9999) score = 9999; // You know, if anyone actualy gets here.
  sprintf(scoreStr, "%4d", score);
}

/* -- spawning function ----------------------------------------------------- */

// Makes the exact asteroid given.  Will search the list and put that many of that size, at that place.
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

// Given a wave, will create asteroids for it.  Not that great on progression.
static void spawnAsteroidLevel(int level)
{
    int index, count, minSize, maxSize;
    int snap, vary;
    if(level > 99) return; // Figure out what to do here.  Things break if we let it go.
    count = 4 + level / 5;
    minSize = ASTEROIDS_SPAWN_MIN + level % 5;
    maxSize = ASTEROIDS_SPAWN_MAX + level % 5;

    for (index = 0; index < MAX_ASTEROIDS && count > 0; index++)
    {
        if (!asteroids[index].active) {
            /*
             Pick one of 4 spawning zones.
             Top, Bottom, Left, Right.
             Ether x or y is going to be zero or 100 (snap), the other is random between those points (vary).
             We just flip a coin to pick which is which.
             */
            if(rand() % 2)
            {
              snap = (rand() % 2) * xMax;
              vary = myRandom(0, yMax);
              initAsteroid(&asteroids[index], snap, vary, myRandom(minSize, maxSize));
            }
            else
            {
              snap = (rand() % 2) * yMax;
              vary = myRandom(0, xMax);
              initAsteroid(&asteroids[index], vary, snap, myRandom(minSize, maxSize));
            }
            count--;
        }
    }
    sprintf(waveStr,"Wave %2d",level);
    waveCounter = WAVE_MAX_FRAMES;
    ship.invincible = SHIP_INVINCIBILITY_FRAMES;
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
        position->x = xMax + border;
    else if(position->x > xMax + border)
        position->x = -border;
    else
        retVal--;


    if(position->y < -border)
        position->y = yMax + border;
    else if(position->y > yMax + border)
        position->y = -border;
    else
        retVal--;

    return retVal;
}
// Because I can make a function, I do.  Apparently.
double
sqrDistance(Coords* a, Coords* b)
{
    double x, y;
    x = (a->x - b->x);
    y = (a->y - b->y);
    return x*x + y*y;
}

/*
I looked up a lot of fomulas online for how to do this.  This one I found
in c# here: http://gamedev.stackexchange.com/questions/26004/how-to-detect-2d-line-on-line-collision
But the math behind it I do understand.  I am using this because my formula was
not working right before.

It works off of creating finding the intersection as part of the equaion along
both lines.  If it is in both line segments, then both counters will be between 0 and 1.
*/
int lineColision(Coords *a, Coords *b, Coords *c, Coords *d)
{
    double n1, n2, denom;
    // The bottom that we are going to divide by.
    // This is the dinference of slope without regard for what the actual slopes are.
    // This avoids actualy figuring out which points are less then the others.
    // This value will be zero when they are colinear.
    denom = ((b->x - a->x) * (d->y - c->y)) - ((b->y - a->y) * (d->x - c->x));
    // This is the offset from each line,
    // by comparing point a to point c, then point b to point c in n2.
    n1 = ((a->y - c->y) * (d->x - c->x)) - ((a->x - c->x) * (d->y - c->y));
    n2 = ((a->y - c->y) * (b->x - a->x)) - ((a->x - c->x) * (b->y - a->y));

    // If there were going to be improvments made, it would be here.
    // Improvment would be done by calculating slope and check order of points.
    if (denom == 0) return n1 == 0 && n2 == 0;

    n1 /= denom;
    n2 /= denom;

    return (n1 >= 0 && n1 <= 1) && (n2 >= 0 && n2 <= 1);
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
            if ( lineColision(&a,&b,&c,&d) )
            {
                return 1;
            }
        }
    }
               return 0;
}
