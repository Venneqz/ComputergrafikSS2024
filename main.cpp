#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"

// Standard window width
const int WINDOW_WIDTH  = 640;
// Standard window height
const int WINDOW_HEIGHT = 480;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar  = 100.0f;

float r, g, b;
float h, s, v;
float c, m, y;
std::string choice;

void readInput();

/*
Struct to hold data for object rendering.
*/
class Object
{
public:
  inline Object ()
    : vao(0),
      positionBuffer(0),
      colorBuffer(0),
      indexBuffer(0)
  {}

  inline ~Object () { // GL context must exist on destruction
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &positionBuffer);
  }

  GLuint vao;        // vertex-array-object ID
  
  GLuint positionBuffer; // ID of vertex-buffer: position
  GLuint colorBuffer;    // ID of vertex-buffer: color
  
  GLuint indexBuffer;    // ID of index-buffer
  
  glm::mat4x4 model; // model matrix
};

Object triangle;
Object quad;

void renderTriangle()
{
  // Create mvp.
  glm::mat4x4 mvp = projection * view * triangle.model;
  
  // Bind the shader program and set uniform(s).
  program.use();
  program.setUniform("mvp", mvp);
  
  // Bind vertex array object so we can render the 1 triangle.
  glBindVertexArray(triangle.vao);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
  glBindVertexArray(0);
}

