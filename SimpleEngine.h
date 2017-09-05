// Copyright (c) 2011 Kha Do
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef SIMPLEENGINE_H_
#define SIMPLEENGINE_H_

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

#include "ObjectNode.h"
#include "EngineMath.h"

#define NUMBRUSHES 7
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Shader files we're going to use
#define VSFILENAME "normalVertShader.glsl"
#define GSFILENAME "normalGeomShader.glsl"
#define PSFILENAME "normalFragShader.glsl"
#define QVSFILENAME "quadVertShader.glsl"
#define QPSFILENAME "quadFragShader.glsl"
#define IMPVSFILENAME "ImpVertShader.glsl"
#define IMPPSFILENAME "ImpFragShader.glsl"

// Model files to use
#define SPACESHIP "space_frigate_0.obj"
#define ROUNDABOUT "roundabout.obj"
#define LAMP "STLamp.obj"
#define CUBE "cube.obj"
#define COBRA "kitchen.obj"

// Static methods for glut callbacks
void handleDrawScreen();
void handleMenu(int);
void handleMotion(int, int);
void handleMouse(int, int, int, int);
void handleTimer();
void handleSpecialKeys(int key, int x, int y);
void handleSpecialUpKeys(int key, int x, int y);
void handleKeys(unsigned char key, int x, int y);
void handleUpKeys(unsigned char key, int x, int y);

int readFileLength(const char *filename);
void readTextFile(const char *filename, GLchar const **source);

// Camera bias
const GLfloat bias[16] = {
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
};

// Lights
GLfloat lightPos[NUMSMAPS][4] = {
    {100.0 + 50, 100.0, 0.0, 1.0},
    {0, 200, 0, 1.0},
};
    
GLfloat lightDir[NUMSMAPS][3] = {
    {0.0 + 50, 0.0, 0.0},
       {0.0, 0.0, 0.0},
};
    
GLfloat lightUp[NUMSMAPS][3] = {
    {-0.5, 0.5, 0},
    {0, 0, 1},
};
    
GLfloat lightClip[NUMSMAPS][2] = {
    {25, 300},
    {75, 300},
};


class SimpleEngine {

    public:
        bool drawPolys;

        // Constructor
        SimpleEngine();

        void drawScreen();
        void timer();
        void specialKeys(int key, int x, int y);
        void specialUpKeys(int key, int x, int y);
        void keys(unsigned char key, int x, int y);
        void upKeys(unsigned char key, int x, int y);
        void setupFramebuffers();

    private:
        ObjectNode *head;
        bool leftKey, rightKey, upKey, downKey, zKey, xKey;
        long lastTime, eTime;
        int whichTexture;
        int whichBrush;
        GLuint shadowFrameBuffer, refFrameBuffer, impFrameBuffer;
        GLfloat camRotation;
        GLuint shadowTextures[NUMSMAPS];
        GLuint refColor, refNormal, refSize, refDepth, impColor;
        GLint irotID;
        GLuint quadProg, impProg;
        GLuint brush[NUMBRUSHES];
        std::map < std::string, GLuint > vbos;
        std::map < std::string, GLuint > ibos;
        GLuint shaderProgram;
        GLuint shadowProgram;
        GLuint shadowMap;
        GLfloat camPos[3];
        GLfloat mmatrix[16];
        GLfloat invcammat[16];

        void draw(ObjectNode * node);
        void simpleDraw(ObjectNode * node);
        void drawPoints(ObjectNode * node);
        GLuint loadShader(const char *filename, GLenum shaderType);
        GLuint loadBrush(const char * filename);
        void setupShaders();
        void loadData();
        void checkObjectCollisions(ObjectNode * o);
        void makeShadowMaps();
        void makeShadowCube();
        void makeRefImages();
        void makeImpImage();


};

#endif
