#include "helper.h"
#include "Camera.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cmath>

static GLFWwindow * win = NULL;
#define PI 3.14159265
// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;
Camera cam;
bool fullScreen = false;
int width, height;
int displayWidth;
int displayHeight;

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

void clear() {
  glClearColor(0, 0, 0, 1);
  glClearDepth(1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void configureScreen(){

   

  if(fullScreen){

    fullScreen = false;
    glfwSetWindowMonitor( win, nullptr, 0, 0, displayWidth, displayHeight, 0 );

  }else{
    displayWidth = width;
    displayHeight = height;

    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    fullScreen = true;
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    glfwSetWindowMonitor( win, monitor, 0, 0, mode->width, mode->height, 0 );
  }

}



static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  
    switch(key) {
      case GLFW_KEY_ESCAPE:
      {
        if(action == GLFW_PRESS) {
          glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
        break;
      }
        
      
      case GLFW_KEY_O:
      {
        //Increase height factor
        cam.increaseHeightFactor();
        GLuint hfLocation = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(hfLocation, cam.heightFactor);
        break;
      }
        
      
      case GLFW_KEY_L:
      {
         //Decrease height factor
        cam.decreaseHeightFactor();
        GLuint hfLocation = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(hfLocation, cam.heightFactor);
        break;
      }
       

      case GLFW_KEY_W:
      {
         //Pitch up
        cam.pitchUp();
        break;
      }
       
      
      case GLFW_KEY_S:
      {
         //Pitch down
        cam.pitchDown();
        break;
      }
       
      
      case GLFW_KEY_D:
      {
        //Yaw right
        cam.yawRight();
        break;
      }
        
      
      case GLFW_KEY_A:
      {
        //Yaw left
        cam.yawLeft();
        break;
      }
        
      
      case GLFW_KEY_U:
      {
        //Increase speed
        cam.increaseSpeed();
        break;
      }
        
      
      case GLFW_KEY_J:
      {
        //Decrease speed
        cam.decreaseSpeed();
        break;

      }
        
      case GLFW_KEY_F:
      {
        if(action == GLFW_PRESS) {
          configureScreen();
        }
        //Switch to full screen
        break;
      }
    }

}


int main(int argc, char * argv[]) {

  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  
  glfwMakeContextCurrent(win);
  
 
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);
  cam.initialize(widthTexture, heightTexture, idProgramShader);

  glEnable(GL_DEPTH_TEST);

  glfwSetKeyCallback(win, keyCallback);

  cam.sendVerticesData();

  int vertexSize = 18*widthTexture*heightTexture;

  while (!glfwWindowShouldClose(win)) {
   
    clear();

    
    glfwGetWindowSize(win, &width, &height);
    glViewport (0, 0, width, height);


    cam.sendTextureData();
    cam.updateCameraPosition();
    cam.sendCameraData();

    glDrawArrays(GL_TRIANGLES, 0, vertexSize);

    
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}