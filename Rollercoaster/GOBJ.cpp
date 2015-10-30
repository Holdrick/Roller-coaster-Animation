#include "GOBJ.h"


#define VERTEX_DATA 0
#define NORMAL_DATA 1

GOBJ::GOBJ(string filename){
	setupObj(filename);
}

GOBJ::~GOBJ(){
	glDeleteVertexArrays(1, &VAO);
}

void GOBJ::readObj(string filename) {
	char ch;
	string str;
	GLfloat f;
	GLuint i;
	ifstream reader(filename.c_str());

	if (reader.is_open()) {
		while (!reader.eof()) {
			reader >> str;

			if (str == "v") {
				reader >> f;
				vertices.push_back(f);

				reader >> f;
				vertices.push_back(f);

				reader >> f;
				vertices.push_back(f);

				vertices.push_back(1.0f);
			}
			if (str == "vn"){
				reader >> f;
				normals.push_back(f);

				reader >> f;
				normals.push_back(f);

				reader >> f;
				normals.push_back(f);
			}
			if (str == "vt"){
				reader >> f;
				texVertices.push_back(f);

				reader >> f;
				texVertices.push_back(f);
			}
			if (str == "f") {
				reader >> i;
				indices.push_back(i - 1);
				reader >> ch;
				reader >> i;
				texIndices.push_back(i - 1);
				reader >> ch;
				reader >> i;
				normIndices.push_back(i - 1);

				reader >> i;
				indices.push_back(i - 1);
				reader >> ch;
				reader >> i;
				texIndices.push_back(i - 1);
				reader >> ch;
				reader >> i;
				normIndices.push_back(i - 1);

				reader >> i;
				indices.push_back(i - 1);
				reader >> ch;
				reader >> i;
				texIndices.push_back(i - 1);
				reader >> ch;
				reader >> i;
				normIndices.push_back(i - 1);
			}
			str = "";
		}
		reader.close();
	}
	else{
		cout << "Couldn't open file \"" << filename << "\"" << endl;
	}
}

void GOBJ::setupObj(string filename){
	readObj(filename);
	findCenter();
	rearrangeData();
}

void GOBJ::rearrangeData()
{
	vector<GLfloat> verts, norms, tex;
	GLuint vi, ni, ti;

	for (GLuint i = 0; i < indices.size(); i++){
		vi = indices[i] * 4;
		verts.push_back(vertices[vi]);
		verts.push_back(vertices[vi + 1]);
		verts.push_back(vertices[vi + 2]);
		verts.push_back(vertices[vi + 3]);

		ni = normIndices[i] * 3;
		norms.push_back(normals[ni]);
		norms.push_back(normals[ni + 1]);
		norms.push_back(normals[ni + 2]);

		ti = texIndices[i] * 2;
		tex.push_back(texVertices[ti]);
		tex.push_back(texVertices[ti + 1]);
	}

	vertices = verts;
	normals = norms;
	texVertices = tex;

}

void GOBJ::buffer()
{
	GLuint offset = 0;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize(), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		offset,
		getSize(VERTEX_DATA),
		getData(VERTEX_DATA));

	offset += getSize(VERTEX_DATA);

	glBufferSubData(GL_ARRAY_BUFFER,
		offset,
		getSize(NORMAL_DATA),
		getData(NORMAL_DATA));

	offset = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(VERTEX_DATA);
	glEnableVertexAttribArray(NORMAL_DATA);

	glVertexAttribPointer(VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)offset);

	offset += getSize(VERTEX_DATA);

	glVertexAttribPointer(NORMAL_DATA, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)offset);

	offset += getSize(NORMAL_DATA);
}

void GOBJ::draw(mat4 model, vec3 position, GLuint mv)
{
	model = translate(model, position - center);
	glUniformMatrix4fv(mv, 1, GL_FALSE, value_ptr(model));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, indices.size());
}

GLuint GOBJ::bufferSize()
{
	return sizeof(GLfloat)* (vertices.size() + normals.size());
}

GLfloat* GOBJ::getData(GLuint type)
{
	if (type == 0)
		return vertices.data();
	else if (type == 1)
		return normals.data();
	else
		return texVertices.data();
}

GLuint GOBJ::getSize(GLuint type)
{
	if (type == 0)
		return sizeof(GLfloat)* vertices.size();
	else if (type == 1)
		return sizeof(GLfloat)* normals.size();
	else
		return sizeof(GLfloat)* texVertices.size();
}

void GOBJ::findCenter()
{
	vec3 diff, center;
	vec3 max = findMax();
	vec3 min = findMin();

	center.x = (max.x - min.x) / 2;
	center.y = (max.y - min.y) / 2;
	center.z = (max.z - min.z) / 2;
}

vec3 GOBJ::findMax(){
	vec3 max;

	max.x = vertices[0];
	max.y = vertices[1];
	max.z = vertices[2];

	for (GLuint i = 4; i < vertices.size(); i += 4){
		if (vertices[i] > max.x)
			max.x = vertices[i];

		if (vertices[i + 1] > max.y)
			max.y = vertices[i + 1];

		if (vertices[i + 2] > max.z)
			max.z = vertices[i + 2];
	}

	return max;
}

vec3 GOBJ::findMin(){
	vec3 min;

	min.x = vertices[0];
	min.y = vertices[1];
	min.z = vertices[2];

	for (GLuint i = 4; i < vertices.size(); i += 4){
		if (vertices[i] < min.x)
			min.x = vertices[i];

		if (vertices[i + 1] < min.y)
			min.y = vertices[i + 1];

		if (vertices[i + 2] < min.z)
			min.z = vertices[i + 2];
	}

	return min;
}
