
#define _USE_MATH_DEFINES
#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>
#include "tinyxml2.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include "/usr/local/Cellar/devil/1.8.0_1/include/IL/il.h"
#else
#include "/usr/include/GL/glew.h"
#include <GL/glut.h>
#include "/usr/include/IL/il.h"
#endif

using namespace std;
using namespace tinyxml2;


float camX1 = 125, camY1, camZ1 = 125;

float alpha1 = 3.14f;
float betaz1 = 0.0f;



//Variaveis para alterar a velocidade dos movimentos
float rot = 100.0f;
float trans = 1000.0f;

//  Variaveis usadas para movimentar a camara
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
float camX = 50.0f, camY = 35.0f, camZ = 25.0f;

//  variaveis usadas no movimento da camara com o rato
int startX = 0, startY = 0, tracking = 0;
int alpha = 0, beta = 45, r = 50;

//  estrutura para guardar coordenadas de um ponto
struct Point {
    float coords[3];
};

//  estrutura para guardar informacoes de transformacoes (rotate/translate/scale/color/rotacao)
struct GeoTPoint{
    float angle;
    float x;
    float y;
    float z;
    float t;
};

struct Translacao{
    float u[3];
    float t;
    vector<Point> points;
    int c;
};

//  estrutura que guarda as diferentes transformacoes
struct GeometricTransf{
    GeoTPoint translate;
    GeoTPoint rotate;
    GeoTPoint scale;
    GeoTPoint color;
    GeoTPoint rotacao;
    Translacao trans;

};

//  estrutura que representa um grupo(plane/planeta e satelites)
struct Group{


    GeometricTransf transforms;
    vector<string> models;
    GLuint points[1], norms[1], tex[1];
    int id;
    int v_count;
    //Vetor com os ficheiros das texturas (.jpg and such)
    vector<string> texts;
    //informacao do subgrupo
    vector<Group> childGroups;
};

//  grupo final com o modelo inteiro
vector<Group> PrincipalGroups;


GeometricTransf newTrans(){
    GeometricTransf t;

    t.translate.x=0; t.translate.y=0; t.translate.z=0; t.translate.angle=0;

    t.scale.x=1; t.scale.y=1; t.scale.z=1; t.scale.angle=0;

    t.rotate.angle=0; t.rotate.x=0; t.rotate.y=0; t.rotate.z=0;

    t.color.angle=0; t.color.x=1; t.color.y=1; t.color.z=1;

    t.rotacao.t=0; t.rotacao.x=0; t.rotacao.y=0; t.rotacao.z=0;

    t.trans.t = -1; t.trans.c=0; t.trans.u[0]=0; t.trans.u[1]=1; t.trans.u[2]=0;


    return t;
}


//------------------------------------------------------------------- TEXTURES --------------------------------------------

//Mesma que foi utilizada na ficha 11
int loadTexture(string s) {

    unsigned int t,tw,th;
    unsigned char *texData;
    unsigned int texID;

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)s.c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1,&texID);

    glBindTexture(GL_TEXTURE_2D,texID);
    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,		GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,		GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;

}

//------------------------------------------------------------------- PARSING --------------------------------------------------------------------------------------


//  pega nas transformacoes e guarda a info na tranform
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
    if(atr != nullptr)tranform.angle = stof(atr);

    atr = transf->Attribute("axisX");
    if (atr != nullptr) tranform.x = stof(atr);

    atr = transf->Attribute("axisY");
    if (atr != nullptr) tranform.y = stof(atr);

    atr = transf->Attribute("axisZ");
    if (atr != nullptr) tranform.z = stof(atr);

    atr = transf->Attribute("tempo");
    if (atr != nullptr) tranform.t = stof(atr);

    atr = transf->Attribute("eX");
    if (atr != nullptr) tranform.x = stof(atr);

    atr = transf->Attribute("eY");
    if (atr != nullptr) tranform.y = stof(atr);

    atr = transf->Attribute("eZ");
    if (atr != nullptr) tranform.z = stof(atr);

    return tranform;
}

