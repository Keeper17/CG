

#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>


#include <IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "/usr/include/GL/glew.h"
#include <GL/glut.h>
#include "/usr/include/IL/il.h"
#endif


float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int scale = 150;
int alpha = 0, beta = 45, r = 50;

GLuint coordT, buff[1];

unsigned int t, tw, th;
unsigned char *imageData;


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

void loadJpg(){
    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)"terreno.jpg");

    ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);

    if(th != 256 || tw != 256) printf("Load Failed");
    else printf("Load sucessfull!");
    // imageData is a LINEAR array with the pixel values
    imageData = ilGetData();

}


float vertH(int x, int z){
    //os valores vao de 0 a 255
    //Como são demasiador valores para representar, usa-se uma escala mais pequena
    return ((imageData[x + z*tw]) *scale / 255);
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
    glTranslatef(-128,0,-128);

    //desenha uma "tira" de cada vez
    for(int i = 0 ; i< th; i++){
        //vai desenhar
        glDrawArrays(GL_TRIANGLE_STRIP, i*tw*2, 2*tw);
    }
}



void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	drawTerrain();

// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
}



void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			
			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * 							     sin(betaAux * 3.14 / 180.0);
}


void init() {

// 	Load the height map "terreno.jpg"
    loadJpg();
    if (th != 256 || tw != 256) printf("Load Failed");
    else {
        printf("Load sucessfull!");

// 	Build the vertex arrays
        newBuff(tw, th);
// 	OpenGL settings
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT, GL_LINE);
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
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	ilInit();
	glewInit();
	init();	

// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}

