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

#include "EngineMath.h"

// Multiply matrix m by vector v and store in vector r
void calcMxV(GLfloat m[16], const GLfloat v[3], GLfloat r[3]) {
    GLfloat t = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + m[15];

    r[0] = (v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + m[12]) / t;
    r[1] = (v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + m[13]) / t;
    r[2] = (v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + m[14]) / t;
}

// Transpose matrix m1 and store in matrix m2
void transpose(GLfloat m1[16], GLfloat m2[16]) {
    m2[0] = m1[0];
    m2[1] = m1[4];
    m2[2] = m1[8];
    m2[3] = m1[12];
    m2[4] = m1[1];
    m2[5] = m1[5];
    m2[6] = m1[9];
    m2[7] = m1[13];
    m2[8] = m1[2];
    m2[9] = m1[6];
    m2[10] = m1[10];
    m2[11] = m1[14];
    m2[12] = m1[3];
    m2[13] = m1[7];
    m2[14] = m1[11];
    m2[15] = m1[15];
}

// Multiply matrix m1 by matrix m2 and store in matrix out
void multMat(GLfloat m1[16], GLfloat m2[16], GLfloat out[16]) {
    out[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    out[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    out[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    out[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];

    out[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    out[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    out[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    out[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];

    out[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    out[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    out[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    out[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];

    out[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
    out[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
    out[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
    out[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

}

// Calculate the inverse of matrix m and store in i
void inverseMat(GLfloat m[16], GLfloat i[16]) {

    GLfloat d = m[12] * m[9]  * m[6]  * m[3] -
                m[8]  * m[13] * m[6]  * m[3] -
                m[12] * m[5]  * m[10] * m[3] +
                m[4]  * m[13] * m[10] * m[3] +
                m[8]  * m[5]  * m[14] * m[3] -
                m[4]  * m[9]  * m[14] * m[3] -
                m[12] * m[9]  * m[2]  * m[7] +
                m[8]  * m[13] * m[2]  * m[7] +
                m[12] * m[1]  * m[10] * m[7] -
                m[0]  * m[13] * m[10] * m[7] -
                m[8]  * m[1]  * m[14] * m[7] +
                m[0]  * m[9]  * m[14] * m[7] +
                m[12] * m[5]  * m[2]  * m[11] -
                m[4]  * m[13] * m[2]  * m[11] -
                m[12] * m[1]  * m[6]  * m[11] +
                m[0]  * m[13] * m[6]  * m[11] +
                m[4]  * m[1]  * m[14] * m[11] -
                m[0]  * m[5]  * m[14] * m[11] -
                m[8]  * m[5]  * m[2]  * m[15] +
                m[4]  * m[9]  * m[2]  * m[15] +
                m[8]  * m[1]  * m[6]  * m[15] -
                m[0]  * m[9]  * m[6]  * m[15] -
                m[4]  * m[1]  * m[10] * m[15] +
                m[0]  * m[5]  * m[10] * m[15];

    // If we can't compute inverse, just return
    // Need to replace this with SVD to try to approximate the inverse
    if (d == 0) {
        d=.000000001f;
    }

    i[0] = (-m[13] * m[10] * m[7] + m[9] * m[14] * m[7] + m[13] * m[6] * m[11]
            - m[5] * m[14] * m[11] - m[9] * m[6] * m[15] + m[5] * m[10] * m[15]) / d;
    i[4] = (m[12] * m[10] * m[7] - m[8] * m[14] * m[7] - m[12] * m[6] * m[11]
            + m[4] * m[14] * m[11] + m[8] * m[6] * m[15] - m[4] * m[10] * m[15]) / d;
    i[8] = (-m[12] * m[9] * m[7] + m[8] * m[13] * m[7] + m[12] * m[5] * m[11]
            - m[4] * m[13] * m[11] - m[8] * m[5] * m[15] + m[4] * m[9] * m[15]) / d;
    i[12] = (m[12] * m[9] * m[6] - m[8] * m[13] * m[6] - m[12] * m[5] * m[10]
            + m[4] * m[13] * m[10] + m[8] * m[5] * m[14] - m[4] * m[9] * m[14]) / d;
    i[1] = (m[13] * m[10] * m[3] - m[9] * m[14] * m[3] - m[13] * m[2] * m[11]
           + m[1] * m[14] * m[11] + m[9] * m[2] * m[15] - m[1] * m[10] * m[15]) / d;
    i[5] = (-m[12] * m[10] * m[3] + m[8] * m[14] * m[3] + m[12] * m[2] * m[11]
           - m[0] * m[14] * m[11] - m[8] * m[2] * m[15] + m[0] * m[10] * m[15]) / d;
    i[9] = (m[12] * m[9] * m[3] - m[8] * m[13] * m[3] - m[12] * m[1] * m[11]
           + m[0] * m[13] * m[11] + m[8] * m[1] * m[15] - m[0] * m[9] * m[15]) / d;
    i[13] = (-m[12] * m[9] * m[2] + m[8] * m[13] * m[2] + m[12] * m[1] * m[10]
            - m[0] * m[13] * m[10] - m[8] * m[1] * m[14] + m[0] * m[9] * m[14]) / d;
    i[2] = (-m[13] * m[6] * m[3] + m[5] * m[14] * m[3] + m[13] * m[2] * m[7]
            - m[1] * m[14] * m[7] - m[5] * m[2] * m[15] + m[1] * m[6] * m[15]) / d;
    i[6] = (m[12] * m[6] * m[3] - m[4] * m[14] * m[3] - m[12] * m[2] * m[7]
            + m[0] * m[14] * m[7] + m[4] * m[2] * m[15] - m[0] * m[6] * m[15]) / d;
    i[10] = (-m[12] * m[5] * m[3] + m[4] * m[13] * m[3] + m[12] * m[1] * m[7]
            - m[0] * m[13] * m[7] - m[4] * m[1] * m[15] + m[0] * m[5] * m[15]) / d;
    i[14] = (m[12] * m[5] * m[2] - m[4] * m[13] * m[2] - m[12] * m[1] * m[6]
            + m[0] * m[13] * m[6] + m[4] * m[1] * m[14] - m[0] * m[5] * m[14]) / d;
    i[3] = (m[9] * m[6] * m[3] - m[5] * m[10] * m[3] - m[9] * m[2] * m[7]
            + m[1] * m[10] * m[7] + m[5] * m[2] * m[11] - m[1] * m[6] * m[11]) / d;
    i[7] = (-m[8] * m[6] * m[3] + m[4] * m[10] * m[3] + m[8] * m[2] * m[7]
            - m[0] * m[10] * m[7] - m[4] * m[2] * m[11] + m[0] * m[6] * m[11]) / d;
    i[11] = (m[8] * m[5] * m[3] - m[4] * m[9] * m[3] - m[8] * m[1] * m[7]
            + m[0] * m[9] * m[7] + m[4] * m[1] * m[11] - m[0] * m[5] * m[11]) / d;
    i[15] = (-m[8] * m[5] * m[2] + m[4] * m[9] * m[2] + m[8] * m[1] * m[6]
            - m[0] * m[9] * m[6] - m[4] * m[1] * m[10] + m[0] * m[5] * m[10]) / d;
}

// Load matrix m with the identity matrix
void loadIdentity(GLfloat m[16]) {
    m[0] = 1;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;

    m[4] = 0;
    m[5] = 1;
    m[6] = 0;
    m[7] = 0;

    m[8] = 0;
    m[9] = 0;
    m[10] = 1;
    m[11] = 0;

    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;

}

// Translate matrix m by values x, y, and z
void translate(GLfloat m[16], float x, float y, float z) {
    GLfloat temp[16];
    memcpy(temp, m, sizeof(GLfloat) * 16);
    GLfloat temp2[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
    };
    multMat(temp2, temp, m);
}

// Scale matrix m by value s
void scalem(GLfloat m[16], float s) {
    GLfloat temp[16];
    memcpy(temp, m, sizeof(GLfloat) * 16);
    GLfloat temp2[16] = {
            s, 0, 0, 0,
            0, s, 0, 0,
            0, 0, s, 0,
            0, 0, 0, 1
    };
    multMat(temp2, temp, m);
}

// Rotate matrix m along the x axis by d degrees
void rotatex(GLfloat m[16], float d) {
    GLfloat temp[16];
    memcpy(temp, m, sizeof(GLfloat) * 16);
    GLfloat temp2[16] = {
            1, 0, 0, 0,
            0, cosf(2 * PI * d / 360.0), sinf(2 * PI * d / 360.0), 0,
            0, -sinf(2 * PI * d / 360.0), cosf(2 * PI * d / 360.0), 0,
            0, 0, 0, 1
    };
    multMat(temp2, temp, m);
}

// Rotate matrix m along the y axis by d degrees
void rotatey(GLfloat m[16], float d) {
    GLfloat temp[16];
    memcpy(temp, m, sizeof(GLfloat) * 16);
    GLfloat temp2[16] = {
            cosf(2 * PI * d / 360.0), 0, -sinf(2 * PI * d / 360.0), 0,
            0, 1, 0, 0,
            sinf(2 * PI * d / 360.0), 0, cosf(2 * PI * d / 360.0), 0,
            0, 0, 0, 1
    };
    multMat(temp2, temp, m);
}

// Rotate matrix m along the z axis by d degrees
void rotatez(GLfloat m[16], float d) {
    GLfloat temp[16];
    memcpy(temp, m, sizeof(GLfloat) * 16);
    GLfloat temp2[16] = {
            cosf(2 * PI * d / 360.0), sinf(2 * PI * d / 360.0), 0, 0,
            -sinf(2 * PI * d / 360.0), cosf(2 * PI * d / 360.0), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
    };
    multMat(temp2, temp, m);
}


// Calculate the area of a triangle using Heron's formula
float calcTriangleArea(const ModelOBJ::Vertex * a, const ModelOBJ::Vertex * b, const ModelOBJ::Vertex * c) {
    float ab = distance(a->position, b->position);
    float bc = distance(b->position, c->position);
    float ac = distance(a->position, c->position);
    float s = (ab + bc + ac) / 2;

    return sqrt(s * (s - ab) * (s - bc) * (s - ac));
}

// Calculate the distance between two points
float distance(const float * a, const float * b) {
    return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
}
