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

#include "SimpleEngine.h"

// Have to use a static variable as glut requires static functions for the callbacks
SimpleEngine * ENGINE = new SimpleEngine();

// Handle the menu options
void handleMenu(int m) {
    switch (m) {
    case 0:
        exit(0);
        break;
    }
}

void handleMotion(int x, int y) {
}

void handleMouse(int b, int s, int x, int y) {
}

// Handle key press events for special keys
void handleSpecialKeys(int key, int x, int y) {
    ENGINE->specialKeys(key, x, y);
}

// Handle key up events for special keys
void handleSpecialUpKeys(int key, int x, int y) {
    ENGINE->specialUpKeys(key, x, y);
}

// Handle key press events
void handleKeys(unsigned char key, int x, int y) {
    ENGINE->keys(key, x, y);
}

// Callback function to handle key up events
void handleUpKeys(unsigned char key, int x, int y) {
    ENGINE->upKeys(key, x, y);
}

// Handle draw calls
void handleDrawScreen() {
    ENGINE->drawScreen();
}

void handleTimer() {
    ENGINE->timer();
}

// Load a brush texture from filename
GLuint SimpleEngine::loadBrush(const char * filename) {
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);
    if (!ilLoadImage(filename)) {
        printf("Error loading: %s\n", filename);
        exit(-1);
    }
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, ilGetInteger(IL_IMAGE_FORMAT),
            ilGetInteger(IL_IMAGE_WIDTH),
            ilGetInteger(IL_IMAGE_HEIGHT),
            ilGetInteger(IL_IMAGE_FORMAT),
            GL_UNSIGNED_BYTE, ilGetData());
    ilDeleteImage(image);
    return t;
}

// Constructor
SimpleEngine::SimpleEngine(void) :
            camPos{
                0, 10, 150
            },
            mmatrix{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            },
            invcammat{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            }
{
    drawPolys = false;
    whichTexture = 0;
    camRotation = 0;
}

// Read a textfile and store in GLchar array
void readTextFile(const char *filename, GLchar const **source) {
    int lineCount = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error opening file: %s\n", filename);
        exit(-1);
    }
    GLchar *line = new GLchar[200];

    while (fgets(line, 200, f) != NULL) {
        source[lineCount] = line;
        line = new GLchar[200];
        lineCount++;
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("SimpleEngine");

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        printf("Error initializing Glew\n");
        printf("Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }

    if (!glewIsSupported("GL_VERSION_2_0 GL_EXT_geometry_shader4 \
            GL_EXT_texture_array GL_ARB_texture_cube_map")) {
        printf("Error openGL 2.0 required or missing extensions\n");
        exit(-1);
    }
    // Setup callbacks
    glutDisplayFunc(handleDrawScreen);
    glutMotionFunc(handleMotion);
    glutMouseFunc(handleMouse);
    glutSpecialFunc(handleSpecialKeys);
    glutSpecialUpFunc(handleSpecialUpKeys);
    glutKeyboardFunc(handleKeys);
    glutKeyboardUpFunc(handleUpKeys);

    // Setup the menu
    glutCreateMenu(handleMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    ENGINE->setupFramebuffers();

    glutIdleFunc(handleTimer);
    glutMainLoop();
    // ANSI C requires main to return an int
    return 0;
}

// Setup Framebuffers and other OpenGL objects
void SimpleEngine::setupFramebuffers() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SPRITE_ARB);
    glEnable(GL_PROGRAM_POINT_SIZE);

    GLfloat amb[4] = { 0, 0, 0, 0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    glGenFramebuffers(1, &shadowFrameBuffer);

    // Create shadow maps textures
    glGenTextures(NUMSMAPS, shadowTextures);
    for (int i = 0; i < NUMSMAPS; i++) {
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, shadowTextures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SMAP_WIDTH,
                SMAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    glGenFramebuffers(1, &refFrameBuffer);
    glGenFramebuffers(1, &impFrameBuffer);
    glGenTextures(1, &impColor);
    glBindTexture(GL_TEXTURE_2D, impColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);

    glGenTextures(1, &refColor);
    glBindTexture(GL_TEXTURE_2D, refColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);

    glGenTextures(1, &refNormal);
    glBindTexture(GL_TEXTURE_2D, refNormal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);

    glGenTextures(1, &refSize);
    glBindTexture(GL_TEXTURE_2D, refSize);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);

    glGenTextures(1, &refDepth);
    glBindTexture(GL_TEXTURE_2D, refDepth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, refFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            refColor, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
            refNormal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
            refSize, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                refDepth, 0);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
        printf("Error setting up reference framebuffer object\n");
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, impFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            impColor, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
            refDepth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
        printf("Error setting up reference framebuffer object\n");
        exit(-1);
    }

    setupShaders();

    // Light setup
    glLightfv(GL_LIGHT0, GL_SPECULAR, black);
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, black);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos[0]);

    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glLightfv(GL_LIGHT1, GL_AMBIENT, white);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos[1]);

    // Setup image library
    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    // Load in brush files
    brush[0] = loadBrush("brush0.jpg");
    brush[1] = loadBrush("brush1.jpg");
    brush[2] = loadBrush("brush2.jpg");
    brush[3] = loadBrush("brush3.jpg");
    brush[4] = loadBrush("brush4.jpg");
    brush[5] = loadBrush("brush5.jpg");
    brush[6] = loadBrush("brush6.jpg");

    struct TIMEB tb;
    FTIME(&tb);
    lastTime = (tb.time * 1000) + tb.millitm;
    loadData();
    head->calcScale();
}

