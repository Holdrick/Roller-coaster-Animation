#include "Track.h"
#include <glm/gtx/rotate_vector.hpp>


Track::Track(string filename){
	GLfloat f;
	ifstream reader(filename.c_str());

	if (reader.is_open()){
		while (!reader.eof()){
			reader >> f;
			vertices.push_back(f);

			reader >> f;
			vertices.push_back(f);

			reader >> f;
			vertices.push_back(f);

			vertices.push_back(1.0f);
		}
		reader.close();
	}
	else{
		cout << "Couldn't open file \"" << filename << "\"" << endl;
	}

	vectors = toVec3();
	subdivideVertices();
	subdivideVertices();
	subdivideVertices();
	vectors.resize(vectors.size() - 1);
	getTracks(vectors);
	cartPos = vectors[0];
	segmentStarts = getSegmentStarts();
	findMin();
	findMax();

	firstPass = true;
	speed = MIN_SPEED;
	posIndex = 0;
}

vector<vec3> Track::toVec3(){
	vector<vec3> vects;

	for (int i = 0; i < vertices.size(); i += 4)
		vects.push_back(vec3(vertices[i], vertices[i + 1], vertices[i + 2]));

	return vects;
}

vector<GLfloat> Track::toFloat(vector<vec3> vecs){
	vector<GLfloat> verts;

	for (int i = 0; i < vecs.size(); i++){
		verts.push_back(vecs[i].x);
		verts.push_back(vecs[i].y);
		verts.push_back(vecs[i].z);
		verts.push_back(1.0f);
	}

	return verts;
}

void Track::subdivideVertices(){
	vector<vec3> newVectors, newCoords;
	vec3 v;

	for (int i = 0; i < vectors.size()-1; i++){
		v = (vectors[i] * 0.5f) + (vectors[i + 1] * 0.5f);
		newVectors.push_back(vectors[i]);
		newVectors.push_back(v);
	}
	newVectors.push_back(newVectors[0]);

	for (int i = 0; i < newVectors.size() - 1; i++){
		v = (newVectors[i] * 0.5f) + (newVectors[i + 1] * 0.5f);
		newCoords.push_back(v);
	}
	newCoords.push_back(newCoords[0]);
	vertices = toFloat(newCoords);
	vectors = newCoords;
	numVertices = vertices.size()/4;

	for (int i = 0; i < numVertices; i++){
		normals.push_back(0.0f);
		normals.push_back(1.0f);
		normals.push_back(0.0f);
	}
}

vector<GLuint> Track::getSegmentStarts(){
	vector<GLuint> starts;
	GLfloat y = vectors[1].y;
	GLuint index = 0;

	for (int i = 0; i < vectors.size(); i++){
		if (vectors[i].y > y){
			index = i;
			y = vectors[i].y;
		}
	}

	starts.push_back(0);
	starts.push_back(index);
	starts.push_back(vectors.size() * (0.8));

	return starts;
}

GLfloat Track::calcSpeed(GLfloat currentSpeed, GLuint index, GLfloat height){
	GLfloat speed;

	if (index < segmentStarts[1])
		speed = MIN_SPEED;
	else if (index < segmentStarts[2])
		speed = sqrt((2 * GRAVITY) * (max.y + 0.01f - height));
	else{
		if (currentSpeed * 0.96f < MIN_SPEED)
			speed = MIN_SPEED;
		else
			speed = currentSpeed * 0.96f;
	}

	return speed;
}

