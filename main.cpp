#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "SOIL2/SOIL2/SOIL2.h"
#include "SOIL/SOIL.h"
#include <iostream>

using namespace std;

static GLfloat angle = 0.0;
bool isRotating = false; // Rename rotate to avoid ambiguity
GLuint texID; // Texture ID for Earth texture
GLfloat cameraAngleX = 0.0, cameraAngleY = 0.0;
GLfloat cameraDistance = 5.0;
int lastMouseX, lastMouseY;
bool mousePressed = false;

void initLighting() {
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Define light properties
    GLfloat lightAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat lightDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lightPosition[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Define material properties
    GLfloat matAmbient[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat matDiffuse[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat matShininess[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void loadTexture() {
    texID = SOIL_load_OGL_texture(
        "earth_texture.jpg", // Replace with your Earth texture file path
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS // Remove SOIL_FLAG_INVERT_Y if needed
    );

    if (texID == 0) {
        cout << "Failed to load texture" << std::endl;
    }
    else {
        glBindTexture(GL_TEXTURE_2D, texID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Generate mipmaps
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
}


void display() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up view matrix
    glLoadIdentity();
    gluLookAt(cameraDistance * sin(cameraAngleY) * cos(cameraAngleX),
        cameraDistance * sin(cameraAngleX),
        cameraDistance * cos(cameraAngleY) * cos(cameraAngleX),
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Rotate the sphere
    glRotatef(angle, 0.0, 1.0, 0.0);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Set material properties (if not using shaders)
    // Example material settings
    GLfloat matAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat matDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat matShininess[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    // Draw sphere
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, 1.0, 50, 50);
    gluDeleteQuadric(quad);

    // Disable texturing and lighting after drawing the globe
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    GLfloat closerCameraDistance = cameraDistance * 0.5;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(closerCameraDistance * sin(cameraAngleY) * cos(cameraAngleX),
        closerCameraDistance * sin(cameraAngleX),
        closerCameraDistance * cos(cameraAngleY) * cos(cameraAngleX),
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void idle(void)
{
    if (isRotating) {
        angle += 0.5;
        if (angle > 360) angle -= 360; // Keep the angle in the range [0, 360]
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'r':
        isRotating = !isRotating;
        break;
    case 'w':
        cameraAngleX += 0.05;
        break;
    case 's':
        cameraAngleX -= 0.05;
        break;
    case 'a':
        cameraAngleY += 0.05;
        break;
    case 'd':
        cameraAngleY -= 0.05;
        break;
    case '+':
        cameraDistance -= 0.1;
        if (cameraDistance < 1.0) cameraDistance = 1.0;
        break;
    case '-':
        cameraDistance += 0.1;
        break;
    }
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            lastMouseX = x;
            lastMouseY = y;
            mousePressed = true;
        }
        else if (state == GLUT_UP) {
            mousePressed = false;
        }
    }
}

void motion(int x, int y) {
    if (mousePressed) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        cameraAngleY += deltaX * 0.01;
        cameraAngleX += deltaY * 0.01;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Rotating 3D Sphere");

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    //initLighting();
    loadTexture(); // Load the Earth texture
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard); // Register keyboard callback for key events
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glClearColor(1.0, 1.0, 1.0, 1.0); // Set background color to white

    glutMainLoop();
    
    return 0;
}