Translacao parsingTranslacao(XMLElement* trans, Translacao t){
    const char* atr = nullptr, *coordenadas = nullptr;

    atr = strdup(trans->Attribute("tempo"));

    if(atr != nullptr) t.t = stof(atr);

    XMLElement* pontoAtual = trans->FirstChildElement("ponto");

    while(pontoAtual != nullptr ){
        Point p;

        coordenadas = pontoAtual->Attribute("pX");
        if(atr != nullptr) p.coords[0] = stof(coordenadas);

        coordenadas = pontoAtual->Attribute("pY");
        if(atr != nullptr) p.coords[1] = stof(coordenadas);

        coordenadas = pontoAtual->Attribute("pZ");
        if(atr != nullptr) p.coords[2] = stof(coordenadas);

        t.points.push_back(p);

        pontoAtual = pontoAtual->NextSiblingElement("ponto");
        if (pontoAtual == nullptr ) return t;

    }

    return t;
}

//  guarda os diferentes modelos a usar, neste caso sao todos sphere
vector<string> parsingModels(XMLElement* modelos)
{
    vector<string> models;
    vector<string> texts;
    XMLElement* model = modelos->FirstChildElement("model");

    while(model != nullptr){

        models.push_back(model->Attribute("file"));
        texts.push_back(model->Attribute("texture"));


        model = model->NextSiblingElement("model");
    }

    return models;
}

vector<string> parsingTextures(XMLElement* modelos)
{
    vector<string> texts;
    XMLElement* model = modelos->FirstChildElement("model");

    while(model != nullptr){

        texts.push_back(model->Attribute("texture"));


        model = model->NextSiblingElement("model");
    }

    return texts;
}


//----------------------------------------------------------------- CURVAS --------------------------------------------------------------------
void buildRotMatrix(float *x, float *y, float *z, float *m) {

    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void cross(float *a, float *b, float *res) {

    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

void normalize(float *a) {

    float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0]/l;
    a[1] = a[1]/l;
    a[2] = a[2]/l;
}

void multMatrixVector(float *m, float *v, float *res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }
}

void getCatmullRomPoint(float t, float *p0, float *p1, float *p2, float *p3, float *pos, float *deriv) {

    float m[4][4] = {    {-0.5f,  1.5f, -1.5f,  0.5f},
                         { 1.0f, -2.5f,  2.0f, -0.5f},
                         {-0.5f,  0.0f,  0.5f,  0.0f},
                         { 0.0f,  1.0f,  0.0f,  0.0f}   };

    float a[4][4] = {0.0f};
    float xa[4] = { p0[0], p1[0], p2[0], p3[0]};
    float ya[4] = { p0[1], p1[1], p2[1], p3[1]};
    float za[4] = { p0[2], p1[2], p2[2], p3[2]};
    float  posaux[4] = { powf(t,3), powf(t,2), t, 1};
    float posdaux[4] = { 3* powf(t,2), 2 * t, 1, 0 };

    multMatrixVector(*m,xa,a[0]);
    multMatrixVector(*m,ya,a[1]);
    multMatrixVector(*m,za,a[2]);


    multMatrixVector(*a,posaux,pos);


    multMatrixVector(*a,posdaux,deriv);
}

void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<Point> pontos) {
    int point_count = pontos.size();
    float t = gt * (float)point_count;
    int index = floor(t);
    t = t - (float)index;

    int indices[4];
    indices[0] = (index + point_count-1)%point_count;
    indices[1] = (indices[0]+1)%point_count;
    indices[2] = (indices[1]+1)%point_count;
    indices[3] = (indices[2]+1)%point_count;

    getCatmullRomPoint(t, pontos[indices[0]].coords, pontos[indices[1]].coords, pontos[indices[2]].coords, pontos[indices[3]].coords, pos, deriv);
}