mat4 Track::getNewModel(GLuint index, GLfloat speed){
	mat4 model;
	GLuint previous, next;
	vec3 tangent, normal, binormal, tanPrime, normPrime;
	GLfloat centripedal, radius, height;

	if (index + 1 == vectors.size())
		next = 0;
	else
		next = index + 1;

	if (index - 1 == -1)
		previous = vectors.size() - 1;
	else
		previous = index - 1;

	centripedal = length(vectors[next] - vectors[index]);
	height = length(0.5f * (vectors[next] - (2.0f * vectors[index]) + vectors[previous]));
	radius = (pow(centripedal, 2) + (4.0f * pow(height, 2))) / (8.0f * height);

	tangent = (vectors[next] - vectors[index]) / length(vectors[next] - vectors[index]);

	if (length(vectors[next] - (2.0f * vectors[index]) + vectors[previous]) != 0){
		normal = (vectors[next] - (2.0f * vectors[index]) + vectors[previous]) / length(vectors[next] - (2.0f * vectors[index]) + vectors[previous]);
		binormal = cross(tangent, normal);
		tanPrime = cross(binormal, normal);
		normPrime = (normal * (pow(speed, 2) / radius)) + vec3(0.0f, -GRAVITY, 0.0f); 
	}
	else{
	  vec3 someVector;
	  if(tangent == vec3(0.0f, 1.0f, 0.0f))
	    someVector = vec3(1.0f, 0.0f, 0.0f);
	  else
	    someVector = vec3(0.0f, 1.0f, 0.0f);

	  binormal = cross(tangent, someVector);
	  normPrime = cross(tangent, binormal);
	  tanPrime = cross(binormal, normPrime);
 	}

	model = mat4(vec4(tanPrime, 0.0f),
				 vec4(normPrime, 0.0f),
				 vec4(binormal, 0.0f),
				 vec4(vectors[index], 1.0f));
	
	/*model = mat4(vec4(tanPrime.x, normPrime.x, binormal.x, vectors[index].x),
		         vec4(tanPrime.y, normPrime.y, binormal.y, vectors[index].y),
		         vec4(tanPrime.z, normPrime.z, binormal.z, vectors[index].z),
		         vec4(0.0f, 0.0f, 0.0f, 1.0f));*/

	return model;
}

void Track::getTracks(vector<vec3> vecs){
	vector<vec3> binormsLeft, binormsRight;
	vec3 bLeft, bRight, tangent, normal, binormal;
	GLuint previous = vecs.size() - 1;
	GLuint index = 0;
	GLuint next = 1;

	for (index = 0; index < vecs.size(); index++){
		tangent = (vecs[next] - vecs[index]) / length(vecs[next] - vecs[index]);

		if (length((vecs[next] - (2.0f * vecs[index]) + vecs[previous])) == 0){
		  vec3 someVector;
		  
		  if(tangent == vec3(0.0f, 1.0f, 0.0f))
		    someVector = vec3(1.0f, 0.0f, 0.0f);
		  else
		    someVector = vec3(0.0f, 1.0f, 0.0f);
		    
		  binormal = cross(tangent, someVector);
		  normal = cross(binormal, tangent);
		}
		else{
		  normal = (vecs[next] - (2.0f * vecs[index]) + vecs[previous]) / length((vecs[next] - (2.0f * vecs[index]) + vecs[previous]));
		  binormal = cross(tangent, normal);
		}

		bLeft = vecs[index] + (0.1f * binormal);
		bRight = vecs[index] - (0.1f * binormal);

		binormsLeft.push_back(bLeft);
		binormsRight.push_back(bRight);

		if(previous == vecs.size() - 1)
		  previous = 0;
		else
		  previous++;

		if(next + 1 == vecs.size())
		  next = vecs.size()-1;
		else
		  next++;
	}
	
	leftTrack = toFloat(binormsLeft);
	rightTrack = toFloat(binormsRight);
}