// 
void SimpleEngine::makeRefImages() {
    // Setup the perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, SCREEN_WIDTH / SCREEN_HEIGHT, 10, 500);

    // Viewing transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(camRotation, 0, 1, 0);
    glTranslatef(-camPos[0], -camPos[1], -camPos[2]);
    glGetFloatv(GL_MODELVIEW_MATRIX, mmatrix);
    inverseMat(mmatrix, invcammat);

    makeShadowMaps();

    glBindFramebuffer(GL_FRAMEBUFFER, refFrameBuffer);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5, 0.75, 0.9, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(irotID, 1, GL_FALSE, invcammat);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    for (int i = 0; i < NUMSMAPS; i++) {
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, shadowTextures[i]);
    }

    // Light setup
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos[0]);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos[1]);

    draw(head);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SimpleEngine::makeImpImage() {
    glBindFramebuffer(GL_FRAMEBUFFER, impFrameBuffer);
    glUseProgram(impProg);
    GLint colorLoc = glGetUniformLocation(impProg, "color");
    GLint normalLoc = glGetUniformLocation(impProg, "normal");
    GLint sizeLoc = glGetUniformLocation(impProg, "size");
    GLint brushLoc = glGetUniformLocation(impProg, "brush");
    glUniform1i(colorLoc, 0);
    glUniform1i(normalLoc, 1);
    glUniform1i(sizeLoc, 2);
    glUniform1i(brushLoc, 3);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5, 0.75, 0.9, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, refColor);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, refSize);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, brush[whichBrush]);
    glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

    drawPoints(head);

    glDisable(GL_POINT_SPRITE_ARB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Handle key press events for special keys
void SimpleEngine::specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        leftKey = true;
        break;
    case GLUT_KEY_RIGHT:
        rightKey = true;
        break;
    case GLUT_KEY_UP:
        upKey = true;
        break;
    case GLUT_KEY_DOWN:
        downKey = true;
        break;
    }
}

// Handle key up events for special keys
void SimpleEngine::specialUpKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        leftKey = false;
        break;
    case GLUT_KEY_RIGHT:
        rightKey = false;
        break;
    case GLUT_KEY_UP:
        upKey = false;
        break;
    case GLUT_KEY_DOWN:
        downKey = false;
        break;
    }
}

