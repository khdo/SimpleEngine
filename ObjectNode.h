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



#ifndef OBJECTNODE_H_
#define OBJECTNODE_H_

#include "EngineMath.h"

using namespace std;

class ObjectNode {
// An object to store models as a node in a tree
// If the node is only a structural node that contains other nodes and does not have it's own model
// then set structural to true
    public:
        ModelOBJ * model;
        ObjectNode *parent;
        std::list < ObjectNode * >children;
    
        GLfloat scale;
        GLfloat position[3];
        GLfloat velocity[3];
        GLfloat acceleration[3];
        GLfloat angularVelocity[3];
        GLfloat angularAccel[3];
        GLfloat rotation[3];
        GLfloat boxMin[3];
        GLfloat boxMax[3];
        GLfloat mmatrix[16];
        GLuint vbo;
        GLuint ibo;
        GLfloat globalScale;
        GLuint pvbo;
        GLulong totalNumPoints;
        std::map < std::string, GLuint > textures;
    
        bool collision;
        bool structural;        // Is this just a node to contain other nodes
    
        ObjectNode();
        ObjectNode(const char *filename);
    
        void update(long time);
        void calcRad();
        void clearCollision();
        void makeMatrix(ObjectNode * o);
        bool doesCollide(GLfloat x, GLfloat y, GLfloat z, GLfloat rad);
        bool doesCollide(ObjectNode * o);
        void calcScale();
        void generatePoints();
        void loadTextures(ModelOBJ * model);
};

#endif
