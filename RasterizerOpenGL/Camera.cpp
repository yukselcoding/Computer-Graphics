#include "Camera.h"
#include "helper.h"
#include <vector>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
using namespace std;

glm::vec3 Camera::crossProduct(glm::vec3 a, glm::vec3 b) {
    glm::vec3 result(a.y * b.z - b.y * a.z, b.x * a.z - a.x * b.z, a.x * b.y - b.x * a.y);
    return result;
 }


Camera::Camera() {
  
}

void Camera::initialize(int widthTexture, int heightTexture, GLuint &idProgramShader){
  this->v = glm::vec3 (0,1.0,0);
  this->gaze = glm::vec3 (0,0,1.0);
  this->u = crossProduct(this->v, this->gaze);
  this->w = glm::vec3 (-(this->gaze.x), -(this->gaze.y), -(this->gaze.z));
  this->position = glm::vec3 (static_cast<float>(widthTexture/2.0), static_cast<float>(widthTexture/10.0), static_cast<float>(-widthTexture/4.0));
  this->nearPlane = 0.1;
  this->farPlane = 1000;
  this->aspectRatio = 1;
  this->angle = 45.0;
  this->speed = 0;
  this->textureWidth = widthTexture;
  this->textureHeight = heightTexture;
  this->rgbTexture = 0;
  this->idProgramShader = idProgramShader;
  this->idProgramShader = idProgramShader;
  this->vertex = new float[18*widthTexture*heightTexture];
  this->heightFactor = 10;
  this->projection = glm::perspective(this->angle, this->aspectRatio, this->nearPlane, this->farPlane);
}



void Camera::sendVerticesData() {
  unsigned int vertexBufferObject;



  int k=0;
  for(int i=0; i<textureHeight ; i++){
      for(int j=0; j<textureWidth; j++ ){
        

          //FIRST TRIANGLE
          vertex[k++] = j;
          vertex[k++] = 0;
          vertex[k++] = i;

          vertex[k++] = j;
          vertex[k++] = 0;
          vertex[k++] = (i+1);

          vertex[k++] = (j+1);
          vertex[k++] = 0;
          vertex[k++] = i;

          //SECOND TRIANGLE
          vertex[k++] = (j+1);
          vertex[k++] = 0;
          vertex[k++] = (i+1);

          vertex[k++] = (j+1);
          vertex[k++] = 0;
          vertex[k++] = i;

          vertex[k++] = j;
          vertex[k++] = 0;
          vertex[k++] = (i+1);


      }

  }

  glGenBuffers(1, &vertexBufferObject); 
 

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*textureHeight*textureWidth*18, vertex, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0); 
   
}

void Camera::sendTextureData()  {
  glUniform1i(glGetUniformLocation(idProgramShader, "heightTexture"), this->textureHeight);
  glUniform1f(glGetUniformLocation(idProgramShader, "heightFactor"), this->heightFactor);
  glUniform1i(glGetUniformLocation(idProgramShader, "widthTexture"), this->textureWidth);
  glUniform1i(glGetUniformLocation(idProgramShader, "rgbTexture"), this->rgbTexture);
}


void Camera::sendCameraData() {
	glm::vec3 viewCenter = glm::vec3(this->position + this->gaze * this->nearPlane);
	this->view = glm::lookAt(this->position, viewCenter, this->v);
	this->projectionView = this->projection * this->view;
	glUniformMatrix4fv(glGetUniformLocation(this->idProgramShader, "MVP"), 1, GL_FALSE, &this->projectionView[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(this->idProgramShader, "MV"), 1, GL_FALSE, &this->view[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(this->idProgramShader, "cameraPosition"), 1, GL_FALSE, &this->position[0]);
}


void Camera::increaseHeightFactor() {
	this->heightFactor += 0.5;
}

void Camera::decreaseHeightFactor() {
	this->heightFactor -= 0.5;
}

void Camera::increaseSpeed() {
	this->speed += 0.25;
}

void Camera::decreaseSpeed() {
	this->speed -= 0.25;
}

void Camera::pitchDown() {
	// TODO
	this->gaze = glm::rotate(this->gaze, 0.05f, this->u);
	this->v = glm::rotate(this->v, 0.05f, this->u);
}

void Camera::pitchUp() {
	// TODO
	this->gaze = glm::rotate(this->gaze, -0.05f, this->u);
	this->v = glm::rotate(this->v, -0.05f, this->u);
}

void Camera::yawLeft() {
	// TODO
	this->gaze = glm::rotate(this->gaze, 0.05f, this->v);
	this->u = glm::rotate(this->u, 0.05f, this->v);
}

void Camera::yawRight() {
	// TODO
	this->gaze = glm::rotate(this->gaze, -0.05f, this->v);
	this->u = glm::rotate(this->u, -0.05f, this->v);
}

void Camera::updateCameraPosition() {
	this->position += this->speed * this->gaze;			
}