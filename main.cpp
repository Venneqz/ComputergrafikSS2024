#include <iostream>
#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSLProgram.h"

// Fenstergröße
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// GLUT Fenster-ID
int glutID = 0;
cg::GLSLProgram program;
glm::mat4x4 view;
glm::mat4x4 projection;

// Variablen für die Kugel
float sphereRadius = 1.0f;
int n = 0;  // Tessellationstiefe, initial auf der niedrigsten Stufe

// Kugelobjekt
class Sphere {
public:
    GLuint vao;
    GLuint positionBuffer;
    GLuint indexBuffer;
    std::vector<GLushort> indices;
    std::vector<glm::vec3> vertices;

    Sphere() : vao(0), positionBuffer(0), indexBuffer(0) {}
    ~Sphere() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &positionBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }

    void init(int subdivision) {
        // Erzeuge Vertex-Daten basierend auf der Unterteilungsstufe
        createSphere(subdivision);

        // Erzeuge und binde das VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Erzeuge und binde Position Buffer
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        GLuint posLocation = glGetAttribLocation(program.getHandle(), "position");
        glEnableVertexAttribArray(posLocation);
        glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Erzeuge und binde Index Buffer
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

        // Unbinde das VAO
        glBindVertexArray(0);
    }

    void createSphere(int depth) {
        // Platzhalterfunktion für die Kugelerzeugung, sollte tatsächlich implementiert werden
        // Für diese Aufgabe müssen Sie möglicherweise eine Funktion zur Tessellierung von Sphären implementieren
    }

    void render() {
        glBindVertexArray(vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe-Modus
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
};

Sphere sphere;

void init() {
    // OpenGL initialisieren
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // View und Projection Matrizen setzen
    glm::vec3 eye(0.0f, 0.0f, 5.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(eye, center, up);
    projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

    // Shader kompilieren
    if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
        std::cerr << program.log();
    }
    if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
        std::cerr << program.log();
    }
    if (!program.link()) {
        std::cerr << program.log();
    }

    // Kugel initialisieren
    sphere.init(n);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.use();
    glm::mat4 mvp = projection * view;
    program.setUniform("mvp", mvp);

    sphere.render();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'r':
        sphereRadius -= 0.1f; break;
    case 'R':
        sphereRadius += 0.1f; break;
    case '+':
        if (n < 4) {
            n++;
            sphere.init(n);
        }
        break;
    case '-':
        if (n > 0) {
            n--;
            sphere.init(n);
        }
        break;
    case 'n':
        // Reset der Szene
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Praktikum 2 - Aufgabe 1");
    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    init();
    glutMainLoop();
    return 0;
}

