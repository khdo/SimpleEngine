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

#ifndef ENGINEMATH_H_
#define ENGINEMATH_H_

#ifdef _M_IX86
#include <windows.h>
#define FTIME _ftime_s
#define TIMEB _timeb
#define SNPRINTF _snprintf
#else
#define FTIME ftime
#define TIMEB timeb
#define SNPRINTF snprintf
#endif

// OpenGL headers of MacOS
#ifdef __MACH__
#include <GL/glew.h>
#include <OpenGL/glu.h>
#include "AGL/agl.h"
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <cstdlib>
#include <GL/glut.h>
#endif

#include <list>
#include <map>
#include <math.h>
#include <IL/il.h>
#include <string.h>

#include "GLObjViewer/model_obj.h"

#define TIMESKEW 0.06
#define TIMESLICE 16.66
#define PI 3.14159
#define SMAP_WIDTH 1600
#define SMAP_HEIGHT 1200
#define CUBESIZE 1024
#define NUMSMAPS 2

#define BUFFER_OFFSET(bytes)    ((GLubyte *) NULL + (bytes))

static const GLfloat yellow[4] = { 0.5, 0.5, .1, 1 };
static const GLfloat blue[4] = { .2f, .2f, 1.0f, 1.0f };
static const GLfloat white[4] = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat black[4] = { 0, 0, 0, 1 };
static const GLfloat red[4] = { .7, 0.2, 0.2, 0.5 };
static const GLfloat gray[4] = { 0.82, 0.8, 0.75, 1 };
static const GLfloat darkgray[4] = { 0.5, 0.5, 0.5, 1 };
static const GLfloat green[4] = { 0, 1, 0, 1 };
static const GLfloat skyblue[4] = { 0.5, 0.75, 0.9, 1.0 };
static const GLfloat zeros[3] = { 0, 0, 0 };


void calcMxV(GLfloat m[16], const GLfloat v[3], GLfloat r[3]);
void transpose(GLfloat m1[16], GLfloat m2[16]);
void inverseMat(GLfloat m[16], GLfloat i[16]);
void multMat(GLfloat m1[16], GLfloat m2[16], GLfloat out[16]);
void loadIdentity(GLfloat m[16]);
void translate(GLfloat m[16], float x, float y, float z);
void scalem(GLfloat m[16], float s);
void rotatex(GLfloat m[16], float d);
void rotatey(GLfloat m[16], float d);
void rotatez(GLfloat m[16], float d);
float calcTriangleArea(const ModelOBJ::Vertex * a, const ModelOBJ::Vertex * b, const ModelOBJ::Vertex * c);
float distance(const float * a, const float * b);

#endif  // ENGINEMATH_H_