// Handle key press events
void SimpleEngine::keys(unsigned char key, int x, int y) {
    switch (key) {
    case 'z':
    case 'Z':
        zKey = true;
    break;
    case 'x':
    case 'X':
        xKey = true;
        break;
    case 'b':
    case 'B':
        whichTexture++;
        if (whichTexture == 4)
            whichTexture = 0;
        break;
    case 'v':
    case 'V':
        whichBrush++;
        if (whichBrush == NUMBRUSHES)
            whichBrush = 0;
        break;
    case 'p':
    case 'P':
        if (drawPolys)
            drawPolys = false;
        else
            drawPolys = true;
        break;
    case 0x1b:
        exit(0);
        break;
    }
}

// Handle key up events
void SimpleEngine::upKeys(unsigned char key, int x, int y) {
    switch (key) {
    case 'z':
    case 'Z':
        zKey = false;
        break;
    case 'x':
    case 'X':
        xKey = false;
        break;
    case 'b':
    case 'B':
        break;
    }
}

// Render the scene and draw the screen
void SimpleEngine::drawScreen() {

    // First, we'll do a normal render pass to make a regular image of the scene
    makeRefImages();

    // Then we'll render the scene again as a cloud of brush strokes
    makeImpImage();

    glMaterialfv(GL_FRONT, GL_EMISSION, black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
    glMaterialfv(GL_FRONT, GL_AMBIENT, black);

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Choose which version of the scene to print out
    glUseProgram(quadProg);
    GLint texLoc = glGetUniformLocation(quadProg, "texture");
    glUniform1i(texLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    switch (whichTexture) {
    case 0:
        glBindTexture(GL_TEXTURE_2D, impColor);
        break;
    case 1:
        glBindTexture(GL_TEXTURE_2D, refColor);
        break;
    case 2:
        glBindTexture(GL_TEXTURE_2D, refNormal);
        break;
    case 3:
        glBindTexture(GL_TEXTURE_2D, refSize);
        break;
    }

    glPolygonMode(GL_FRONT, GL_FILL);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0, -1.0, .9);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0, -1.0, .9);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0, 1.0, .9);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0, 1.0, .9);
    glEnd();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(-.9, .9, .8);
    glScalef(0.0004, 0.0004, 0.0004);

    // Print the Frames per second on the screen
    char fps[10];
    SNPRINTF(fps, sizeof(fps), "%4.1f", 1000.0 / eTime);
    for (char *c = fps; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
    glPopMatrix();

    glutSwapBuffers();
}

// Calculate the time difference, and adjust the camera, and objects
void SimpleEngine::timer() {
    struct TIMEB tb;

    long newTime;
    FTIME(&tb);
    newTime = (tb.time * 1000) + tb.millitm;
    eTime = newTime - lastTime;
    double t = eTime * TIMESKEW;

    if (leftKey)
        camRotation -= t;
    if (rightKey)
        camRotation += t;
    if (upKey) {
        camPos[0] -= t * cos(2 * PI * (camRotation + 90) / 360);
        camPos[2] -= t * sin(2 * PI * (camRotation + 90) / 360);
    }
    if (downKey) {
        camPos[0] += t * cos(2 * PI * (camRotation + 90) / 360);
        camPos[2] += t * sin(2 * PI * (camRotation + 90) / 360);
    }
    if (zKey)
        camPos[1] -= t;
    if (xKey)
        camPos[1] += t;

    head->update(eTime);

    GLfloat v[3];
    v[0] = (GLfloat) camPos[0];
    v[1] = (GLfloat) camPos[1];
    v[2] = (GLfloat) camPos[2];
    lastTime = newTime;
    glutPostRedisplay();
}

