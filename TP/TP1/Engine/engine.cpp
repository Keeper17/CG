#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <sstream>
#include "tinyxml2.h"
#include <math.h>

using namespace std;
using namespace tinyxml2;

// variaveis de rotação
float angleX = 0;
float angleY = 0;

float a = M_PI/6;
float b = M_PI/6;
float radius = 5;

struct Point {
    float x;
    float y;
    float z;
};

struct Shape {
    vector<Point> vertices;
};

vector<string> files;
vector<Shape> shapes;


XMLError loadFile(const char* filename)
{
    XMLDocument doc;
    if(doc.LoadFile(filename) != XML_SUCCESS) {
        return XML_ERROR_FILE_READ_ERROR;
    }

    XMLNode* scene = doc.FirstChild();

    if(scene == nullptr) {
        return XML_ERROR_FILE_READ_ERROR;
    }

    XMLElement* model = scene->FirstChildElement("model");

    while(model != nullptr) {
        const char* file = nullptr;
        file = model->Attribute("file");
        if (file == nullptr) return XML_ERROR_PARSING;
        files.emplace_back(file);
        model = model->NextSiblingElement("model");
    }
    return XML_SUCCESS;

}

void loadModel(const string filename)
{
    ifstream model_file(filename, ios::in | ios::binary);
    string line;
    Point point;
    Shape shape;
    float x, y, z;

    while(getline(model_file, line, '\0')) {
        istringstream line_parser(line);
        line_parser >> x >> y >> z;
        point.x = x;
        point.y = y;
        point.z = z;
        shape.vertices.emplace_back(point);
    }
    shapes.emplace_back(shape);
    cout<<"Added new shape"<<endl;
    model_file.close();
}

void renderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float px = radius*cos(b)*sin(a);
    float py = radius*sin(b);
    float pz = radius*cos(b)*cos(a);

    glLoadIdentity();
    gluLookAt(px,py,pz,
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);

    glBegin(GL_LINES);
        //X Axis
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-1000.0f, 0.0f, 0.0f);
        glVertex3f( 1000.0f, 0.0f, 0.0f);
        //Y Axis
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -1000.0f, 0.0f);
        glVertex3f(0.0f, 1000.0f, 0.0f);
        //Z Axis
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -1000.0f);
        glVertex3f(0.0f, 0.0f, 1000.0f);
    glEnd();

    glRotatef(angleY, 0, 1, 0);
    glRotatef(angleX, 1, 0, 0);

    int c = 0;
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.3, 0.8);
    for(Shape sh : shapes) {
        for (Point pt : sh.vertices) {
            glColor3f(0.0, 0.0, 1.0);
            //Triangles will be drawn with alternating colors so as to be distinguished from each other
            if (c == 6) {
                c = 0;
                glColor3f(1.0, 0.0, 0.0);
            }
            if (c == 3) {
                glColor3f(0.0, 0.0, 1.0);
            }
            glVertex3f(pt.x, pt.y, pt.z);
            c++;
        }
    }
    glEnd();
    glutSwapBuffers();
}

void printVertices(void){
    for(Shape sh : shapes)
        for(Point pt : sh.vertices){
            cout<<pt.x<<pt.y<<pt.z<<endl;
        }
}

void changeSize(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key_code, int x, int y)
{
    if (key_code == GLUT_KEY_LEFT) a -= M_PI/30;
    if (key_code == GLUT_KEY_RIGHT) a += M_PI/30;
    if (key_code == GLUT_KEY_DOWN) b += M_PI/30;
    if (key_code == GLUT_KEY_UP) b -= M_PI/30;

    glutPostRedisplay();
}

void keys(unsigned char key, int x, int y) {
    switch(key){
        case 'a' :
            angleY -= 5;
            break;
        case 'w' :
            angleX += 5;
            break;
        case 's' :
            angleX -= 5;
            break;
        case 'd' :
            angleY += 5;
            break;
        case  'r' :
            a = 0;
            b = 0;
            angleX = 0;
            angleY = 0;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {

    if(!argv[1])
        cout<<"No input file given!"<<endl;
    else {
        XMLError open = loadFile(argv[1]);

        if(open != XML_SUCCESS)
            cout<<"Unable to open XML file!"<<endl;
        else
            cout<<"XML file loaded successfully!"<<endl;

        for(const string file : files) {
            loadModel(file);
        }
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("CG@DI Phase 1 - Engine");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();

    return 1;
}

