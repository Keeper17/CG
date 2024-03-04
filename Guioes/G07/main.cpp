

#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <IL/il.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "/usr/include/GL/glew.h"
#include <GL/glut.h>
#include "/usr/include/IL/il.h"
#endif


float camX = 125, camY, camZ = 125;

float alpha = 3.14f;
float betaz = 0.0f;


int scale = 250;

GLuint coordT, buff[1];

unsigned int t, tw, th;
unsigned char *imageData;

int nTress = 300;
float *arvores;
float angulo1 = 0;
float angulo2 = 0;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


float vertH(int x, int z){
    //os valores vao de 0 a 255
    //Como são demasiador valores para representar, usa-se uma escala mais pequena
    return ((imageData[x + z*tw]) *scale / 255);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void arvPos(int n){
    float x, z;
    int i;
    int a =0;
    arvores = (float*) malloc(n*2*sizeof(float));

    srand ((unsigned int) time(NULL));
    int k = 0;
    while(k<n){
        z = 0;
        x = 0;

        while( pow(x,2) + pow(z,2)<2500 ) {

            x = (float)(rand() % 250 - 125);
            z = (float)(rand() % 250 - 125);
        }
        arvores[a++] = x+125;
        arvores[a++] = z+125;
        k++;
    }
}



float hf(float x, float z){
    float x1 = floor(x);
    float x2 = ceil(x);
    float z1 = floor(z) ;
    float z2 = ceil(z);
    float fx = x - x1;
    float fz = z - z1;

    float hx1z = vertH(x1,z1)*(1-fz) + vertH(x1,z2)*fz;
    float hx2z = vertH(x2,z1)*(1-fz) + vertH(x2,z2)*fz;

    float hxz = hx1z*(1-fx) + hx2z*fx;

    return hxz;
}



void desenhaArv(int n){
    int i, l = 0;
    float x, z;

    for(i = 0; i < n; i++){
        glPushMatrix();
        x = arvores[l++];
        z = arvores[l++];
        float h = hf(x,z);
        //printf("%d\n",i);
        glPushMatrix();
        glTranslatef(x, h, z);
        glRotatef(-90, 1, 0, 0);

        glColor3f(0.4,0.26,0.13);
        glutSolidCone(0.5, 4, 16, 8);

        glTranslatef(0,0,2);
        glColor3f(0.0,0.38,0.0);
        glutSolidCone(3, 6, 16, 10);

        glPopMatrix();
    }

}

void desenhaTeaI(int n){
    int i;
    float alpha;
    glColor3f(0,0,0.8);

    for(i=0; i<n; i++){
        alpha = i*(360/n);

        glPushMatrix();
        glTranslatef(125,0,125);
        glRotatef(angulo1 + alpha,0,1,0);
        glTranslatef(15,2,0);
        glutSolidTeapot(2);
        glPopMatrix();
    }

    angulo1 -= 0.5;
}

void desenhaTeaE(int n){
    int i;
    float alpha;
    glColor3f(1,0,0);

    for(i=0; i<n; i++){
        alpha = i*(360/n);

        glPushMatrix();
        glTranslatef(125,0,125);
        glRotatef(angulo2 + alpha,0,1,0);
        glTranslatef(35,2,0);
        glutSolidTeapot(2);
        glPopMatrix();
    }

    angulo2 += 0.5;
}

/*------------------------------------------------------------------------------------------------------------------------*/


void loadJpg(){
    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)"terreno.jpg");

    ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);

    if(th != 256 || tw != 256) printf("Load Failed");
    else printf("Load sucessfull!");
    imageData = ilGetData();
}

void newBuff(float w, float h){
    float *vertexB = NULL;
    int size = 0;

    //2 triangulos, mas é preciso repetir alguns pontos
    coordT = (w-1) * h * 2 * 3;
    //preparar o array que vai ter os vertices do terreno
    vertexB = (float*) malloc(sizeof(float) * coordT);

    glEnableClientState(GL_VERTEX_ARRAY);

    for(int x = 0; x < w-1; x++){
        for(int z = 0; z < h; z++){
            vertexB[size++] = (x+1);
            vertexB[size++] = vertH(x+1,z); //para preencher com a altura
            vertexB[size++] = z;

            vertexB[size++] = x;
            vertexB[size++] = vertH(x,z);  //para preencher com a altura
            vertexB[size++] = z;
        }
    }

    glGenBuffers(1, buff);
    glBindBuffer(GL_ARRAY_BUFFER, buff[0]);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertexB, GL_STATIC_DRAW);
}

void drawTerrain() {
    // colocar aqui o código de desnho do terreno usando VBOs com TRIANGLE_STRIPS
    glVertexPointer(3,GL_FLOAT,0,0);
    //desenha uma "tira" de cada vez
    for(int i = 0 ; i< th; i++){
        //vai desenhar
        glColor3f(0.139,0.69,0.19);
        glDrawArrays(GL_TRIANGLE_STRIP, i*tw*2, 2*tw);
    }
}

void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, 2.0f+hf(camX,camZ), camZ,
              sin(alpha)+camX,2.0f + hf(camX,camZ) + sin(betaz),cos(alpha)+camZ,
			  0.0f,1.0f,0.0f);

    drawTerrain();
    desenhaArv(nTress);
    desenhaTeaI(8);
    desenhaTeaE(16);

    glTranslatef(125,0,125);
    glColor3f(1,0,0.5);
    glutSolidTorus(1,3,20,20);

// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char c, int xx, int yy) {
// put code to process regular keys in here
    switch(c){
        case 'w': camX += sin(alpha); camZ += cos(alpha);
            break;
        case 's': camX -= sin(alpha); camZ -= cos(alpha);
            break;
        case 'a': camX -= cos(3.14-alpha); camZ -= sin(3.14-alpha);
            break;
        case 'd': camX += cos(3.14-alpha); camZ += sin(3.14-alpha);
            break;
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {

        case GLUT_KEY_RIGHT:
            alpha -= 0.1f; break;

        case GLUT_KEY_LEFT:
            alpha += 0.1f; break;

        case GLUT_KEY_DOWN:
            betaz -= 0.1f;break;

        case GLUT_KEY_UP:
            betaz += 0.1f;break;
    }
    glutPostRedisplay();

}

void init() {

// 	Load the height map "terreno.jpg"
    loadJpg();
    if (th != 256 || tw != 256) printf("Load Failed\n");
    else {
        printf("Load sucessfull!\n");

// 	Build the vertex arrays
        newBuff(tw, th);
// 	OpenGL settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");

// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

	ilInit();
	glewInit();
	init();

    arvPos(nTress);
// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}