void renderQuad()
{
  // Create mvp.
  glm::mat4x4 mvp = projection * view * quad.model;
  
  // Bind the shader program and set uniform(s).
  program.use();
  program.setUniform("mvp", mvp);
  
  // Bind vertex array object so we can render the 2 triangles.
  glBindVertexArray(quad.vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
  glBindVertexArray(0);
}

void initTriangle()
{
  // Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
  const std::vector<glm::vec3> vertices = { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
  const std::vector<glm::vec3> colors   = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
  const std::vector<GLushort>  indices  = { 0, 1, 2 };

  GLuint programId = program.getHandle();
  GLuint pos;

  // Step 0: Create vertex array object.
  glGenVertexArrays(1, &triangle.vao);
  glBindVertexArray(triangle.vao);
  
  // Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
  glGenBuffers(1, &triangle.positionBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triangle.positionBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
  
  // Bind it to position.
  pos = glGetAttribLocation(programId, "position");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  // Step 2: Create vertex buffer object for color attribute and bind it to...
  glGenBuffers(1, &triangle.colorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triangle.colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
  
  // Bind it to color.
  pos = glGetAttribLocation(programId, "color");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  // Step 3: Create vertex buffer object for indices. No binding needed here.
  glGenBuffers(1, &triangle.indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle.indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
  
  // Unbind vertex array object (back to default).
  glBindVertexArray(0);
  
  // Modify model matrix.
  triangle.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f));
}

void initQuad()
{
  // Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
  const std::vector<glm::vec3> vertices = { { -1.0f, 1.0f, 0.0f }, { -1.0, -1.0, 0.0 }, { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } };
  const std::vector<glm::vec3> colors = {
      glm::vec3(r, g, b),
      glm::vec3(r, g, b),
      glm::vec3(r, g, b),
      glm::vec3(r, g, b)
  };

  const std::vector<GLushort>  indices  = { 0, 1, 2, 0, 2, 3 };

  GLuint programId = program.getHandle();
  GLuint pos;
  
  // Step 0: Create vertex array object.
  glGenVertexArrays(1, &quad.vao);
  glBindVertexArray(quad.vao);
  
  // Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
  glGenBuffers(1, &quad.positionBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad.positionBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
  
  // Bind it to position.
  pos = glGetAttribLocation(programId, "position");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  // Step 2: Create vertex buffer object for color attribute and bind it to...
  glGenBuffers(1, &quad.colorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad.colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
  
  // Bind it to color.
  pos = glGetAttribLocation(programId, "color");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  // Step 3: Create vertex buffer object for indices. No binding needed here.
  glGenBuffers(1, &quad.indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
  
  // Unbind vertex array object (back to default).
  glBindVertexArray(0);
  
  // Modify model matrix.
  quad.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f));
}

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
  // OpenGL: Set "background" color and enable depth testing.
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  
  // Construct view matrix.
  glm::vec3 eye(0.0f, 0.0f, 4.0f);
  glm::vec3 center(0.0f, 0.0f, 0.0f);
  glm::vec3 up(0.0f, 1.0f, 0.0f);
  
  view = glm::lookAt(eye, center, up);
  
  // Create a shader program and set light direction.
  if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
    std::cerr << program.log();
    return false;
  }
  
  if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
    std::cerr << program.log();
    return false;
  }
  
  if (!program.link()) {
    std::cerr << program.log();
    return false;
  }

  // Create all objects.
  initTriangle();
  initQuad();
  
  return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderTriangle();
	renderQuad();
}

void glutDisplay ()
{
   render();
   glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize (int width, int height)
{
  // Division by zero is bad...
  height = height < 1 ? 1 : height;
  glViewport(0, 0, width, height);
  
  // Construct projection matrix.
  projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
  switch (keycode) {
  case 27: // ESC
    glutDestroyWindow ( glutID );
    return;
  case 'c':
      readInput();
      initQuad();
	break;
  }
  glutPostRedisplay();
}


void rgbToCmy(float r, float g, float b) {
    // Berechne CMY-Werte
    float c = 1 - r;
    float m = 1 - g;
    float y = 1 - b;

    // Ausgabe der CMY-Werte
    std::cout << "CMY: " << c << ", " << m << ", " << y << std::endl;
}

void rgbToHsv(float r, float g, float b) {
    //Finden des maximalen und minimalen Wertes
    float max = std::max(r, std::max(g, b));
    float min = std::min(r, std::min(g, b));
    v= max;
    //Sättigung berechnen
    if (max != 0) {
		s = (max - min) / max;
	}
    else {
		s = 0;
	}
   //Hue berechnen
    if (max == min) {
        h = 0;
    }
    else if (max = r) {
		h = 60 * (g - b) / (max - min);
	}
    else if (max = g) {
		h = 60 * (2 + (b - r) / (max - min));
	}
    else if (max = b) {
		h = 60 * (4 + (r - g) / (max - min));
	}
    if (h < 0) {
        h = h + 360;
    }
    //Ausgabe der HSV-Werte
    std::cout << "HSV: " << h << ", " << s << ", " << v << std::endl;
}

void hsvToRgb(float h, float s, float v) {
    //Berechnung Sektor im Farbkreis
    float hSektor = h / 60;
    float c = v * s; //Chroma
    float x = c * (1 - abs(fmod(hSektor, 2) - 1));
    float m = v - c;

    //Berechnung der RGB-Werte basierend auf dem Sektor
    if (h >= 0 && h < 60) {
		r = c;
		g = x;
		b = 0;
	}
    else if (h >= 60 && h < 120) {
		r = x;
		g = c;
		b = 0;
	}
    else if (h >= 120 && h < 180) {
		r = 0;
		g = c;
		b = x;
	}
    else if (h >= 180 && h < 240) {
		r = 0;
		g = x;
		b = c;
	}
    else if (h >= 240 && h < 300) {
		r = x;
		g = 0;
		b = c;
	}
    else if (h >= 300 && h < 360) {
		r = c;
		g = 0;
		b = x;
	}
    std::cout << "RGB: " << r << ", " << g << ", " << b << std::endl;
}

void hsvToCmy(float h, float s, float v) {
    //ACHTUNG: geht davon aus, dass die RGB-Werte bereits in hsvToRgb berechnet wurden
    

    // Berechne CMY-Werte
    float c = 1 - r;
    float m = 1 - g;
    float y = 1 - b;

    // Ausgabe der CMY-Werte
    std::cout << "CMY: " << c << ", " << m << ", " << y << std::endl;
}

void cmyToRgb(float c, float m, float y) {
	//Berechnung der RGB-Werte
	r = 1 - c;
	g = 1 - m;
	b = 1 - y;

	//Ausgabe der RGB-Werte
	std::cout << "RGB: " << r << ", " << g << ", " << b << std::endl;
}


void readInput() {
    std::cout << "Wollen sie RGB, CMY oder HSV eingeben?" << std::endl;
    std::cin >> choice;
    if (choice == "RGB" || choice == "rgb") {
        std::cout << "Bitte geben Sie die RGB-Werte ein:" << std::endl;
        std::cout << "R:" << std::endl;
        std::cin >> r;
        std::cout << "G:" << std::endl;
        std::cin >> g;
        std::cout << "B:" << std::endl;
        std::cin >> b;
        //check if values are in range
        if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
            std::cout << "Die Werte müssen zwischen 0 und 1 liegen." << std::endl;
            readInput();
            return;
        }
        else {
            //speichern der rgb Werte
            r = r;
            g = g;
            b = b;
        }

    }
    else if (choice == "HSV" || choice == "hsv") {
        std::cout << "Bitte geben Sie die HSV-Werte ein:" << std::endl;
        std::cout << "H:" << std::endl;
        std::cin >> h;
        std::cout << "S:" << std::endl;
        std::cin >> s;
        std::cout << "V:" << std::endl;
        std::cin >> v;
        //check if values are in range
        if (h < 0 || h > 360 || s < 0 || s > 1 || v < 0 || v > 1) {
            std::cout << "Die Werte müssen zwischen 0 und 360 für H und zwischen 0 und 1 für S und V liegen." << std::endl;
            readInput();
            return;
        }
        else {
            //speichern der hsv Werte
            h = h;
            s = s;
            v = v;
        }
    }
    else if (choice == "CMY" || choice == "cmy") {
        std::cout << "Bitte geben Sie die CMY-Werte ein:" << std::endl;
        std::cout << "C:" << std::endl;
        std::cin >> c;
        std::cout << "M:" << std::endl;
        std::cin >> m;
        std::cout << "Y:" << std::endl;
        std::cin >> y;
        //check if values are in range
        if (c < 0 || c > 1 || m < 0 || m > 1 || y < 0 || y > 1) {
            std::cout << "Die Werte müssen zwischen 0 und 1 liegen." << std::endl;
            readInput();
            return;
        }
        else {
            //speichern der cmy Werte
            c = c;
            m = m;
            y = y;
        }
    }
    else {
        std::cout << "Ungültige Eingabe." << std::endl;
        readInput();
        return;
    }

    //Umrechnungen je nach Eingabe
    if (choice == "RGB" || choice == "rgb") {
        rgbToCmy(r, g, b);
        rgbToHsv(r, g, b);
    }
    else if (choice == "HSV" || choice == "hsv") {
        hsvToRgb(h, s, v);
        hsvToCmy(h, s, v);
    }
    else if (choice == "CMY" || choice == "cmy") {
        cmyToRgb(c, m, y);
    }
    
}

int main(int argc, char** argv)
{
  //Einlesen der user input
  readInput();
  

  // GLUT: Initialize freeglut library (window toolkit).
  glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(40,40);
  glutInit(&argc, argv);
  
  // GLUT: Create a window and opengl context (version 4.3 core profile).
  glutInitContextVersion(4, 3);
  glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  
  glutCreateWindow("Aufgabenblatt 01");
  glutID = glutGetWindow();
  
  // GLEW: Load opengl extensions
  //glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    return -1;
  }
#if _DEBUG
  if (glDebugMessageCallback) {
    std::cout << "Register OpenGL debug callback " << std::endl;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(cg::glErrorVerboseCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE,
			  GL_DONT_CARE,
			  GL_DONT_CARE,
			  0,
			  nullptr,
			  true); // get all debug messages
  } else {
    std::cout << "glDebugMessageCallback not available" << std::endl;
  }
#endif

  // GLUT: Set callbacks for events.
  glutReshapeFunc(glutResize);
  glutDisplayFunc(glutDisplay);
  //glutIdleFunc   (glutDisplay); // redisplay when idle
  
  glutKeyboardFunc(glutKeyboard);
  
  // init vertex-array-objects.
  bool result = init();
  if (!result) {
    return -2;
  }

  // GLUT: Loop until the user closes the window
  // rendering & event handling
  glutMainLoop ();
  
  // Cleanup in destructors:
  // Objects will be released in ~Object
  // Shader program will be released in ~GLSLProgram
  
  return 0;
}