// Load in the 3d models and populate the scene
void SimpleEngine::loadData() {
    head = new ObjectNode();
    head->structural = true;

    ObjectNode *temp;
    temp = new ObjectNode(CUBE);
    int cs = 400;
    temp->position[0] = -cs/2;
    temp->position[1] = -cs/2;
    temp->position[2] = -cs/2;
    temp->scale = cs;
    temp->structural = false;
    temp->parent = head;
    head->children.push_back(temp);
    temp = new ObjectNode(ROUNDABOUT);
    temp->position[1] = -10;
    temp->scale = 10;
    temp->structural = false;
    temp->parent = head;
    head->children.push_back(temp);

    //ObjectNode *temp4 = new ObjectNode();
    //temp4->structural = true;
    //temp4->position[0] = 70;
    //temp4->position[1] = 5;
    //temp4->angularVelocity[1] = 2;
    //temp4->parent = head;

    //temp = new ObjectNode(SPACESHIP);
    //temp->scale = 1;
    //temp->position[0] = -40;
    //temp->rotation[1] = 90;
    //temp->structural = false;
    //temp->parent = temp4;
    //temp4->children.push_back(temp);

    //head->children.push_back(temp4);

    //temp = new ObjectNode();
    //temp->position[0] = 0;
    //temp->position[1] = 30;
    //temp->position[2] = -30;
    //temp->structural = true;
    //temp->angularVelocity[1] = 1;
    //ObjectNode *temp2 = new ObjectNode();
    //temp2->structural = true;
    //temp2->position[0] = 30;
    //temp2->position[1] = 20;
    //temp2->position[2] = 0;
    //temp2->rotation[1] = -90;
    //temp2->angularVelocity[2] = 3;

    //ObjectNode *temp3 = new ObjectNode(SPACESHIP);
    //temp3->position[1] = 40;
    //temp3->scale = 1;
    //temp3->structural = false;
    //temp3->parent = temp2;
    //temp2->parent = temp;
    //temp2->children.push_back(temp3);
    //temp->children.push_back(temp2);

    //temp2 = new ObjectNode(SPACESHIP);
    //temp2->position[0] = 0;
    //temp2->position[1] = 0;
    //temp2->position[2] = 20;
    //temp2->rotation[1] = 180;
    //temp2->angularVelocity[0] = 2;
    //temp2->scale = 1;
    //temp2->structural = false;
    //temp2->parent = temp;
    //temp->parent = head;
    //temp->children.push_back(temp2);
    //head->children.push_back(temp);

    //temp = new ObjectNode(LAMP);
    //temp->scale = 5;
    //temp->position[1] = -10;
    //temp->parent = head;
    //head->children.push_back(temp);
    //temp->structural = false;

}

// Check for object collisions
void SimpleEngine::checkObjectCollisions(ObjectNode * o) {
    head->doesCollide(o);
    std::list < ObjectNode * >::iterator oi = o->children.begin();
    while (oi != o->children.end()) {
        checkObjectCollisions(*oi);
        oi++;
    }
}

// Link the shader programs
void linkProgram(GLuint program) {
    GLint retCode;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &retCode);
    if (retCode != GL_TRUE) {
        printf("Error linking program\n");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("Infolog: %s\n", infoLog);
        exit(-1);
    }
}

// Load a glsl shader programs from a file
GLuint SimpleEngine::loadShader(const char *filename, GLenum shaderType) {
    GLint len = readFileLength(filename);
    if (len <= 0) {
        printf("Error loading shader file: %s\n", filename);
        exit(-1);
    }

    GLchar const **source = new GLchar const *[len];
    readTextFile(filename, source);
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, len, source, NULL);
    GLint retCode;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &retCode);
    if (retCode != GL_TRUE) {
        printf("Error compiling shader: %s\n", filename);
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        printf("Infolog: %s\n", infoLog);
        exit(-1);
    }

    for (int i = 0; i < len; i++)
        delete[]source[i];
    delete[]source;
    return shader;
}

