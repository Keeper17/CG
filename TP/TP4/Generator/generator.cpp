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
#include <cstring>

#endif

using namespace std;

struct Point {
    float x;
    float y;
    float z;

    //Para a normal do ponto (para iluminção)
    float nx;
    float ny;
    float nz;

    //Coordenadas na textura
    float tx;
    float ty;
};

vector<vector<int>> patchIndexes;
vector<float> bezierPoints;

int readBezierPatchFile(string filename){
    string line, token;
    int patchNo = 0;

    ifstream infile(filename);

    //Obter o número de patches
    if(getline(infile, line)) patchNo = stoi(line);

    //Obter cada uma das várias patches
    for(int i = 0; i < patchNo && getline(infile,line); i++){
        istringstream stream(line);
        vector<int> patchLine;
        while(getline(stream, token, ',')) {
            patchLine.push_back(stoi(token));
        }

        patchIndexes.push_back(patchLine);
    }

    //Para a linha com o número de pontos (é ignorada)
    getline(infile, line);

    //Obter cada um dos pontos de controlo
    while(getline(infile, line)){
        istringstream stream(line);
        while(getline(stream, token, ',')){
            bezierPoints.push_back(stof(token));
        }

    }
    infile.close();
    return patchNo;

}

float bezierPoint(float u, float v, float p[4][4]){
    float u3 = pow(u,3);
    float u2 = pow(u, 2);
    float u_vector[4] = {u3, u2, u, 1};
    float m_matrix[4][4] = {{-1,3,-3,1}, {3, -6, 3, 0}, {-3,3,0,0}, {1,0,0,0}};
    float v3 = pow(v,3);
    float v2 = pow(v,2);
    float v_vector[4] = {v3, v2, v, 1};

    float aux1[4] = {0,0,0,0};
    float aux2[4] = {0,0,0,0};
    float aux3[4] = {0,0,0,0};
    float final = 0;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux1[i] += u_vector[j] * m_matrix[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux2[i] += aux1[j] * p[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux3[i] += aux2[j] * m_matrix[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        final += aux3[i] * v_vector[i];
    }

    return final;
}

//Guarda em r o produto externo entre a e b
void crossProduct(float *a, float *b, float *r){

    r[0] = a[1]*b[2] - a[2]*b[1];
    r[1] = a[2]*b[0] - a[0]*b[2];
    r[2] = a[0]*b[1] - a[1]*b[0];
}

//Para normalizar um vetor
void norm(float *r){
    float m = sqrt(pow(r[0], 2) + pow(r[1], 2) + pow(r[2], 2));

    r[0] /= m;
    r[1] /= m;
    r[2] /= m;
}

float uDeriv(float u, float v, float p[4][4]){
    float u3 = 3.0 * pow(u,2);
    float u2 = 2.0 * u;
    float u_vector[4] = {u3, u2, 1, 0};


    float m_matrix[4][4] = {{-1,3,-3,1}, {3, -6, 3, 0}, {-3,3,0,0}, {1,0,0,0}};


    float v3 = pow(v,3);
    float v2 = pow(v,2);
    float v_vector[4] = {v3, v2, v, 1};

    float aux1[4] = {0,0,0,0};
    float aux2[4] = {0,0,0,0};
    float aux3[4] = {0,0,0,0};
    float final = 0;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux1[i] += u_vector[j] * m_matrix[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux2[i] += aux1[j] * p[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux3[i] += aux2[j] * m_matrix[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        final += aux3[i] * v_vector[i];
    }

    return final;
}

float vDeriv(float u, float v, float p[4][4]){
    float u3 = pow(u,3);
    float u2 = pow(u, 2);
    float u_vector[4] = {u3, u2, u, 1};


    float m_matrix[4][4] = {{-1,3,-3,1}, {3, -6, 3, 0}, {-3,3,0,0}, {1,0,0,0}};


    float v3 = 3.0 * pow(v,2);
    float v2 = 2.0 * v;
    float v_vector[4] = {v3, v2, 1, 0};

    float aux1[4] = {0,0,0,0};
    float aux2[4] = {0,0,0,0};
    float aux3[4] = {0,0,0,0};
    float final = 0;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux1[i] += u_vector[j] * m_matrix[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux2[i] += aux1[j] * p[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            aux3[i] += aux2[j] * m_matrix[j][i];
        }
    }
    for(int i = 0; i < 4; i++){
        final += aux3[i] * v_vector[i];
    }

    return final;
}

void writePoint(Point p, ofstream &out_file){
    string phrase = to_string(p.x) + " "
            + to_string(p.y) + " "
            + to_string(p.z) +
            "\n";
    out_file.write(phrase.c_str(), phrase.size() + 1);
}

//Na escrita de um ponto, temos de passar a ter em conta as coordenadas das texturas e as normais para a iluminação
void writeTextAndLight(Point p, ofstream &out_file){
    string phrase = to_string(p.nx) + " "
                    + to_string(p.ny) + " "
                    + to_string(p.nz) + " "
                    + to_string(p.tx) + " "
                    + to_string(p.ty) + "\n";
    out_file.write(phrase.c_str(), phrase.size() + 1);
}

void drawSphere(float radius, float slices, float stacks, ofstream& ou_file_lt, ofstream& out_file_lt) {
    float alpha;
    float aStep = (2*M_PI)/slices;
    float beta;
    float bStep = M_PI/stacks;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4,  z4, h, texX, texZ, texXn, texZn;

    Point pA, pB, pC, pD;

    for(float b=-(stacks/2); b<(stacks/2); b++, h++) {
        beta=(b*bStep);

        for(float a=0; a<slices; a++) {
            alpha=(aStep*a);

            texX=(a/slices);
            texZ=(h/stacks);
            texXn=((a+1)/slices);
            texZn=((h+1)/stacks);

            x1 = radius*cos(beta)*sin(alpha);
            y1 = radius*sin(beta);
            z1 = radius*cos(beta)*cos(alpha);

            x2 = radius*cos(beta)*sin(aStep*(a+1));
            y2 = radius*sin(beta);
            z2 = radius*cos(beta)*cos(aStep*(a+1));

            x3 = radius*cos((b+1)*bStep)*sin(aStep*(a+1));
            y3 = radius*sin((b+1)*bStep);
            z3 = radius*cos((b+1)*bStep)*cos(aStep*(a+1));

            x4 = radius*cos((b+1)*bStep)*sin(alpha);
            y4 = radius*sin((b+1)*bStep);
            z4 = radius*cos((b+1)*bStep)*cos(alpha);

            pA = {x1, y1, z1, (x1/radius), (y1/radius), (z1/radius), texX, texZ};
            pB = {x2, y2, z2, (x2/radius), (y1/radius), (z2/radius), texXn, texZ};
            pC = {x3, y3, z3, (x3/radius), (y3/radius), (z3/radius), texXn, texZn};
            pD = {x4, y4, z4, (x4/radius), (y3/radius), (z4/radius), texX, texZn};

            writePoint(pA, out_file_lt);
            writeTextAndLight(pA, out_file_lt);
            writePoint(pB, out_file_lt);
            writeTextAndLight(pB, out_file_lt);
            writePoint(pC, out_file_lt);
            writeTextAndLight(pC, out_file_lt);

            writePoint(pC, out_file_lt);
            writeTextAndLight(pC, out_file_lt);
            writePoint(pD, out_file_lt);
            writeTextAndLight(pD, out_file_lt);
            writePoint(pA, out_file_lt);
            writeTextAndLight(pA, out_file_lt);
        }
    }
}


void drawConeBase(float radius, int slices, ofstream& out_file, ofstream& out_file_tl) {
    float a, aNext;
    for(int i = 0; i < slices; i++){
        a     = (((float)i)/(float)slices)*(2.0*M_PI);
        aNext = (((float)i+1.0)/(float)slices)*(2.0*M_PI);

        Point pA, pB, pC;

        float tx = 0.5*cos(a)+0.5;
        float tz = 0.5*sin(a)+0.5;

        float txn = 0.5*cos(aNext)+0.5;
        float tzn = 0.5*sin(aNext)+0.5;

        pA = {radius * sin(a), 0.0, radius * cos(a), 0.0, -1.0, 0.0, tx, tz};
        pB = {radius * sin(aNext), 0.0, radius * cos(aNext), 0.0, -1.0, 0.0, txn, tzn};
        pC = {0.0,0.0,0.0, 0.0, -1.0, 0.0, 0.0, 0.0};

        writePoint(pC, out_file);
        writeTextAndLight(pC, out_file_tl);
        writePoint(pB, out_file);
        writeTextAndLight(pB, out_file_tl);
        writePoint(pA, out_file);
        writeTextAndLight(pA, out_file_tl);
    }
}

void drawConeSides(float radius, float height, int stacks, int slices, ofstream& out_file, ofstream& out_file_tl){
    float a, aNext;
    float currHeight = 0.0f;
    float currRadius = radius;
    float heightDiff = height/(float)stacks;
    float radiusDiff = radius/(float)stacks;

    //O componente y da normal é constante
    float ny = sin(atan( (float)radius / (float)height ));

    for(int i = 0; i < stacks; i++){
        for(int j = 0; j < slices; j++){
            a     = (((float)j)/(float)slices)*(2.0*M_PI);
            aNext = (((float)j+1.0)/(float)slices)*(2.0*M_PI);

            Point pA, pB, pC, pD;

            //+0.5 porque o centro do circulo da textura é em (0.5,0.5) na imagem.
            float tx1 = ((float)i/(float)stacks) * cos(a) + 0.5;
            float tz1 = ((float)i/(float)stacks) * sin(a) + 0.5;

            float tx2 = ((float)i/(float)stacks) * cos(aNext) + 0.5;
            float tz2 = ((float)i/(float)stacks) * sin(aNext) + 0.5;

            float tx3 = ((float)(i+1)/(float)stacks) * cos(a) + 0.5;
            float tz3 = ((float)(i+1)/(float)stacks) * sin(a) + 0.5;

            float tx4 = ((float)(i+1)/(float)stacks) * cos(aNext) + 0.5;
            float tz4 = ((float)(i+1)/(float)stacks) * sin(aNext) + 0.5;

            pA = {(currRadius - radiusDiff) * sin(a), currHeight + heightDiff, (currRadius - radiusDiff) * cos(a), sin(a), ny, cos(a), tx1, tz1};
            pB = {(currRadius - radiusDiff) * sin(aNext), currHeight + heightDiff, (currRadius - radiusDiff) * cos(aNext), sin(aNext), ny, cos(aNext), tx2, tz2};
            pC = {currRadius * sin(a), currHeight, currRadius * cos(a), sin(a), ny ,cos(a), tx3, tz3};
            pD = {currRadius * sin(aNext), currHeight,currRadius * cos(aNext), sin(aNext), ny, cos(aNext), tx4, tz4};

            writePoint(pA, out_file);
            writeTextAndLight(pA, out_file_tl);
            writePoint(pD, out_file);
            writeTextAndLight(pD, out_file_tl);
            writePoint(pB, out_file);
            writeTextAndLight(pB, out_file_tl);

            writePoint(pA, out_file);
            writeTextAndLight(pA, out_file_tl);
            writePoint(pC, out_file);
            writeTextAndLight(pC, out_file_tl);
            writePoint(pD, out_file);
            writeTextAndLight(pD, out_file_tl);
        }
        currRadius -= radiusDiff;
        currHeight += heightDiff;
    }
}

void drawCone(float radius, float height, int stacks, int slices, ofstream& out_file, ofstream& out_file_tl) {
    drawConeBase(radius, slices, out_file, out_file_tl);
    drawConeSides(radius, height, stacks, slices, out_file, out_file_tl);
}

void drawPlane(float width, ofstream& out_file, ofstream& out_file_tl) {
    Point pA, pB, pC, pD;

    float n = width/2;

    pA = { n, 0,  -n, 0, 1, 0, 0, 1};
    pB = {-n, 0,  -n, 0, 1, 0, 0 ,0};
    pC = {-n, 0,  n, 0, 1, 0, 1, 0};
    pD = { n, 0,  n, 0, 1, 0, 1, 1};

    //Up
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);

    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    
    //Down
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
}

//De cada vez que escrevemos uma face, temos de reescrever os vetores normais.
void drawOuterBox(float xlen, float ylen, float zlen, ofstream& out_file, ofstream& out_file_tl) {
    Point pA, pB, pC, pD, pE, pF, pG, pH;


    // Base
    pA = {xlen, 0    , 0, 0 ,-1, 0, 1,1};
    pB = {0   , 0    , 0, 0, -1, 0, 0, 1};
    pC = {0   , 0    , zlen, 0, -1, 0, 0, 0};
    pD = {xlen, 0    , zlen, 0, -1, 0, 1, 0};


    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);

    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    // Base Dentro

    pA = {xlen, 0    , 0, 0 ,1, 0, 1,1};
    pB = {0   , 0    , 0, 0, 1, 0, 0, 1};
    pC = {0   , 0    , zlen, 0, 1, 0, 0, 0};
    pD = {xlen, 0    , zlen, 0, 1, 0, 1, 0};

    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);

    // Topo
    pE = {xlen, ylen , 0, 0, 1, 0, 1, 1};
    pF = {0   , ylen , 0, 0, 1, 0, 0, 1};
    pG = {0   , ylen , zlen, 0, 1, 0 , 0 ,0};
    pH = {xlen, ylen , zlen, 0, 1, 0, 1, 0};

    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pE, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);

    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);
    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);

    // Topo Dentro
    pE = {xlen, ylen , 0, 0, -1, 0, 1, 1};
    pF = {0   , ylen , 0, 0, -1, 0, 0, 1};
    pG = {0   , ylen , zlen, 0, -1, 0 , 0 ,0};
    pH = {xlen, ylen , zlen, 0, -1, 0, 1, 0};

    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);
    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);
    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);

    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);

    //Frente
    pG = {0   , ylen , zlen, 0, 0, 1 , 1 ,0};
    pH = {xlen, ylen , zlen, 0, 0, 1, 1, 1};
    pC = {0   , 0    , zlen, 0, 0, 1, 0, 0};
    pD = {xlen, 0    , zlen, 0, 0, 1, 1, 0};

    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);

    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    //Frente Dentro
    pG = {0   , ylen , zlen, 0, 0, -1 , 1 ,1};
    pH = {xlen, ylen , zlen, 0, 0, -1, 1, 0};
    pC = {0   , 0    , zlen, 0, 0, -1, 0, 1};
    pD = {xlen, 0    , zlen, 0, 0, -1, 0, 0};

    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);

    //Costas
    pA = {xlen, 0    , 0, 0 ,0, -1, 0,0};
    pB = {0   , 0    , 0, 0, 0, -1, 1, 0};
    pE = {xlen, ylen , 0, 0, 0, -1, 0, 1};
    pF = {0   , ylen , 0, 0, 0, -1, 1, 1};

    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);
    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);

    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);

    //Costas Dentro
    pA = {xlen, 0    , 0, 0 ,0, 1, 1,0};
    pB = {0   , 0    , 0, 0, 0, 1, 0, 0};
    pE = {xlen, ylen , 0, 0, 0, 1, 1, 1};
    pF = {0   , ylen , 0, 0, 0, 1, 0, 1};

    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);
    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);

    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);

    //Direita
    pA = {xlen, 0    , 0, 1 ,0, 0, 0,1};
    pE = {xlen, ylen , 0, 1, 0, 0, 1, 1};
    pH = {xlen, ylen , zlen, 1, 0, 0, 0, 1};
    pD = {xlen, 0    , zlen, 1, 0, 0, 0, 0};

    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);

    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);
    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    //Direita Dentro
    pA = {xlen, 0    , 0, -1 ,0, 0, 0, 0};
    pE = {xlen, ylen , 0, -1, 0, 0, 0, 1};
    pH = {xlen, ylen , zlen, -1, 0, 0, 1, 1};
    pD = {xlen, 0    , zlen, -1, 0, 0, 1, 0};

    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);
    writePoint(pA, out_file);
    writeTextAndLight(pA, out_file_tl);
    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);

    writePoint(pD, out_file);
    writeTextAndLight(pD, out_file_tl);
    writePoint(pH, out_file);
    writeTextAndLight(pH, out_file_tl);
    writePoint(pE, out_file);
    writeTextAndLight(pE, out_file_tl);

    //Esquerda
    pG = {0   , ylen , zlen, -1, 0, 0 , 1 ,1};
    pB = {0   , 0    , 0, -1, 0, 0, 0, 0};
    pF = {0   , ylen , 0, -1, 0, 0, 0, 1};
    pC = {0   , 0    , zlen, -1, 0, 0, 1, 0};

    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);
    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);

    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);

    //Esquerda Dentro
    pG = {0   , ylen , zlen, 1, 0, 0 , 0 ,1};
    pB = {0   , 0    , 0, 1, 0, 0, 1, 0};
    pF = {0   , ylen , 0, 1, 0, 0, 1, 1};
    pC = {0   , 0    , zlen, 1, 0, 0, 0, 0};

    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
    writePoint(pF, out_file);
    writeTextAndLight(pF, out_file_tl);
    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);

    writePoint(pB, out_file);
    writeTextAndLight(pB, out_file_tl);
    writePoint(pC, out_file);
    writeTextAndLight(pC, out_file_tl);
    writePoint(pG, out_file);
    writeTextAndLight(pG, out_file_tl);
}

