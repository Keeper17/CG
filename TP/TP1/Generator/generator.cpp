#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

struct Point {
    float x;
    float y;
    float z;
};

void writePoint(Point p, ofstream &out_file){
    string phrase = to_string(p.x) + " " + to_string(p.y) + " " + to_string(p.z) + "\n";
    out_file.write(phrase.c_str(), phrase.size() + 1);
}

void drawSphere(float radius, int stacks, int slices, ofstream &out_file) {
    float a, b, aNext, bNext;
    for(int i = -stacks; i < stacks; i++){
        a     = (((float)i)    /((float)stacks/2.0))*(M_PI/2.0);
        aNext = (((float)i-1.0)/((float)stacks/2.0))*(M_PI/2.0);
        for(int j = 0; j < slices; j++){
            b     = (((float)j)    /(float)slices)*(2.0*M_PI);
            bNext = (((float)j+1.0)/(float)slices)*(2.0*M_PI);

            Point pA, pB, pC, pD;

            pA = {radius * cos(a) * sin(b), radius * sin(a), radius * cos(a) * cos(b)};
            pB = {radius * cos(a) * sin(bNext), radius * sin(a), radius * cos(a) * cos(bNext)};
            pC = {radius * cos(aNext) * sin(bNext), radius * sin(aNext), radius * cos(aNext) * cos(bNext)};
            pD = {radius * cos(aNext) * sin(b), radius * sin(aNext), radius * cos(aNext) * cos(b)};

            writePoint(pA, out_file);
            writePoint(pD, out_file);
            writePoint(pB, out_file);

            writePoint(pB, out_file);
            writePoint(pD, out_file);
            writePoint(pC, out_file);
        }
    }
}

void drawConeBase(float radius, int slices, ofstream& out_file) {
    float a, aNext;
    for(int i = 0; i < slices; i++){
        a     = (((float)i)/(float)slices)*(2.0*M_PI);
        aNext = (((float)i+1.0)/(float)slices)*(2.0*M_PI);

        Point pA, pB, pC;

        pA = {radius * sin(a), 0.0, radius * cos(a)};
        pB = {radius * sin(aNext), 0.0, radius * cos(aNext)};
        pC = {0.0,0.0,0.0};

        writePoint(pC, out_file);
        writePoint(pB, out_file);
        writePoint(pA, out_file);
    }
}

void drawConeSides(float radius, float height, int stacks, int slices, ofstream& out_file){
    float a, aNext;
    float currHeight = 0.0f;
    float currRadius = radius;
    float heightDiff = height/(float)stacks;
    float radiusDiff = radius/(float)stacks;

    for(int i = 0; i < stacks; i++){
        for(int j = 0; j < slices; j++){
            a     = (((float)j)/(float)slices)*(2.0*M_PI);
            aNext = (((float)j+1.0)/(float)slices)*(2.0*M_PI);

            Point pA, pB, pC, pD;

            pA = {(currRadius - radiusDiff) * sin(a), currHeight + heightDiff, (currRadius - radiusDiff) * cos(a)};
            pB = {(currRadius - radiusDiff) * sin(aNext), currHeight + heightDiff, (currRadius - radiusDiff) * cos(aNext)};
            pC = {currRadius * sin(a), currHeight, currRadius * cos(a)};
            pD = {currRadius * sin(aNext), currHeight,currRadius * cos(aNext)};

            writePoint(pA, out_file);
            writePoint(pD, out_file);
            writePoint(pB, out_file);

            writePoint(pA, out_file);
            writePoint(pC, out_file);
            writePoint(pD, out_file);
        }
        currRadius -= radiusDiff;
        currHeight += heightDiff;
    }
}

void drawCone(float radius, float height, int stacks, int slices, ofstream& out_file) {
    drawConeBase(radius, slices, out_file);
    drawConeSides(radius, height, stacks, slices, out_file);
}

void drawPlane(float width, ofstream& out_file) {
    Point pA, pB, pC, pD;

    float n = width/2;

    pA = { n, 0,  -n};
    pB = {-n, 0,  -n};
    pC = {-n, 0,  n};
    pD = { n, 0,  n};

    //Up
    writePoint(pD, out_file);
    writePoint(pA, out_file);
    writePoint(pB, out_file);

    writePoint(pB, out_file);
    writePoint(pC, out_file);
    writePoint(pD, out_file);
    
    //Down
    writePoint(pB, out_file);
    writePoint(pA, out_file);
    writePoint(pD, out_file);

    writePoint(pD, out_file);
    writePoint(pC, out_file);
    writePoint(pB, out_file);
}

