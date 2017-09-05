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

#include "ObjectNode.h"

ObjectNode::ObjectNode(void) :
    parent(NULL),
    model(NULL),
    scale(1),
    position{ 0, 0, 0 },
    rotation{ 0, 0, 0 },
    velocity{ 0, 0, 0 },
    acceleration{ 0, 0, 0 },
    angularVelocity{ 0, 0, 0 },
    angularAccel{ 0, 0, 0 },
    collision(false),
    boxMin{ 0, 0, 0 },
    boxMax{ 0, 0, 0 },
    mmatrix{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }
{
    children.empty();
}

ObjectNode::ObjectNode(const char *filename) :
    parent(NULL),
    model(NULL),
    scale(1),
    position{ 0, 0, 0 },
    rotation{ 0, 0, 0 },
    velocity{ 0, 0, 0 },
    acceleration{ 0, 0, 0 },
    angularVelocity{ 0, 0, 0 },
    angularAccel{ 0, 0, 0 },
    collision(false),
    boxMin{ 0, 0, 0 },
    boxMax{ 0, 0, 0 },
    mmatrix{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }
{
    children.empty();
    model = new ModelOBJ();
    model->import(filename);
    loadTextures(model);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
            model->getVertexSize() * model->getNumberOfVertices(),
            model->getVertexBuffer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            model->getIndexSize() * model->getNumberOfIndices(),
            model->getIndexBuffer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Calculate the hit box for the object
void ObjectNode::calcRad() {
    GLfloat r[3];

    loadIdentity(mmatrix);
    rotatez(mmatrix, rotation[2]);
    rotatey(mmatrix, rotation[1]);
    rotatex(mmatrix, rotation[0]);
    scalem(mmatrix, scale);
    translate(mmatrix, position[0], position[1], position[2]);
    GLfloat temp[16];
    memcpy(temp, mmatrix, sizeof(GLfloat) * 16);
    if (parent != NULL)
        multMat(parent->mmatrix, temp, mmatrix);

    if (!structural) {
        calcMxV(mmatrix, model->getVertex(0).position, r);
        boxMin[0] = r[0];
        boxMax[0] = r[0];
        boxMin[1] = r[1];
        boxMax[1] = r[1];
        boxMin[2] = r[2];
        boxMax[2] = r[2];
        for (int i = 1; i < model->getNumberOfVertices(); i++) {
            calcMxV(mmatrix, model->getVertex(i).position, r);
            if (r[0] < boxMin[0])
                boxMin[0] = r[0];
            if (r[0] > boxMax[0])
                boxMax[0] = r[0];
            if (r[1] < boxMin[1])
                boxMin[1] = r[1];
            if (r[1] > boxMax[1])
                boxMax[1] = r[1];
            if (r[2] < boxMin[2])
                boxMin[2] = r[2];
            if (r[2] > boxMax[2])
                boxMax[2] = r[2];
        }
    }
}

// Update the object's position and rotation based on the elapsed time
void ObjectNode::update(long time) {
    double t = time * TIMESKEW;
    position[0] += velocity[0] * t;
    position[1] += velocity[1] * t;
    position[2] += velocity[2] * t;
    velocity[0] += acceleration[0] * t;
    velocity[1] += acceleration[1] * t;
    velocity[2] += acceleration[2] * t;
    rotation[0] += angularVelocity[0] * t;
    rotation[1] += angularVelocity[1] * t;
    rotation[2] += angularVelocity[2] * t;
    angularVelocity[0] += angularAccel[0] * t;
    angularVelocity[1] += angularAccel[1] * t;
    angularVelocity[2] += angularAccel[2] * t;
    collision = false;
    if (rotation[0] > 360)
        rotation[0] -= 360;
    if (rotation[1] > 360)
        rotation[1] -= 360;
    if (rotation[2] > 360)
        rotation[2] -= 360;
    calcRad();
    std::list < ObjectNode * >::iterator ci = children.begin();
    while (ci != children.end()) {
        (*ci)->update(time);
        ci++;
    }
}

// Clear the collion flag
void ObjectNode::clearCollision() {
    collision = false;
    std::list < ObjectNode * >::iterator ci = children.begin();
    while (ci != children.end()) {
        (*ci)->clearCollision();
        ci++;
    }
}

// Check for collision against a sphere with radius r, and position x, y, z
bool ObjectNode::doesCollide(GLfloat x, GLfloat y, GLfloat z, GLfloat rad) {
    bool retval = true;

    if (x - rad > boxMax[0]) {
        retval = false;
    } else if (x + rad < boxMin[0]) {
        retval = false;
    } else if (y - rad > boxMax[1]) {
        retval = false;
    } else if (y + rad < boxMin[1]) {
        retval = false;
    } else if (z - rad > boxMax[2]) {
        retval = false;
    } else if (z + rad < boxMin[2]) {
        retval = false;
    }

    std::list < ObjectNode * >::iterator ci = children.begin();
    while (ci != children.end()) {
        (*ci)->doesCollide(x, y, z, rad);
        ci++;
    }

    if (retval) {
        collision = true;
        return true;
    } else
        return false;
}

// Check for collision with other object o
bool ObjectNode::doesCollide(ObjectNode * o) {
    bool retval = true;

    if (o->structural || structural) {
        retval = false;
    } else if (o == this) {
        retval = false;
    } else if (o->boxMin[0] > boxMax[0]) {
        retval = false;
    } else if (o->boxMax[0] < boxMin[0]) {
        retval = false;
    } else if (o->boxMin[1] > boxMax[1]) {
        retval = false;
    } else if (o->boxMax[1] < boxMin[1]) {
        retval = false;
    } else if (o->boxMin[2] > boxMax[2]) {
        retval = false;
    } else if (o->boxMax[2] < boxMin[2]) {
        retval = false;
    }

    std::list < ObjectNode * >::iterator ci = children.begin();
    while (ci != children.end()) {
        (*ci)->doesCollide(o);
        ci++;
    }

    if (retval) {
        o->collision = true;
        collision = true;
        return true;
    } else {
        return false;
    }
}

// Calculate scale of object
void ObjectNode::calcScale() {
    if (parent == NULL)
        globalScale = scale;
    else {
        globalScale = parent->scale * scale;
    }
    generatePoints();
    std::list < ObjectNode * >::iterator ci = children.begin();
    while (ci != children.end()) {
        (*ci)->calcScale();
        ci++;
    }
}

// Generate a number of points for each triangle in the object
void ObjectNode::generatePoints() {
    totalNumPoints = 0;
    if (model == NULL)
        return;
    int numTriangles = model->getNumberOfTriangles();
    int * numPoints = new int[numTriangles];
    float area = 0;
    const int * indexes = model->getIndexBuffer();
    const ModelOBJ::Vertex * verts = model->getVertexBuffer();

    for (int i = 0; i < numTriangles; i++) {
        area = calcTriangleArea(&verts[indexes[3 * i]], &verts[indexes[3 * i + 1]], &verts[indexes[3 * i + 2]]);
        numPoints[i] = ceilf(area * globalScale * 150);
        totalNumPoints += numPoints[i];
    }

    GLfloat * points = new GLfloat[totalNumPoints * 3];
    long currentPoint = 0;
    for (int i = 0; i < numTriangles; i++) {
        for (int j = 0; j < numPoints[i]; j++) {
            float a = (float)rand() / RAND_MAX;
            float b = (float)rand() / RAND_MAX;
            float c;
            if (a + b > 1) {
                a = 1 - a;
                b = 1 - b;
            }
            c = 1 - a - b;
            points[3 * currentPoint] = (verts[indexes[3 * i]].position[0] * a) + (verts[indexes[3 * i + 1]].position[0] * b) + (verts[indexes[3 * i + 2]].position[0] * c);
            points[3 * currentPoint + 1] = (verts[indexes[3 * i]].position[1] * a) + (verts[indexes[3 * i + 1]].position[1] * b) + (verts[indexes[3 * i + 2]].position[1] * c);
            points[3 * currentPoint + 2] = (verts[indexes[3 * i]].position[2] * a) + (verts[indexes[3 * i + 1]].position[2] * b) + (verts[indexes[3 * i + 2]].position[2] * c);
            currentPoint++;
        }
    }

    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * totalNumPoints * sizeof(GLfloat), points, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete [] points;

}

// Load textures from model
void ObjectNode::loadTextures(ModelOBJ * model) {
    if (model == NULL)
        return;

    const ModelOBJ::Mesh * mesh;
    for (int i = 0; i < model->getNumberOfMeshes(); i++) {
        mesh = &model->getMesh(i);
        if (mesh->pMaterial->colorMapFilename.size() == 0)
            continue;

        std::map < std::string, GLuint >::iterator mi =
                textures.find(mesh->pMaterial->colorMapFilename);
        if (mi == textures.end()) {
            int size = mesh->pMaterial->colorMapFilename.size();
            char *fn = new char[size + 1];
            strncpy(fn, mesh->pMaterial->colorMapFilename.c_str(),
                    size + 1);
            ILuint image;
            ilGenImages(1, &image);
            ilBindImage(image);
            if (!ilLoadImage(fn)) {
                printf("Error loading: %s\n", fn);
                continue;
            }
            GLuint t;
            glGenTextures(1, &t);
            glBindTexture(GL_TEXTURE_2D, t);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

            gluBuild2DMipmaps(GL_TEXTURE_2D, ilGetInteger(IL_IMAGE_FORMAT),
                    ilGetInteger(IL_IMAGE_WIDTH),
                    ilGetInteger(IL_IMAGE_HEIGHT),
                    ilGetInteger(IL_IMAGE_FORMAT),
                    GL_UNSIGNED_BYTE, ilGetData());
            textures[mesh->pMaterial->colorMapFilename] = t;
            ilDeleteImage(image);
            delete[]fn;
        }
    }
}
