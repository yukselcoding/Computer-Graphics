#ifndef __CAMERA__H__
#define __CAMERA__H__


#include "helper.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cmath>

using namespace std;


struct vertex {
  float x, y, z;

  double lengthOfVertex() {
    return sqrt(x * x + y * y + z * z);
  }

  

  static vertex normalizeVertex(vertex v) {
    vertex result;
    double d;

    d = v.lengthOfVertex();
    result.x = v.x / d;
    result.y = v.y / d;
    result.z = v.z / d;

    return result;
  }
};


class Camera {
public:
	int widthTexture;
	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 w;
	glm::vec3 position;
	glm::vec3 gaze;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 projectionView;
	float* vertex;

	double angle;
	double aspectRatio;
	double nearPlane;
	double farPlane;
	double heightFactor;
	double speed;
	int textureWidth;
	int textureHeight;
	GLint rgbTexture;
	GLuint idProgramShader;	

	Camera();
	void initialize(int widthTexture, int heightTexture, GLuint &idProgramShader);
	glm::vec3 crossProduct(glm::vec3 a, glm::vec3 b);
	void increaseHeightFactor();
	void decreaseHeightFactor();
	void increaseSpeed();
	void decreaseSpeed();
	void pitchDown();
	void pitchUp();
	void yawLeft();
	void yawRight();
	void updateCameraPosition();
	void sendVerticesData();
	void sendCameraData(); 
	void sendTextureData();
private:

};


#endif