mat4 Track::updateCart(){
	GLuint next;

	if (posIndex + 1 == vectors.size())
		next = 0;
	else
		next = posIndex + 1;

	GLfloat displacement, change = speed * TIME_CHANGE;
	vec3 inter = vectors[next] - cartPos;
	GLfloat unit = length(vectors[next] - cartPos);

	if (length(vectors[next] - cartPos) > change){
		speed = calcSpeed(speed, posIndex, vectors[posIndex].y);
		change = speed * TIME_CHANGE;
		cartPos += change*(inter / unit);
	}
	else{
		displacement = length(cartPos - vectors[next]);

		if (posIndex + 1 == vectors.size())
			posIndex = 0;
		else
			posIndex += 1;

		if (posIndex + 1 == vectors.size())
			next = 0;
		else
			next = posIndex + 1;

		while ((displacement + length(vectors[next] - vectors[posIndex])) < change){
			displacement += length(vectors[next] - vectors[posIndex]);

			if (posIndex + 1 == vectors.size())
				posIndex = 0;
			else
				posIndex += 1;

			if (posIndex + 1 == vectors.size())
				next = 0;
			else
				next = posIndex + 1;
		}
		cartPos += (change - displacement) * (inter / unit);
	}			

	return getNewModel(posIndex, speed);
}

void Track::findMax(){
	max.x = vectors[0].x;
	max.y = vectors[0].y;
	max.z = vectors[0].z;

	for (int i = 0; i < vectors.size(); i++){
		if (vectors[i].x > max.x)
			max.x = vectors[i].x;

		if (vectors[i].y > max.y)
			max.y = vectors[i].y;

		if (vectors[i].z > max.z)
			max.z = vectors[i].z;
	}
}

void Track::findMin(){
	min.x = vectors[0].x;
	min.y = vectors[0].y;
	min.z = vectors[0].z;

	for (int i = 0; i < vectors.size(); i++){
		if (vectors[i].x < min.x)
			min.x = vectors[i].x;

		if (vectors[i].y < min.y)
			min.y = vectors[i].y;

		if (vectors[i].z < min.z)
			min.z = vectors[i].z;
	}
}

void Track::bufferWire(GLuint vertexLocation, GLuint normalLocation) {
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat)* vertices.size() + sizeof(GLfloat)* normals.size(),
		NULL,
		GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		sizeof(GLfloat)* vertices.size(),
		vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(GLfloat)* vertices.size(),
		sizeof(GLfloat)* normals.size(),
		normals.data());

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(normalLocation);
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(GLfloat)* vertices.size()));
}

void Track::bufferLeft(GLuint vertexLocation, GLuint normalLocation){
	glGenBuffers(1, &leftBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, leftBuffer);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat)* leftTrack.size(),
		NULL,
		GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		sizeof(GLfloat)* leftTrack.size(),
		leftTrack.data());

	glGenVertexArrays(1, &leftArray);
	glBindVertexArray(leftArray);
	glBindBuffer(GL_ARRAY_BUFFER, leftBuffer);

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
}
void Track::bufferRight(GLuint vertexLocation, GLuint normalLocation){
	glGenBuffers(1, &rightBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat)* rightTrack.size(),
		NULL,
		GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		sizeof(GLfloat)* rightTrack.size(),
		rightTrack.data());

	glGenVertexArrays(1, &rightArray);
	glBindVertexArray(rightArray);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
}
void Track::drawWire(GLuint ambient){
	//Hill Climb
	glUniform3f(ambient, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_LINE_STRIP, 0, segmentStarts[1] + 2);

	//Physics!
	glUniform3f(ambient, 1.0f, 0.0f, 0.0f);
	glDrawArrays(GL_LINE_STRIP, segmentStarts[1] + 1, segmentStarts[2] - segmentStarts[1]);

	//Deceleration
	glUniform3f(ambient, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINE_STRIP, segmentStarts[2], getNumVerts() - segmentStarts[2]);
}
void Track::drawLeft(){
	glBindVertexArray(leftArray);
	glDrawArrays(GL_LINE_STRIP, 0, leftTrack.size());
}
void Track::drawRight(){
	glBindVertexArray(rightArray);
	glDrawArrays(GL_LINE_STRIP, 0, rightTrack.size());
}