// Load in our shaders, compile them, and set their uniform variables
void SimpleEngine::setupShaders() {
    GLuint vshader, pshader, gshader, qvshader, qpshader, impvshader, imppshader;
    vshader = loadShader(VSFILENAME, GL_VERTEX_SHADER);
    pshader = loadShader(PSFILENAME, GL_FRAGMENT_SHADER);
    gshader = loadShader(GSFILENAME, GL_GEOMETRY_SHADER_EXT);
    qvshader = loadShader(QVSFILENAME, GL_VERTEX_SHADER);
    qpshader = loadShader(QPSFILENAME, GL_FRAGMENT_SHADER);
    impvshader = loadShader(IMPVSFILENAME, GL_VERTEX_SHADER);
    imppshader = loadShader(IMPPSFILENAME, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vshader);
    glAttachShader(shaderProgram, gshader);
    glAttachShader(shaderProgram, pshader);
    glProgramParameteriEXT(shaderProgram, GL_GEOMETRY_INPUT_TYPE_EXT,
            GL_TRIANGLES);
    glProgramParameteriEXT(shaderProgram, GL_GEOMETRY_OUTPUT_TYPE_EXT,
            GL_TRIANGLE_STRIP);
    glProgramParameteriEXT(shaderProgram, GL_GEOMETRY_VERTICES_OUT_EXT, 3);

    linkProgram(shaderProgram);

    quadProg = glCreateProgram();
    glAttachShader(quadProg, qvshader);
    glAttachShader(quadProg, qpshader);
    linkProgram(quadProg);

    glUseProgram(quadProg);
    GLint texLoc = glGetUniformLocation(quadProg, "texture");
    glUniform1i(texLoc, 0);

    glUseProgram(shaderProgram);
    texLoc = glGetUniformLocation(shaderProgram, "texture0");
    GLint smapLoc = glGetUniformLocation(shaderProgram, "shadowmap");

    GLint texs[NUMSMAPS] = { 1, 2 };
    glUniform1i(texLoc, 0);
    glUniform1iv(smapLoc, NUMSMAPS, texs);

    impProg = glCreateProgram();
    glAttachShader(impProg, impvshader);
    glAttachShader(impProg, imppshader);
    linkProgram(impProg);
    glUseProgram(impProg);
    GLint colorLoc = glGetUniformLocation(impProg, "color");
    GLint normalLoc = glGetUniformLocation(impProg, "normal");
    GLint sizeLoc = glGetUniformLocation(impProg, "size");
    GLint brushLoc = glGetUniformLocation(impProg, "brush");
    glUniform1i(colorLoc, 0);
    glUniform1i(normalLoc, 1);
    glUniform1i(sizeLoc, 2);
    glUniform1i(brushLoc, 3);
}

// Get the length of a file
int readFileLength(const char *filename) {
    int lineCount = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error opening file: %s\n", filename);
        exit(-1);
    }
    GLchar *line = new GLchar[200];
    while (fgets(line, 200, f) != NULL) {
        lineCount++;
    }
    fclose(f);
    return lineCount;
}