void drawInnerBox(float xlen, float ylen, float zlen, int div, ofstream& out_file, ofstream& out_file_tl) {
    float dist = xlen/div;
    Point pA, pB, pC, pD;
    for(int i = 1; i < div; i++){

        //Frente
        pA = {dist * i, ylen, zlen, -1, 0, 0, 1, 1};
        pB = {dist * i, ylen, 0, -1, 0, 0, 0, 1};
        pC = {dist * i, 0   , 0, -1, 0, 0, 0, 0};
        pD = {dist * i, 0   , zlen, -1, 0, 0, 1, 0};

        writePoint(pD, out_file);
        writeTextAndLight(pD, out_file_tl);
        writePoint(pA, out_file);
        writeTextAndLight(pA, out_file_tl);
        writePoint(pB, out_file);
        writeTextAndLight(pB, out_file_tl);

        writePoint(pB, out_file);
        writeTextAndLight(pB, out_file_tl);
        writePoint(pC, out_file);
        writeTextAndLight(pC, out_file_tl);
        writePoint(pD, out_file);
        writeTextAndLight(pD, out_file_tl);

        //Trás
        pA = {dist * i, ylen, zlen, 1, 0, 0, 0, 1};
        pB = {dist * i, ylen, 0, 1, 0, 0, 1, 1};
        pC = {dist * i, 0   , 0, 1, 0, 0, 1, 0};
        pD = {dist * i, 0   , zlen, 1, 0, 0, 0, 0};

        writePoint(pB, out_file);
        writeTextAndLight(pB, out_file_tl);
        writePoint(pA, out_file);
        writeTextAndLight(pA, out_file_tl);
        writePoint(pD, out_file);
        writeTextAndLight(pD, out_file_tl);

        writePoint(pD, out_file);
        writeTextAndLight(pD, out_file_tl);
        writePoint(pC, out_file);
        writeTextAndLight(pC, out_file_tl);
        writePoint(pB, out_file);
        writeTextAndLight(pB, out_file_tl);
    }
}