void renderCatmullRomCurve(vector<Point> p) {
    float pos[4];
    float deriv[4];
    float i;
    glBegin(GL_LINE_LOOP);
    glColor3f(1,0,0);
    for ( i = 0.0; i < 1.0; i+=0.001) {
        glColor3f(1,1,1);
        getGlobalCatmullRomPoint(i,pos,deriv,p);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
}

//----------------------------------------------------------------------------------------------- ANIMACOES -----------------------------------------------------------

void rotacaoAni(GeoTPoint a){
    float tempoP = glutGet(GLUT_ELAPSED_TIME);
    float ang = 360/(a.t*rot);//Define a rapidez da rotacao

    glRotatef(tempoP*ang,a.x,a.y,a.z);
}

void translacaoAni(Translacao a){
    float pos[4], deriv[4], Z[3], m[16];

    renderCatmullRomCurve(a.points);


    float tempoP = glutGet(GLUT_ELAPSED_TIME) / trans;//Altera a velocidade de transalcao dos planetas
    float tempo = tempoP - (a.t)*(a.c);

    if(tempo > (a.t)) {//Acabou de dar uma volta
        a.c++;
        tempo = tempoP - (a.t)*(a.c);
    }

    float gt = tempo/(a.t);
    getGlobalCatmullRomPoint( gt, pos, deriv, a.points);
    //vetor X
    normalize(deriv);
    //vetor Z
    cross(deriv,(a.u),Z);
    normalize(Z);
    //vetor Y
    cross(Z,deriv,(a.u));
    normalize(a.u);

    buildRotMatrix(deriv,a.u,Z,m);

    glTranslatef(pos[0],pos[1],pos[2]);
    glMultMatrixf(m);
}

//---------------------------------------------------------------------------------- LOAD --------------------------------------------------------------------------
// vai receber quais os modelos a desenhar e vai por os pontos desses modelos no vetor com os pontos todos
Group loadModels(Group p){
    string line1, line2;
    float x, y, z, nx, ny, nz, tx, tz;
    GLuint count = 0;

    vector<float> points;
    vector<float> norms;
    vector<float> tex;

    vector<string> modelos = p.models;

    glEnableClientState(GL_VERTEX_ARRAY);

    for(string filename : modelos){

        ifstream model_file(filename, ios::in | ios::binary);
        ifstream tex_file(filename +  ".lat", ios::in | ios::binary);

        while(getline(model_file, line1, '\0')) {
            istringstream line_parser1(line1);

            line_parser1 >> x >> y >> z;

            points.push_back(x);
            points.push_back(y);
            points.push_back(z);

            count++;
        }

        while(getline(tex_file, line2, '\0')){
            istringstream line_parser2(line2);

            line_parser2 >> nx >> ny >> nz >> tx >> tz;

            norms.push_back(nx);
            norms.push_back(ny);
            norms.push_back(nz);

            tex.push_back(tx);
            tex.push_back(tz);
        }


        model_file.close();
        tex_file.close();

        glGenBuffers(1, p.points);
        glBindBuffer(GL_ARRAY_BUFFER, p.points[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 3, &points[0], GL_STATIC_DRAW);

        glGenBuffers(1, p.norms);
        glBindBuffer(GL_ARRAY_BUFFER, p.norms[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 3, &norms[0], GL_STATIC_DRAW);

        glGenBuffers(1, p.tex);
        glBindBuffer(GL_ARRAY_BUFFER, p.tex[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 2, &tex[0], GL_STATIC_DRAW);

        p.v_count = count;
        p.id = loadTexture(p.texts[0]);
    }
    return p;
}


Group parsingGroups(XMLElement* group)
{
    Group groupp;
    groupp.transforms = newTrans();
    groupp.transforms.translate = parsingGeoTransf(group->FirstChildElement("translate"),groupp.transforms.translate);
    groupp.transforms.rotate = parsingGeoTransf(group->FirstChildElement("rotate"),groupp.transforms.rotate);
    groupp.transforms.scale = parsingGeoTransf(group->FirstChildElement("scale"),groupp.transforms.scale);
    groupp.transforms.color = parsingGeoTransf(group->FirstChildElement("color"),groupp.transforms.color);
    if(group->FirstChildElement("translacao")){
        groupp.transforms.trans = parsingTranslacao(group->FirstChildElement("translacao"),groupp.transforms.trans);
    }
    groupp.transforms.rotacao = parsingGeoTransf(group->FirstChildElement("rotacao"),groupp.transforms.rotacao);
    groupp.models = parsingModels(group->FirstChildElement("models"));
    groupp.texts = parsingTextures(group->FirstChildElement("models"));
    groupp = loadModels(groupp);

    XMLElement* childGroups = group->FirstChildElement("group");

    while (childGroups != nullptr){
        groupp.childGroups.push_back(parsingGroups(childGroups));
        childGroups = childGroups->NextSiblingElement("group");
    }
    return groupp;
}


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
        PrincipalGroups.push_back(groupX);
        groupL = groupL->NextSiblingElement("group");
    }

    return XML_SUCCESS;
}

//-------------------------------------------------------- FUNCOES ADICIONAIS DE DESENHO -----------------------------------------------------------

void drawEixosP(){
    glBegin(GL_LINES);
    glColor3f(0.0, 100, 0.0);
    glVertex3f(0.0, -2, 0.0);
    glVertex3f(0.0, 2, 0.0);
    glEnd();
}

void drawAxes(){
    glBegin(GL_LINES);

    // draw line for x axis
    glColor3f(100, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(100, 0.0, 0.0);

    // draw line for y axis
    glColor3f(0.0, 100, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 100, 0.0);

    // draw line for Z axis
    glColor3f(0.0, 0.0, 100);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 100);
    glEnd();
}




void draw(vector<Group> grupos){
    for(Group p : grupos) {

        glPushMatrix();
        glTranslatef(p.transforms.translate.x, p.transforms.translate.y, p.transforms.translate.z);
        if(p.transforms.trans.t != -1) {
            translacaoAni(p.transforms.trans);
        }
        glRotatef(p.transforms.rotate.angle, p.transforms.rotate.x, p.transforms.rotate.y, p.transforms.rotate.z);
        glScalef(p.transforms.scale.x, p.transforms.scale.y, p.transforms.scale.z);
        rotacaoAni(p.transforms.rotacao);
        //drawEixosP();
        glColor3f(p.transforms.color.x,p.transforms.color.y,p.transforms.color.z);

        draw(p.childGroups);

        glBindTexture(GL_TEXTURE_2D, p.id);

        glBindBuffer(GL_ARRAY_BUFFER, p.points[0]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, p.norms[0]);
        glNormalPointer(GL_FLOAT, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, p.tex[0]);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, p.v_count);

        glBindTexture(GL_TEXTURE_2D, 0);

        glPopMatrix();
    }
}


void renderScene(){

    float pos[4] = {1.0, 1.0, 1.0, 0.0};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX,camY,camZ,
              centerX,centerY,centerZ,
              upX,upY,upZ);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    float white[4] = { 1,1,1,1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    draw(PrincipalGroups);
    //glEnd();
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
        case  'z' :
            rot*=2;
            trans*=2;
            break;
        case  'x' :
            rot*=0.5;
            trans*=0.5;
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


void processMouseMotion(int xx, int yy){

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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(500,100);
    glutInitWindowSize(1000,800);
    glutCreateWindow("CG@DI Phase 4 - Lighting and Textures");

    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);


#ifndef __APPLE__
    glewInit();
#endif


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_NORMALIZE);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    loadFile(argv[1]);

    glutMainLoop();


    return 1;
}