// Make our shadow maps by doing a simple render of the scene
void SimpleEngine::makeShadowMaps() {
    GLfloat *up;
    GLfloat *dir;
    GLfloat *pos;
    GLfloat *clip;

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
    glViewport(0, 0, SMAP_WIDTH, SMAP_HEIGHT);
    glUseProgram(quadProg);
    glShadeModel(GL_FLAT);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // Do one render pass per shadow map
    for (int i = 0; i < NUMSMAPS; i++) {
        pos = lightPos[i];
        up = lightUp[i];
        dir = lightDir[i];
        clip = lightClip[i];
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D, shadowTextures[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                GL_FRAMEBUFFER_COMPLETE) {
            printf("Error setting up map framebuffer object\n");
            exit(-1);
        }

        glActiveTexture(GL_TEXTURE1 + i);
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf(bias);
        gluPerspective(120, SMAP_WIDTH / SMAP_HEIGHT, clip[0], clip[1]);
        gluLookAt(pos[0], pos[1], pos[2], dir[0], dir[1], dir[2], up[0],
                up[1], up[2]);
        glMultMatrixf(invcammat);

        // Setup the perspective projection
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluPerspective(120, SMAP_WIDTH / SMAP_HEIGHT, clip[0], clip[1]);

        // Viewing transformation
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        gluLookAt(pos[0], pos[1], pos[2], dir[0], dir[1], dir[2], up[0],
                up[1], up[2]);

        glClearColor(1, 1, 1, 1);
        glClear(GL_DEPTH_BUFFER_BIT);

        simpleDraw(head);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Draw a simplified version of an object and its children
void SimpleEngine::simpleDraw(ObjectNode * node) {
    if (!node->structural) {
        glPushMatrix();
        glMultMatrixf(node->mmatrix);

        const ModelOBJ::Mesh * mesh;

        for (int i = 0; i < node->model->getNumberOfMeshes(); i++) {
            mesh = &node->model->getMesh(i);
            glBindBuffer(GL_ARRAY_BUFFER, node->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node->ibo);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, node->model->getVertexSize(),
                    BUFFER_OFFSET(0));
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, node->model->getVertexSize(),
                    BUFFER_OFFSET(5 * sizeof(GLfloat)));
            glDrawElements(GL_TRIANGLES, mesh->triangleCount * 3,
                    GL_UNSIGNED_INT, BUFFER_OFFSET(sizeof(GLuint) *
                    mesh->startIndex));
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glPopMatrix();
    }

    std::list < ObjectNode * >::iterator oi = node->children.begin();
    while (oi != node->children.end()) {
        simpleDraw(*oi);
        oi++;
    }
}

// Draw the specified object and all its children
void SimpleEngine::draw(ObjectNode * node) {
    // Skip nodes that are structural as they have no models
    if (!node->structural) {
        glPushMatrix();
        glMultMatrixf(node->mmatrix);

        if (drawPolys)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (node->collision) {
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, red);
        } else {
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

        }

        const ModelOBJ::Mesh * mesh;
        const ModelOBJ::Material * mat;

        glUseProgram(shaderProgram);

        for (int i = 0; i < node->model->getNumberOfMeshes(); i++) {
            mesh = &node->model->getMesh(i);
            mat = mesh->pMaterial;

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat->specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat->emission);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->shininess);
            map < std::string, GLuint >::iterator it =
                    node->textures.find(mat->colorMapFilename);
            glActiveTexture(GL_TEXTURE0);
            if (it != node->textures.end()) {
                glBindTexture(GL_TEXTURE_2D, it->second);
            } else
                glBindTexture(GL_TEXTURE_2D, 0);

            glBindBuffer(GL_ARRAY_BUFFER, node->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node->ibo);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, node->model->getVertexSize(),
                    BUFFER_OFFSET(0));
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, node->model->getVertexSize(),
                    BUFFER_OFFSET(5 * sizeof(GLfloat)));
            if (node->model->hasTextureCoords()) {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, node->model->getVertexSize(),
                        BUFFER_OFFSET(3 * sizeof(GLfloat)));
            }
            glDrawElements(GL_TRIANGLES, mesh->triangleCount * 3,
                    GL_UNSIGNED_INT,
                    BUFFER_OFFSET(sizeof(GLuint) *
                            mesh->startIndex));
            if (node->model->hasTextureCoords()) {
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glPopMatrix();
    }

    std::list < ObjectNode * >::iterator oi = node->children.begin();
    while (oi != node->children.end()) {
        draw(*oi);
        oi++;
    }
}

// Draw our list of points
void SimpleEngine::drawPoints(ObjectNode * node) {
    if (!node->structural) {
        glPushMatrix();
        glMultMatrixf(node->mmatrix);
        glBindBuffer(GL_ARRAY_BUFFER, node->pvbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glDrawArrays(GL_POINTS, 0, node->totalNumPoints);
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glPopMatrix();
    }

    std::list < ObjectNode * >::iterator oi = node->children.begin();
    while (oi != node->children.end()) {
        drawPoints(*oi);
        oi++;
    }
}