void drawBox(float xlen, float ylen, float zlen, int div, ofstream& out_file, ofstream& out_file_tl) {
    drawOuterBox(xlen, ylen, zlen, out_file, out_file_tl);
    drawInnerBox(xlen, ylen, zlen, div, out_file, out_file_tl);
}

void drawBezierPatch(string in_file, int tesselation, ofstream& out_file, ofstream& out_file_tl){

    float step = 1.0f/(float)tesselation;
    float x1, y1, z1;
    int patches = readBezierPatchFile(in_file);
    float x[4][4], y[4][4], z[4][4];
    float u, un, v, vn;

    for(int i = 0; i < patches; i++){
        for(int j = 0; j < 4; j++){
            for(int k = 0; k < 4; k++){
                int id = patchIndexes[i][(j * 4) + k];

                x[j][k] = bezierPoints[id * 3];
                y[j][k] = bezierPoints[(id * 3) + 1];
                z[j][k] = bezierPoints[(id * 3) + 2];
            }
        }
        for(int ul = 0; ul < tesselation; ul++) {
            u = (float) ul * step;
            un = (float) (ul + 1) * step;
            for (int vl = 0; vl < tesselation; vl++) {
                v = (float) vl * step;
                vn = (float) (vl + 1) * step;

                Point pA, pB, pC, pD;

                float *cp = (float*)malloc(sizeof(float)*3);
                float *ud = (float*)malloc(sizeof(float)*3);
                float *vd = (float*)malloc(sizeof(float)*3);


                //A
                ud[0] = uDeriv(u, v, x);
                ud[1] = uDeriv(u, v, y);
                ud[2] = uDeriv(u, v, z);
                vd[0] = vDeriv(u, v, x);
                vd[1] = vDeriv(u, v, y);
                vd[2] = vDeriv(u, v, z);
                norm(ud);
                norm(vd);
                crossProduct(ud, vd, cp);
                norm(cp);

                pA.x = bezierPoint(u, v, x);
                pA.y = bezierPoint(u, v, y);
                pA.z = bezierPoint(u, v, z);
                pA.nx = cp[0]*-1;
                pA.ny = cp[1]*-1;
                pA.nz = cp[2]*-1;
                pA.tx = u;
                pA.ty = v;

                //B
                ud[0] = uDeriv(un, v, x);
                ud[1] = uDeriv(un, v, y);
                ud[2] = uDeriv(un, v, z);
                vd[0] = vDeriv(un, v, x);
                vd[1] = vDeriv(un, v, y);
                vd[2] = vDeriv(un, v, z);
                norm(ud);
                norm(vd);
                crossProduct(ud, vd, cp);
                norm(cp);

                pB.x = bezierPoint(un, v, x);
                pB.y = bezierPoint(un, v, y);
                pB.z = bezierPoint(un, v, z);
                pB.nx = cp[0]*-1;
                pB.ny = cp[1]*-1;
                pB.nz = cp[2]*-1;
                pB.tx = un;
                pB.ty = v;

                //C
                ud[0] = uDeriv(u, vn, x);
                ud[1] = uDeriv(u, vn, y);
                ud[2] = uDeriv(u, vn, z);
                vd[0] = vDeriv(u, vn, x);
                vd[1] = vDeriv(u, vn, y);
                vd[2] = vDeriv(u, vn, z);
                norm(ud);
                norm(vd);
                crossProduct(ud, vd, cp);
                norm(cp);

                pC.x = bezierPoint(u, vn, x);
                pC.y = bezierPoint(u, vn, y);
                pC.z = bezierPoint(u, vn, z);
                pC.nx = cp[0]*-1;
                pC.ny = cp[1]*-1;
                pC.nz = cp[2]*-1;
                pC.tx = u;
                pC.ty = vn;

                //D
                ud[0] = uDeriv(un, vn, x);
                ud[1] = uDeriv(un, vn, y);
                ud[2] = uDeriv(un, vn, z);
                vd[0] = vDeriv(un, vn, x);
                vd[1] = vDeriv(un, vn, y);
                vd[2] = vDeriv(un, vn, z);
                norm(ud);
                norm(vd);
                crossProduct(ud, vd, cp);
                norm(cp);

                pD.x = bezierPoint(un, vn, x);
                pD.y = bezierPoint(un, vn, y);
                pD.z = bezierPoint(un, vn, z);
                pD.nx = cp[0]*-1;
                pD.ny = cp[1]*-1;
                pD.nz = cp[2]*-1;
                pD.tx = un;
                pD.ty = vn;

                writePoint(pA, out_file);
                writeTextAndLight(pA, out_file_tl);
                writePoint(pC, out_file);
                writeTextAndLight(pC, out_file_tl);
                writePoint(pB, out_file);
                writeTextAndLight(pB, out_file_tl);

                writePoint(pB, out_file);
                writeTextAndLight(pB, out_file_tl);
                writePoint(pC, out_file);
                writeTextAndLight(pC, out_file_tl);
                writePoint(pD, out_file);
                writeTextAndLight(pD, out_file_tl);
            }
        }
    }
}

