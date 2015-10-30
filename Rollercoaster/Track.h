#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "math.h"

using namespace std;
using namespace glm;

const GLfloat MIN_SPEED = 1.0f;
const GLfloat GRAVITY = 9.81f;
const GLfloat TIME_CHANGE = 0.016f;

class Track{
public:
	Track(string filename);

	vector<GLuint> segmentStarts;

	GLuint getNumVerts(){ return numVertices; }
	vec3 getCartPos(){ return cartPos; }
	GLuint getPosIndex(){ return posIndex; }
	GLuint getLeftSize(){ return leftTrack.size(); }
	GLuint getRightSize(){ return rightTrack.size(); }

	void subdivideVertices();
	void bufferWire(GLuint vertLocation, GLuint normLocation);
	void bufferLeft(GLuint vertLocation, GLuint normLocation);
	void bufferRight(GLuint vertLocation, GLuint normLocation);
	void drawWire(GLuint ambient);
	void drawLeft();
	void drawRight();
	void findMax();
	void findMin();
	vector<vec3> toVec3();
	vector<GLfloat> toFloat(vector<vec3> vecs);
	mat4 updateCart();
	vector<GLuint> getSegmentStarts();
	GLfloat calcSpeed(GLfloat currentSpeed, GLuint index, GLfloat height);
	mat4 getNewModel(GLuint index, GLfloat speed);
	void getTracks(vector<vec3> vecs);

private:
	GLuint vertexBuffer, numVertices, posIndex;
	GLuint leftBuffer, rightBuffer, leftArray, rightArray;
	vector<GLfloat> vertices, normals;
	vector<GLfloat> leftTrack, rightTrack, leftNorm, rightNorm;
	vector<vec3> vectors;
	vec3 max, min, cartPos;
	GLfloat speed;
	GLboolean firstPass;
};