void drawOuterBox(float xlen, float ylen, float zlen, ofstream& out_file) {
    Point pA, pB, pC, pD, pE, pF, pG, pH;

    pA = {xlen, 0    , 0};
    pB = {0   , 0    , 0};
    pC = {0   , 0    , zlen};
    pD = {xlen, 0    , zlen};
    pE = {xlen, ylen , 0};
    pF = {0   , ylen , 0};
    pG = {0   , ylen , zlen};
    pH = {xlen, ylen , zlen};

    // Base
    writePoint(pD, out_file);
    writePoint(pA, out_file);
    writePoint(pB, out_file);

    writePoint(pB, out_file);
    writePoint(pC, out_file);
    writePoint(pD, out_file);

    // Base Dentro
    writePoint(pB, out_file);
    writePoint(pA, out_file);
    writePoint(pD, out_file);

    writePoint(pD, out_file);
    writePoint(pC, out_file);
    writePoint(pB, out_file);

    // Topo
    writePoint(pH, out_file);
    writePoint(pE, out_file);
    writePoint(pF, out_file);

    writePoint(pF, out_file);
    writePoint(pG, out_file);
    writePoint(pH, out_file);

    // Topo Dentro
    writePoint(pF, out_file);
    writePoint(pE, out_file);
    writePoint(pH, out_file);

    writePoint(pH, out_file);
    writePoint(pG, out_file);
    writePoint(pF, out_file);

    //Frente
    writePoint(pD, out_file);
    writePoint(pH, out_file);
    writePoint(pG, out_file);

    writePoint(pG, out_file);
    writePoint(pC, out_file);
    writePoint(pD, out_file);

    //Frente Dentro
    writePoint(pG, out_file);
    writePoint(pH, out_file);
    writePoint(pD, out_file);

    writePoint(pD, out_file);
    writePoint(pC, out_file);
    writePoint(pG, out_file);

    //Costas
    writePoint(pA, out_file);
    writePoint(pE, out_file);
    writePoint(pF, out_file);

    writePoint(pF, out_file);
    writePoint(pB, out_file);
    writePoint(pA, out_file);

    //Costas Dentro
    writePoint(pF, out_file);
    writePoint(pE, out_file);
    writePoint(pA, out_file);

    writePoint(pA, out_file);
    writePoint(pB, out_file);
    writePoint(pF, out_file);

    //Direita
    writePoint(pD, out_file);
    writePoint(pA, out_file);
    writePoint(pE, out_file);

    writePoint(pE, out_file);
    writePoint(pH, out_file);
    writePoint(pD, out_file);

    //Direita Dentro
    writePoint(pE, out_file);
    writePoint(pA, out_file);
    writePoint(pD, out_file);

    writePoint(pD, out_file);
    writePoint(pH, out_file);
    writePoint(pE, out_file);

    //Esquerda
    writePoint(pB, out_file);
    writePoint(pF, out_file);
    writePoint(pG, out_file);

    writePoint(pG, out_file);
    writePoint(pC, out_file);
    writePoint(pB, out_file);

    //Esquerda Dentro
    writePoint(pG, out_file);
    writePoint(pF, out_file);
    writePoint(pB, out_file);

    writePoint(pB, out_file);
    writePoint(pC, out_file);
    writePoint(pG, out_file);
}

void drawInnerBox(float xlen, float ylen, float zlen, int div, ofstream& out_file) {
    float dist = xlen/div;
    Point pA, pB, pC, pD;
    for(int i = 1; i < div; i++){
        pA = {dist * i, ylen, zlen};
        pB = {dist * i, ylen, 0};
        pC = {dist * i, 0   , 0};
        pD = {dist * i, 0   , zlen};

        //Frente
        writePoint(pD, out_file);
        writePoint(pA, out_file);
        writePoint(pB, out_file);

        writePoint(pB, out_file);
        writePoint(pC, out_file);
        writePoint(pD, out_file);

        //Trás
        writePoint(pB, out_file);
        writePoint(pA, out_file);
        writePoint(pD, out_file);

        writePoint(pD, out_file);
        writePoint(pC, out_file);
        writePoint(pB, out_file);
    }
}

void drawBox(float xlen, float ylen, float zlen, int div, ofstream& out_file) {
    drawOuterBox(xlen, ylen, zlen, out_file);
    drawInnerBox(xlen, ylen, zlen, div, out_file);
}

int main(int argc, char** argv) {
    int val = 0;
    string shape = argv[1];
    ofstream output_file;

    if((shape.compare("sphere")) == 0) val = 1;
    if((shape.compare("cone")) == 0) val = 2;
    if((shape.compare("box")) == 0) val = 3;
    if((shape.compare("plane")) == 0) val = 4;

    switch(val){
        case 1 : {
            if(!argv[5]){
                cout << "No output file given!" << endl;
            }
            else{
                output_file.open(argv[5], ios::out | ios::app | ios::binary);

                drawSphere(atof(argv[2]), atoi(argv[4]), atoi(argv[3]), output_file);

                output_file.close();
            }
            break;
        }
        case 2 : {
            if(!argv[6]){
                cout << "No output file given!" << endl;
            }
            else {
                output_file.open(argv[6], ios::out | ios::app | ios::binary);

                drawCone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), output_file);

                output_file.close();
            }
            break;
        }
        case 3 : {
            if(!argv[5]){
                cout << "No output file given!" << endl;
            }
            else {
                if(!argv[6]){
                    output_file.open(argv[5], ios::out | ios::app | ios::binary);

                    drawOuterBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), output_file);

                    output_file.close();
                } else{
                    output_file.open(argv[6], ios::out | ios::app | ios::binary);

                    drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), output_file);

                    output_file.close();
                }                
            }
            break;
        }
        case 4 : {
            if(!argv[2]){
                cout << "No output file given!" << endl;
            }
            else{
                output_file.open(argv[3], ios::out | ios::app | ios::binary);

                drawPlane(atof(argv[2]), output_file);

                output_file.close();
            }
            break;
        }
        default : {
            cout << "Invalid arguments" << endl;
        }
    }
}