int main(int argc, char** argv) {
    int val = 0;
    string shape = argv[1];
    ofstream output_file, output_file_tl;

    if((shape.compare("sphere")) == 0)  val = 1;
    if((shape.compare("cone")) == 0)    val = 2;
    if((shape.compare("box")) == 0)     val = 3;
    if((shape.compare("plane")) == 0)   val = 4;
    if((shape.compare("bezier")) == 0)  val = 5;

    switch(val){
        //Sphere
        case 1 : {
            if(!argv[5]){
                cout << "Not enough arguments!" << endl;
            }
            else{
                output_file.open(argv[5], ios::out | ios::app | ios::binary);
                output_file_tl.open(strcat(argv[5], ".lat"), ios::out | ios::app | ios::binary);

                drawSphere(atof(argv[2]), atoi(argv[4]), atoi(argv[3]), output_file, output_file_tl);

                output_file.close();
                output_file_tl.close();
            }
            break;
        }
        //Cone
        case 2 : {
            if(!argv[6]){
                cout << "Not enough arguments!" << endl;
            }
            else {
                output_file.open(argv[6], ios::out | ios::app | ios::binary);
                output_file_tl.open(strcat(argv[6], ".lat"), ios::out | ios::app | ios::binary);


                drawCone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), output_file, output_file_tl);

                output_file.close();
                output_file_tl.close();
            }
            break;
        }
        //Box
        case 3 : {
            if(!argv[5]){
                cout << "Not enough arguments!" << endl;
            }
            else {
                if(!argv[6]){
                    output_file.open(argv[5], ios::out | ios::app | ios::binary);
                    output_file_tl.open(strcat(argv[5], ".lat"), ios::out | ios::app | ios::binary);

                    drawOuterBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), output_file, output_file_tl);

                    output_file.close();
                } else{
                    output_file.open(argv[6], ios::out | ios::app | ios::binary);
                    output_file_tl.open(strcat(argv[6], ".lat"), ios::out | ios::app | ios::binary);

                    drawBox(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), output_file, output_file_tl);

                    output_file.close();
                    output_file_tl.close();
                }                
            }
            break;
        }
        //Plane
        case 4 : {
            if(!argv[2]){
                cout << "Not enough arguments!" << endl;
            }
            else{
                output_file.open(argv[3], ios::out | ios::app | ios::binary);
                output_file_tl.open(strcat(argv[3],".lat"), ios::out | ios::app | ios::binary);

                drawPlane(atof(argv[2]), output_file, output_file_tl);

                output_file.close();
                output_file_tl.close();
            }
            break;
        }
        //Bezier Patch
        case 5 : {
            if(!argv[4]){
                cout << "Not enough arguments!" << endl;
            }
            else{
                output_file.open(argv[4], ios::out | ios::app | ios::binary);
                output_file_tl.open(strcat(argv[4], ".lat"), ios::out | ios::app | ios::binary);

                string in_file = argv[2];

                drawBezierPatch(in_file, atoi(argv[3]), output_file, output_file_tl);

                output_file.close();
                output_file_tl.close();
            }
            break;
        }
        //Otherwise
        default : {
            cout << "Invalid arguments" << endl;
        }
    }

    output_file.close();

    return 0;
}
