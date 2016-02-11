/*
 * displayFont.c
 *
 * A font and string formater in open gl.
 * Author: Raphael BN
*/
#ifdef __APPLE__
    #include <GLUT/glut.h> // Required on mac.  Was working with GL/glut.h in 10.10.5, but not in 10.11.2
#else
    #include <GL/glut.h> // Required elsewhere.
#endif

#include <stdio.h>
#include <stdlib.h>
#include "displayFont.h"
#include "shake.h"

void A()
{
        drawLineWithShake(-100.0, -100.0, -100.0, 50.0 );

        drawLineWithShake(-100.0, 50.0, 0.0, 100.0 );

        drawLineWithShake( 0.0, 100.0, 100.0, 50.0 );

        drawLineWithShake( 100.0, 50.0, 100.0, -100.0 );

        drawLineWithShake(-100.0, -50.0, 100.0, -50.0 );
}
void B()
{
    drawLineWithShake(-100.0, -100.0, -100.0, 100.0 );

    drawLineWithShake(-100.0, -100.0, 50.0, -100.0 );
    drawLineWithShake(-100.0,    0.0, 50.0,    0.0 );
    drawLineWithShake(-100.0,  100.0, 50.0,  100.0 );

    drawLineWithShake( 50.0, 100.0, 100.0, 66.6 );
    drawLineWithShake( 100.0, 66.6, 100.0, 33.3 );
    drawLineWithShake( 100.0, 33.0, 50.0, 0.0 );

    drawLineWithShake( 50.0, 0.0, 100.0, -33.3 );
    drawLineWithShake( 100.0, -33.3, 100.0, -66.6 );
    drawLineWithShake( 100.0, -66.6, 50.0, -100.0 );
}
void C()
{
    drawLineWithShake( 100.0, 100.0, -100.0, 100.0 );
    drawLineWithShake( -100.0, 100.0, -100.0, -100.0 );
    drawLineWithShake( -100.0, -100.0, 100.0, -100.0 );
}
void D()
{
    drawLineWithShake( -100.0, 100.0, 0.0, 100.0 );
    drawLineWithShake( 0.0, 100.0, 100.0, 66.6);
    drawLineWithShake( 100.0, 66.6, 100.0, -66.6);
    drawLineWithShake( 0.0, -100.0, 100.0, -66.6);
    drawLineWithShake( -100.0, -100.0, 0.0, -100.0 );
    drawLineWithShake( -100.0, 100.0, -100.0, -100.0 );
}
void F()
{
    drawLineWithShake( -100.0,100.0,-100.0,-100.0);
    drawLineWithShake( -100.0,100.0, 100.0, 100.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
}
void E()
{
    drawLineWithShake( -100.0, -100.0, 100.0, -100.0);
    F();
}
void G()
{
    drawLineWithShake( 100.0, 33.3, 100.0, 100.0);
    drawLineWithShake( 100.0, 100.0, -100.0, 100.0);
    drawLineWithShake( -100.0, 100.0, -100.0, -100.0);
    drawLineWithShake(-100.0, -100.0, 100.0, -100.0);
    drawLineWithShake( 100.0, -100.0, 100.0, -33.3);
    drawLineWithShake( 100.0, -33.3, 0.0, -33.3);
}
void H()
{
    drawLineWithShake(-100.0,-100.0,-100.0,100.0);
    drawLineWithShake( 100.0,-100.0, 100.0,100.0);
    drawLineWithShake( -100.0, 0.0, 100.0, 0.0);
}
void I()
{
    drawLineWithShake(-100.0, 100.0,100.0,100.0);
    drawLineWithShake(-100.0, -100.0,100.0,-100.0);
    drawLineWithShake(0.0, 100.0,0.0,-100.0);
}
void J()
{
    drawLineWithShake(100.0, 100.0, 100.0, -100.0);
    drawLineWithShake(100.0, -100.0, 0.0, -100.0);
    drawLineWithShake(0.0, -100.0, -100.0, -33.0);
}
void K()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 100.0);
    drawLineWithShake(-100.0, 0.0, 100.0, -100.0);
}
void L()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake(-100.0, -100.0, 100.0, -100.0);
}
void M()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake( 100.0, 100.0, 100.0,-100.0);
    drawLineWithShake(-100.0, 100.0, 0.0, 33.3);
    drawLineWithShake( 0.0, 33.3, 100.0, 100.0);
}
void N()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake( 100.0, 100.0, 100.0,-100.0);
    drawLineWithShake(-100.0, 100.0, 100.0, -100.0);
}
void O()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake( 100.0, 100.0, 100.0,-100.0);
    drawLineWithShake(-100.0, -100.0, 100.0,-100.0);
    drawLineWithShake( 100.0, 100.0, -100.0, 100.0);
}
void P()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake(-100.0, 100.0, 100.0,100.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
    drawLineWithShake(100.0, 100.0, 100.0,0.0);

}
void Q()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake(-100.0, 100.0, 100.0,100.0);
    drawLineWithShake(-100.0,-100.0, 0.0, -100.0);
    drawLineWithShake(0.0, -100.0, 100.0, -33.3);
    drawLineWithShake(100.0, 100.0, 100.0, -33.3);
    drawLineWithShake(0.0, -33.3, 100.0, -100.0);
}
void R()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake(-100.0, 100.0, 100.0,100.0);
    drawLineWithShake(100.0, 100.0, 100.0, 0.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
    drawLineWithShake(-100.0, 0.0, 100.0, -100.0);

}
void S()
{
    drawLineWithShake(-100.0, 100.0,100.0,100.0);
    drawLineWithShake(-100.0, 100.0, -100.0, 0.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
    drawLineWithShake( 100.0, 0.0, 100.0, -100.0);
    drawLineWithShake( -100.0, -100.0, 100.0, -100.0);
}
void T()
{
    drawLineWithShake(-100.0, 100.0, 100.0, 100.0);
    drawLineWithShake(0.0, 100.0, 0.0, -100.0);
}
void U()
{
    drawLineWithShake(-100.0, 100.0,-100.0,-100.0);
    drawLineWithShake(-100.0, -100.0,100.0, -100.0);
    drawLineWithShake(100.0, 100.0,100.0,-100.0);
}
void V()
{
    drawLineWithShake(-100.0, 100.0, 0.0, -100.0);
    drawLineWithShake(0.0, -100.0, 100.0, 100.0);
}
void W()
{
    drawLineWithShake(-100.0, 100.0, -100.0, -100.0);
    drawLineWithShake(-100.0, -100.0, 0.0, -33.3);
    drawLineWithShake(0.0, -33.3, 100.0, -100.0);
    drawLineWithShake(100.0, -100.0, 100.0, 100.0);
}
void X()
{
    drawLineWithShake(-100.0, 100.0, 100.0, -100.0);
    drawLineWithShake( -100.0, -100.0, 100.0, 100.0);
}
void Y()
{
    drawLineWithShake(-100.0, 100.0, 0.0, 33.3);
    drawLineWithShake(100.0, 100.0, 0.0, 33.3);
    drawLineWithShake(0.0, 33.3, 0.0, -100.0);
}
void Z()
{
    drawLineWithShake(-100.0, 100.0, 100.0, 100.0);
    drawLineWithShake(100.0,100.0,-100.0,-100.0);
    drawLineWithShake(-100.0,-100.0,100.0,-100.0);
}
void Num0()
{
    O();
    drawLineWithShake( -100.0, -100.0, 100.0, 100.0);
}
void Num1()
{
    drawLineWithShake( 0.0, 100.0, 0.0, -100.0);
}
void Num2()
{
    drawLineWithShake(-100.0, 100.0,100.0,100.0);
    drawLineWithShake(100.0, 100.0, 100.0, 0.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
    drawLineWithShake( -100.0, 0.0, -100.0, -100.0);
    drawLineWithShake( -100.0, -100.0, 100.0, -100.0);
}
void Num3()
{
    drawLineWithShake(-100.0, 100.0, 100.0,100.0);
    drawLineWithShake(100.0, 100.0, 100.0, -100.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
    drawLineWithShake(-100.0, -100.0, 100.0, -100.0);
}
void Num4()
{
    drawLineWithShake(-100.0, 100.0, -100.0, 0.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
    drawLineWithShake(100.0, 100.0, 100.0, -100.0);
}
void Num5()
{
    S();
}
void Num6()
{
    C();
    drawLineWithShake(-100.0,0.0,100.0,0.0);
    drawLineWithShake(100.0, 0.0, 100.0, -100.0);
}
void Num7()
{
    drawLineWithShake(-100.0, 100.0, 100.0, 100.0);
    drawLineWithShake(100.0, 100.0, 100.0, -100.0);
}
void Num8()
{
    Num3();
    drawLineWithShake(-100.0, 100.0, -100.0, -100.0);
}
void Num9()
{
    Num7();
    drawLineWithShake(-100.0, 100.0, -100.0, 0.0);
    drawLineWithShake(-100.0, 0.0, 100.0, 0.0);
}
// Is a string formater.  This is actualy really hard to use.  I will have to fix it.
// width is actualy 2 times this, so is hight.  x and y, are the first characters center point.
// Suports letters and numbers.
void DisplayString(char* str, double widthPerCharacter, double hightPerCharacter, double xTransform, double yTransform)
{
	int placement = 0;
	while (*str != '\0')
    {
		glPushMatrix();
		glTranslated(xTransform + widthPerCharacter * placement * 1.35, yTransform, 0.0); // Value 1.3 comes from the width we set lower.
        glScalef(widthPerCharacter * 0.006, hightPerCharacter * 0.01, 1); // These values were chosen to give a good size while being able to work in -100 to 100.
        glBegin(GL_LINES);
        switch (*str) {
            case '0':
                Num0();
                break;
            case '1':
                Num1();
                break;
            case '2':
                Num2();
                break;
            case '3':
                Num3();
                break;
            case '4':
                Num4();
                break;
            case '5':
                Num5();
                break;
            case '6':
                Num6();
                break;
            case '7':
                Num7();
                break;
            case '8':
                Num8();
                break;
            case '9':
                Num9();
                break;
            case 'a':
            case 'A':
                A();
                break;
            case 'b':
            case 'B':
                B();
                break;
            case 'c':
            case 'C':
                C();
                break;
            case 'd':
            case 'D':
                D();
                break;
            case 'e':
            case 'E':
                E();
                break;
            case 'f':
            case 'F':
                F();
                break;
            case 'g':
            case 'G':
                G();
                break;
            case 'h':
            case 'H':
                H();
                break;
            case 'i':
            case 'I':
                I();
                break;
            case 'j':
            case 'J':
                J();
                break;
            case 'k':
            case 'K':
                K();
                break;
            case 'l':
            case 'L':
                L();
                break;
            case 'm':
            case 'M':
                M();
                break;
            case 'n':
            case 'N':
                N();
                break;
            case 'o':
            case 'O':
                O();
                break;
            case 'p':
            case 'P':
                P();
                break;
            case 'q':
            case 'Q':
                Q();
                break;
            case 'r':
            case 'R':
                R();
                break;
            case 's':
            case 'S':
                S();
                break;
            case 't':
            case 'T':
                T();
                break;
            case 'u':
            case 'U':
                U();
                break;
            case 'v':
            case 'V':
                V();
                break;
            case 'w':
            case 'W':
                W();
                break;
            case 'x':
            case 'X':
                X();
                break;
            case 'y':
            case 'Y':
                Y();
                break;
            case 'z':
            case 'Z':
                Z();
                break;
            default:
                break;
        }
        glEnd();
		glPopMatrix();
        str++;
		placement++;
    }
}
