
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

//Variaveis usadas para movimentar a camara
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
float camX = 60.0f, camY = 35.0f, camZ = 25.0f;

//variaveis usadas no movimento da camara com o rato
int startX = 0, startY = 0, tracking = 0;
int alpha = 0, beta = 45, r = 50;

//estrutura para guardar coordenadas de um ponto
struct Point {
    float x;
    float y;
    float z;
};

//estrutura para guardar informacoes de transformacoes (rotate/translate/scale)
struct GeoTPoint{
    float angle;
    float x;
    float y;
    float z;
};

//estrutura que guarda as diferentes transformacoes
struct GeometricTransf{
    GeoTPoint translate;
    GeoTPoint rotate;
    GeoTPoint scale;
};

//estrutura que representa um grupo(plane/planeta e satelites)
struct Group{
    // todas as transformaçoes do grupo
    GeometricTransf transforms;
    // todos os modelos
    vector<string> models;
    //vertices para desenhar o grupo
    vector<Point> points;
    //informacao do subgrupo
    vector<Group> childGroups;
};

//grupo final com o modelo inteiro
vector<Group> PrincipalGroups;

//inicializa uma tranformacao
GeometricTransf newTrans(){
    GeometricTransf t;

    t.translate.x=0;
    t.translate.y=0;
    t.translate.z=0;
    t.translate.angle=0;

    t.scale.x=1;
    t.scale.y=1;
    t.scale.z=1;
    t.scale.angle=0;

    t.rotate.angle=0;
    t.rotate.x=0;
    t.rotate.y=0;
    t.rotate.z=0;

    return t;
}

//pega nas transformacoes e guarda a info na tranform
GeoTPoint parsingGeoTransf(XMLElement* transf ,GeoTPoint tranform)
{
    const char* atr = nullptr;

    if( transf == nullptr ) return tranform;

    atr = transf->Attribute("X");
    if(atr != nullptr) tranform.x = stof(atr);

    atr = transf->Attribute("Y");
    if(atr != nullptr) tranform.y = stof(atr);

    atr = transf->Attribute("Z");
    if(atr != nullptr) tranform.z = stof(atr);

    atr = transf->Attribute("angle");
    if(atr != nullptr) tranform.angle = stof(atr);

    atr = transf->Attribute("axisX");
    if(atr != nullptr) tranform.x = stof(atr);

    atr = transf->Attribute("axisY");
    if(atr != nullptr) tranform.y = stof(atr);

    atr = transf->Attribute("axisZ");
    if(atr != nullptr) tranform.z = stof(atr);

    return tranform;
}

//  guarda os diferentes modelos a usar, neste caso sao todos sphere
vector<string> parsingModels(XMLElement* modelos)
{
    vector<string> models;
    XMLElement* model = modelos->FirstChildElement("model");

    while(model != nullptr){

        models.push_back(model->Attribute("file"));
        model = model->NextSiblingElement("model");
    }

    return models;
}

vector<Point> loadModels(vector<string> modelos){
    string line;
    Point pn;
    float x, y, z;

    vector<Point> points;

    for(string filename : modelos){

        ifstream model_file(filename, ios::in | ios::binary);
        while(getline(model_file, line, '\0')) {
            istringstream line_parser(line);
            line_parser >> x >> y >> z;
            pn.x = x;
            pn.y = y;
            pn.z = z;
            points.push_back(pn);
        }

        model_file.close();
        cout<<"Load de"+ filename + "com sucesso"<<endl;
    }
    return points;
}

// Vai chamar chamar as outras funcoes de parsing para os elementos de um grupo
Group parsingGroups(XMLElement* group)
{
    Group groupp;
    groupp.transforms = newTrans();

    groupp.transforms.translate = parsingGeoTransf(group->FirstChildElement("translate"),groupp.transforms.translate);
    groupp.transforms.rotate = parsingGeoTransf(group->FirstChildElement("rotate"),groupp.transforms.rotate);
    groupp.transforms.scale = parsingGeoTransf(group->FirstChildElement("scale"),groupp.transforms.scale);
    groupp.models = parsingModels(group->FirstChildElement("models"));
    groupp.points = loadModels(groupp.models);

    XMLElement* childGroups = group->FirstChildElement("group");

    while (childGroups != nullptr){
        groupp.childGroups.emplace_back(parsingGroups(childGroups));
        childGroups = childGroups->NextSiblingElement("group");
    }
    return groupp;
}

//Vai ler o ficheiro XML
XMLError loadFile(const char* filename)
{
    XMLDocument doc;
    if(doc.LoadFile(filename) != XML_SUCCESS) {
        return XML_ERROR_FILE_READ_ERROR;
    }

    XMLNode* scene = doc.RootElement();
    if(scene == nullptr) return XML_ERROR_FILE_READ_ERROR;

    XMLElement* groupL = scene->FirstChildElement("group");
    if( groupL == nullptr) return XML_ERROR_PARSING_ELEMENT;

    while(groupL != nullptr){
        Group groupX = parsingGroups(groupL);
        PrincipalGroups.emplace_back(groupX);
        groupL = groupL->NextSiblingElement("group");
    }

    return XML_SUCCESS;
}

//Percorre o vetor principal e desenha os pontos de cada grupo
void draw(vector<Group> grupos){
    for(Group p : grupos) {
        glPushMatrix();
        glTranslatef(p.transforms.translate.x, p.transforms.translate.y, p.transforms.translate.z);
        glScalef(p.transforms.scale.x, p.transforms.scale.y, p.transforms.scale.z);
        glRotatef(p.transforms.rotate.angle, p.transforms.rotate.x, p.transforms.rotate.y, p.transforms.rotate.z);

        draw(p.childGroups);

        int c = 0;
        glBegin(GL_TRIANGLES);
        glColor3f(0.0, 0.3, 0.8);
        for( Point pp : p.points){
            if (c == 6) {
                c = 0;
                glColor3f(1.0, 0.0, 0.0);
            }
            if (c == 3) {
                glColor3f(0.0, 0.0, 1.0);
            }
            glVertex3f(pp.x, pp.y, pp.z);
            c++;
        }
        glEnd();
        glPopMatrix();
    }
}


void renderScene(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(camX,camY,camZ,
              centerX,centerY,centerZ,
              upX,upY,upZ);

    draw(PrincipalGroups);
    glEnd();
    glutSwapBuffers();
}


void changeSize(int w, int h){
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


void specialKeys(int key_code, int x, int y){
    if (key_code == GLUT_KEY_LEFT){
        camX += 1;
        centerX += 1;
    }
    if (key_code == GLUT_KEY_RIGHT){
        camX -= 1;
        centerX -= 1;
    }
    if (key_code == GLUT_KEY_DOWN){
        camZ += 1;
        centerZ += 1;
    }
    if (key_code == GLUT_KEY_UP){
        camZ -= 1;
        centerZ -= 1;
    }

    glutPostRedisplay();
}

void keys(unsigned char key, int x, int y){
    switch(key){
        case  'r' :
            camX = 40.0f;
            camY = 25.0f;
            camZ = 25.0f;
            centerX = 15;
            centerZ = 0;
            centerY = 0;
            alpha = 0;
            beta = 45;
            r = 50;
            tracking = 0;
            break;
    }
    glutPostRedisplay();
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
    camY = rAux * sin(betaAux * 3.14 / 180.0);
}


int main(int argc, char** argv) {

    loadFile(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("CG@DI Phase 2 - Solar System");

    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();

    return 1;
}
