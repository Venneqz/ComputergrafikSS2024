#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "GLSLProgram.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
int glutID = 0;
cg::GLSLProgram program;
glm::mat4x4 view;
glm::mat4x4 projection;

// Einfache Kugel-Klasse mit Tessellation
class Sphere {
public:
    GLuint vao;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    int indicesCount;
    glm::mat4 modelMatrix;

    Sphere() : vao(0), vertexBuffer(0), indexBuffer(0), indicesCount(0) {}

    void init(int recursionLevel) {
        // Tessellation und Aufbau der Kugel hier einfügen
    }

    void draw(glm::mat4 projection, glm::mat4 view) {
        glm::mat4 mvp = projection * view * modelMatrix;
        program.use();
        program.setUniform("mvp", mvp);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

    ~Sphere() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
};

Sphere sphere;
int recursionLevel = 0; // Tessellationsstufe

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sphere.draw(projection, view);
    glutSwapBuffers();
}

void resize(int width, int height) {
    height = (height == 0) ? 1 : height;
    glViewport(0, 0, width, height);
    projection = glm::perspective(45.0f, float(width) / float(height), 0.1f, 100.0f);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC
        glutDestroyWindow(glutID);
        exit(0);
        break;
    case '+':
        if (recursionLevel < 4) {
            recursionLevel++;
            sphere.init(recursionLevel);
        }
        break;
    case '-':
        if (recursionLevel > 0) {
            recursionLevel--;
            sphere.init(recursionLevel);
        }
        break;
    }
    glutPostRedisplay();
}

bool init() {
    glClearColor(0.2, 0.2, 0.2, 1);
    glEnable(GL_DEPTH_TEST);

    if (!program.compileShaderFromFile("vertex_shader.glsl", cg::GLSLShader::VERTEX)) {
        std::cerr << "Vertex shader compilation failed." << std::endl;
        return false;
    }
    if (!program.compileShaderFromFile("fragment_shader.glsl", cg::GLSLShader::FRAGMENT)) {
        std::cerr << "Fragment shader compilation failed." << std::endl;
        return false;
    }
    if (!program.link()) {
        std::cerr << "Shader linking failed." << std::endl;
        return false;
    }
    sphere.init(recursionLevel);
    return true;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Praktikum 2");
    glutID = glutGetWindow();

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    if (!init()) return -1;

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}

