/*******************
** Gabriele Di Bari
** 2011/06/15
********************/
#include <GL\GL.h>

//set scale
void Scale(float x,float y){
        glLoadIdentity();
        glScalef(x,y,1.0f);
}
//draw a point
void DrawPoint(int x,int y){
    glPushMatrix();
    glTranslatef(x,y,0.0f);
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glVertex3f(0.0f,0.0f, 0.0f);
            glVertex3f(1.0f,0.0f, 0.0f);
        glEnd();
 	glPopMatrix();
}
//set color
void Color(int r,int g,int b){
        glColor4f(r/255,g/255,b/255,1.0);
}
//clears the screen with a color
void Cls(int r,int g,int b){
        glClearColor(r/255,g/255,b/255,1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
