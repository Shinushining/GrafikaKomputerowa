
#include <iostream>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// Nagłówki
#include <GL/glew.h>
#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

using namespace std;
// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core
in vec3 position;
in vec3 color;
in vec2 aTexCoord;
in vec3 aNormal;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
out vec3 Color;
void main(){
	Color = color;
	TexCoord=aTexCoord;
	gl_Position = proj*view*model*vec4(position, 1.0);
	FragPos=vec3(model*vec4(position,1.0));
	Normal=aNormal;

}
)glsl";

const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
out vec4 outColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{

//ambient 
float ambientStrength = 0.7;
vec3 ambientlightColor = vec3(1.0,1.0,1.0);
vec4 ambient = ambientStrength * vec4(ambientlightColor,1.0);

//diff
float diffuseStrength = 1.0;
vec3 difflightColor = vec3(1.0,1.0,1.0);
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);
float diff = max(dot(norm,lightDir), 0.0);
vec3 diffvec = diff*difflightColor*diffuseStrength;
vec4 diffuse = vec4(diffvec, 1.0);

//spec
float specularStrength = 1.0;
vec3 specularlightColor = vec3(1.0, 1.0, 1.0);

vec3 viewDir = normalize(camPos-FragPos);
vec3 reflectDir = reflect(-lightDir,norm);
float spec = pow(max(dot(viewDir,reflectDir),0.0),32);

vec4 specular = specularStrength * spec * vec4(specularlightColor,1.0);

outColor = (ambient+diffuse+specular)*texture(texture1, TexCoord);

//outColor=texture(texture1,TexCoord);
//outColor = vec4(Color, 1.0);
//outColor = vec4(1.0,1.0,1.0,1.0);
}
)glsl";

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;

int lastX, lastY;
double yaw = -90;
double pitch = 0;

float obrot = 0;

void kostka(int buffer) {
	int punkty = 36;

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,


		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f


	};

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * punkty * 8, vertices, GL_STATIC_DRAW);
}

void ustawKamereMysz(GLint _uniView, float _elapsedTime, const sf::Window& _window) {
	sf::Vector2i localPosition = sf::Mouse::getPosition(_window);

	sf::Vector2i position;
	bool relocation = false;

	//"przeskakiwanie" na drugą stronę okna w razie wyjścia poza granicę
	if (localPosition.x <= 0) {
		position.x = _window.getSize().x;
		position.y = localPosition.y;
		relocation = true;
	}
	if (localPosition.x >= _window.getSize().x - 1) {
		position.x = 0;
		position.y = localPosition.y;
		relocation = true;
	}
	if (localPosition.y <= 0) {
		position.y = _window.getSize().y;
		position.x = localPosition.x;
		relocation = true;
	}
	if (localPosition.y >= _window.getSize().y - 1) {
		position.y = 0;
		position.x = localPosition.x;
		relocation = true;
	}

	if (relocation) {
		sf::Mouse::setPosition(position, _window);
		firstMouse = true;
	}
	localPosition = sf::Mouse::getPosition(_window);

	if (firstMouse) {
		lastX = localPosition.x;
		lastY = localPosition.y;
		firstMouse = false;
	}

	double xoffset = localPosition.x - lastX;
	double yoffset = localPosition.y - lastY;
	lastX = localPosition.x;
	lastY = localPosition.y;

	double sensitivity = 0.001f;
	double cameraSpeed = 0.005f * _elapsedTime;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset * cameraSpeed;
	pitch -= yoffset * cameraSpeed;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void ustawKamereKlawisze(GLint _uniView, float _time) {
	float cameraSpeed = 0.000002f * _time;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		/*obrot -= cameraSpeed;
		cameraFront.x = sin(obrot);
		cameraFront.z = -cos(obrot);*/
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		/*obrot += cameraSpeed;
		cameraFront.x = sin(obrot);
		cameraFront.z = -cos(obrot);*/
	}

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void StereoProjection(GLuint shaderProgram_, float _left, float _right, float _bottom, float _top, float _near, float _far, float _zero_plane, float _dist, float _eye)
{
	float   _dx = _right - _left;
	float   _dy = _top - _bottom;

	float   _xmid = (_right + _left) / 2.0;
	float   _ymid = (_top + _bottom) / 2.0;

	float   _clip_near = _dist + _zero_plane - _near;
	float   _clip_far = _dist + _zero_plane - _far;

	float  _n_over_d = _clip_near / _dist;

	float   _topw = _n_over_d * _dy / 2.0;
	float   _bottomw = -_topw;
	float   _rightw = _n_over_d * (_dx / 2.0 - _eye);
	float   _leftw = _n_over_d * (-_dx / 2.0 - _eye);

	// Create a fustrum, and shift it off axis
	glm::mat4 proj = glm::frustum(_leftw, _rightw, _bottomw, _topw, _clip_near, _clip_far);

	proj = glm::translate(proj, glm::vec3(-_xmid - _eye, -_ymid, 0));

	GLint uniProj = glGetUniformLocation(shaderProgram_, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

void LoadModelOBJ(int& punkty, const char* filename, int buffer) {
	int vert_num = 0;
	int trian_num = 0;

	ifstream myReadFile;
	myReadFile.open(filename);
	string output;

	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> output;
			if (output == "v") vert_num++;
			if (output == "f") trian_num++;
		}
	}
	myReadFile.close();
	myReadFile.open(filename);
	float** vert;
	vert = new float* [vert_num];
	for (int i = 0; i < vert_num; i++) {
		vert[i] = new float[3];
	}
	int** trian;
	trian = new int* [trian_num];
	for (int i = 0; i < trian_num; i++) {
		trian[i] = new int[3];
	}

	int licz_vert = 0;
	int licz_trian = 0;

	while (!myReadFile.eof()) {
		myReadFile >> output;
		if (output == "v") {
			myReadFile >> vert[licz_vert][0];
			myReadFile >> vert[licz_vert][1];
			myReadFile >> vert[licz_vert][2];
			licz_vert++;
		}
		if (output == "f") {
			myReadFile >> trian[licz_trian][0];
			myReadFile >> trian[licz_trian][1];
			myReadFile >> trian[licz_trian][2];
			licz_trian++;
		}
		output.clear();
	}

	GLfloat* vertices = new GLfloat[trian_num * 9];
	int vert_current = 0;
	for (int i = 0; i < trian_num; i++) {
		vertices[vert_current] = vert[trian[i][0] - 1][0];
		vertices[vert_current + 1] = vert[trian[i][0] - 1][1];
		vertices[vert_current + 2] = vert[trian[i][0] - 1][2];

		vertices[vert_current + 3] = vert[trian[i][1] - 1][0];
		vertices[vert_current + 4] = vert[trian[i][1] - 1][1];
		vertices[vert_current + 5] = vert[trian[i][1] - 1][2];

		vertices[vert_current + 6] = vert[trian[i][2] - 1][0];
		vertices[vert_current + 7] = vert[trian[i][2] - 1][1];
		vertices[vert_current + 8] = vert[trian[i][2] - 1][2];

		vert_current += 9;
	}
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * trian_num * 9, vertices, GL_STATIC_DRAW);

	punkty = trian_num * 9;

	delete[] vertices;
	for (int i = 0; i < vert_num; i++) {
		delete[] vert[i];
	}
	delete[] vert;
	for (int i = 0; i < trian_num; i++) {
		delete[] trian[i];
	}
	delete[] trian;
}

void LoadModelOBJ_EBO(int& punkty, const char* filename, int buffer_vbo, int buffer_ebo) {
	int vert_num = 0;
	int trian_num = 0;

	ifstream myReadFile;
	myReadFile.open(filename);
	string output;

	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> output;
			if (output == "v") vert_num++;
			if (output == "f") trian_num++;
		}
	}

	myReadFile.close();
	myReadFile.open(filename);
	float* vert;
	vert = new float[vert_num * 3];
	int* element;
	element = new int[trian_num * 3];

	int licz_vert = 0;
	int licz_element = 0;
	int temp = 0;

	while (!myReadFile.eof()) {
		myReadFile >> output;
		if (output == "v") {
			myReadFile >> vert[licz_vert];
			myReadFile >> vert[licz_vert + 1];
			myReadFile >> vert[licz_vert + 2];
			licz_vert += 3;
		}
		if (output == "f") {
			myReadFile >> temp;
			temp--;
			element[licz_element] = temp;
			myReadFile >> temp;
			temp--;
			element[licz_element + 1] = temp;
			myReadFile >> temp;
			temp--;
			element[licz_element + 2] = temp;
			licz_element += 3;
		}
		output.clear();
	}
	glBindBuffer(GL_ARRAY_BUFFER, buffer_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vert_num * 3, vert, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * trian_num * 3, element, GL_STATIC_DRAW);
	punkty = trian_num * 3;
	delete[] vert;
	delete[] element;
}

bool LoadModelOBJNormalsCoord(int& punkty_, const char* filename, int buffer, int* tabtekstur)
{
	int vert_num = 0;
	int triangles = 0;
	int normals = 0;
	int coord_num = 0;

	std::ifstream myReadFile;
	myReadFile.open(filename);
	std::string output;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> output;
			if (output == "v") vert_num++;
			if (output == "f") triangles++;
			if (output == "vn") normals++;
			if (output == "vt") coord_num++;
		}
	}

	myReadFile.close();
	myReadFile.open(filename);


	float** vert;
	vert = new float* [vert_num]; //przydzielenie pamięci na w wierszy

	for (int i = 0; i < vert_num; i++)
		vert[i] = new float[3];


	int** trian;
	trian = new int* [triangles]; //przydzielenie pamięci na w wierszy

	for (int i = 0; i < triangles; i++)
		trian[i] = new int[9];

	float** norm;
	norm = new float* [normals]; //przydzielenie pamięci na w wierszy

	for (int i = 0; i < normals; i++)
		norm[i] = new float[3];

	float** coord;
	coord = new float* [coord_num]; //przydzielenie pamięci na w wierszy

	for (int i = 0; i < coord_num; i++)
		coord[i] = new float[2];

	int licz_vert = 0;
	int licz_triang = 0;
	int licz_normals = 0;
	int licz_coord = 0;
	int count = 0;
	int temp = 0;


	while (!myReadFile.eof()) {
		output = "";
		myReadFile >> output;
		if (output == "vn") { myReadFile >> norm[licz_normals][0]; myReadFile >> norm[licz_normals][1]; myReadFile >> norm[licz_normals][2]; licz_normals++; }
		if (output == "v") { myReadFile >> vert[licz_vert][0]; myReadFile >> vert[licz_vert][1]; myReadFile >> vert[licz_vert][2]; licz_vert++; }
		if (output == "vt") { myReadFile >> coord[licz_coord][0]; myReadFile >> coord[licz_coord][1]; licz_coord++; }
		if (output == "o") { tabtekstur[count] = temp * 3; temp = 0; count++; }
		if (output == "f") {

			for (int i = 0; i < 9; i += 3)
			{
				std::string s;
				myReadFile >> s;
				std::stringstream ss(s);

				std::vector <std::string> el;
				std::string item;


				while (getline(ss, item, '/')) {
					el.push_back(item);
				}
				trian[licz_triang][i] = std::stoi(el[0]);
				trian[licz_triang][i + 1] = std::stoi(el[1]);
				trian[licz_triang][i + 2] = std::stoi(el[2]);


			}
			temp++;
			licz_triang++;
		}
	}
	tabtekstur[count] = temp * 3;
	GLfloat* vertices = new GLfloat[triangles * 24];

	int vert_current = 0;

	for (int i = 0; i < triangles; i++)
	{
		vertices[vert_current] = vert[trian[i][0] - 1][0];
		vertices[vert_current + 1] = vert[trian[i][0] - 1][1];
		vertices[vert_current + 2] = vert[trian[i][0] - 1][2];
		vertices[vert_current + 3] = norm[trian[i][2] - 1][0];
		vertices[vert_current + 4] = norm[trian[i][2] - 1][1];
		vertices[vert_current + 5] = norm[trian[i][2] - 1][2];
		vertices[vert_current + 6] = coord[trian[i][1] - 1][0];
		vertices[vert_current + 7] = coord[trian[i][1] - 1][1];

		vertices[vert_current + 8] = vert[trian[i][3] - 1][0];
		vertices[vert_current + 9] = vert[trian[i][3] - 1][1];
		vertices[vert_current + 10] = vert[trian[i][3] - 1][2];
		vertices[vert_current + 11] = norm[trian[i][5] - 1][0];
		vertices[vert_current + 12] = norm[trian[i][5] - 1][1];
		vertices[vert_current + 13] = norm[trian[i][5] - 1][2];
		vertices[vert_current + 14] = coord[trian[i][4] - 1][0];
		vertices[vert_current + 15] = coord[trian[i][4] - 1][1];

		vertices[vert_current + 16] = vert[trian[i][6] - 1][0];
		vertices[vert_current + 17] = vert[trian[i][6] - 1][1];
		vertices[vert_current + 18] = vert[trian[i][6] - 1][2];
		vertices[vert_current + 19] = norm[trian[i][8] - 1][0];
		vertices[vert_current + 20] = norm[trian[i][8] - 1][1];
		vertices[vert_current + 21] = norm[trian[i][8] - 1][2];
		vertices[vert_current + 22] = coord[trian[i][7] - 1][0];
		vertices[vert_current + 23] = coord[trian[i][7] - 1][1];

		vert_current += 24;
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * triangles * 24, vertices, GL_STATIC_DRAW);

	punkty_ = triangles * 3;

	delete vertices;




	for (int i = 0; i < vert_num; i++)
		delete[] vert[i];
	delete[] vert;

	for (int i = 0; i < triangles; i++)
		delete[] trian[i];
	delete[] trian;

	for (int i = 0; i < normals; i++)
		delete[] norm[i];
	delete[] norm;

	for (int i = 0; i < coord_num; i++)
		delete[] coord[i];
	delete[] coord;

	return 0;
}

//void okrag(int boki, int buffer) {
//	if (boki < 3) return;
//	
//	GLfloat* vertices = new GLfloat[boki * 6];
//
//	float dlkata = 2 * 3.1415 / boki;
//	float kat = 0;
//	float promien = 1;
//
//	for (int i = 0; i < boki * 6; i += 6) {
//
//		vertices[i] = promien * cos(kat);
//		vertices[i + 1] = promien * sin(kat);
//		vertices[i + 2] = 0;
//
//		vertices[i + 3] = (cos(kat) + 1) / 2.0;
//		vertices[i + 4] = (sin(kat) + 1) / 2.0;
//		vertices[i + 5] = (cos(kat) + 1) / 2.0;
//		kat += dlkata;
//	}
//	glBindBuffer(GL_ARRAY_BUFFER, buffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * boki * 6, vertices, GL_STATIC_DRAW);
//
//	delete vertices;
//}

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	// Okno renderingu
	sf::Window window(sf::VideoMode(800, 800, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);
	//sf::Window window(sf::VideoMode(800, 600, 32), "OpenGL", sf::Style::Fullscreen | sf::Style::Close, settings);


	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);

	// Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	glewInit();



	// Utworzenie VAO (Vertex Array Object)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Utworzenie VBO (Vertex Buffer Object)
	// i skopiowanie do niego danych wierzchołkowych
	GLuint vbo;
	glGenBuffers(1, &vbo);

	//Element buffer object
	GLuint ebo;
	glGenBuffers(1, &ebo);


	//int boki = 6;
	//okrag(boki, vbo);

	//kostka(vbo);

	int punkty = 0;
	int tabtekstur[5];

	LoadModelOBJNormalsCoord(punkty, "untitled.obj", vbo, tabtekstur);

	//LoadModelOBJ(punkty, "untitled.obj", vbo);

	//LoadModelOBJ_EBO(punkty, "test2.obj", vbo, ebo);

	// Utworzenie i skompilowanie shadera wierzchołków
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	//sprawdzanie poprawności kompilacji
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		std::cout << "vertexShader blad kompilacji" << endl;
		//zmienna na komunikat błędu, zakładamy, że nie będzie dłuższy niż 512 bajtów
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		std::cout << buffer << endl;
		return 1;
	}
	else {
		std::cout << "vertexShader kompilacja poprawna" << endl;
	}

	// Utworzenie i skompilowanie shadera fragmentów
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//sprawdzanie poprawności kompilacji
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		std::cout << "fragmentShader blad kompilacji" << endl;
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		std::cout << buffer << endl;
		return 1;
	}
	else {
		std::cout << "fragmentShader kompilacja poprawna" << endl;
	}

	// Zlinkowanie obu shaderów w jeden wspólny program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specifikacja formatu danych wierzchołkowych
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	//GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	//glEnableVertexAttribArray(colAttrib);
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	GLint NorAttrib = glGetAttribLocation(shaderProgram, "aNormal");
	glEnableVertexAttribArray(NorAttrib);
	glVertexAttribPointer(NorAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	GLint TexCoord = glGetAttribLocation(shaderProgram, "aTexCoord");
	glEnableVertexAttribArray(TexCoord);
	glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

	GLint uniView = glGetUniformLocation(shaderProgram, "view");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.06f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	GLint prymityw = GL_TRIANGLES;
	//GLint mouse_x = 0;
	//GLint mouse_y = 0;

	glm::vec3 lightPos(0.0f, 0.0f, 7.0f);
	GLint uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3fv(uniLightPos, 1, glm::value_ptr(lightPos));

	glEnable(GL_DEPTH_TEST);

	unsigned int texture1;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("cat1.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);


	unsigned int texture2;

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("karton.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	unsigned int texture3;

	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("crate.bmp", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	unsigned int texture4;

	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("metal.bmp", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture3"), 2);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture4"), 3);

	// Rozpoczęcie pętli zdarzeń
	bool running = true;
	double dist = 14.5;
	int tryb = 2;
	sf::Clock clock;

	window.setFramerateLimit(60);
	sf::Time time;


	int licznik = 0;
	while (running) {

		time = clock.restart();

		licznik++;
		float fFps = 1000000 / time.asMicroseconds();
		if (licznik > fFps) {
			window.setTitle(std::to_string(fFps));
			licznik = 0;
		}

		sf::Event windowEvent;
		while (window.pollEvent(windowEvent)) {
			switch (windowEvent.type) {
			case sf::Event::Closed:
				running = false;
				break;

			case sf::Event::KeyPressed:
				switch (windowEvent.key.code) {
				case sf::Keyboard::Escape:
					running = false;
					break;

				case sf::Keyboard::Num1:
					prymityw = GL_POINTS;
					break;
				case sf::Keyboard::Num2:
					prymityw = GL_LINES;
					break;
				case sf::Keyboard::Num3:
					prymityw = GL_LINE_STRIP;
					break;
				case sf::Keyboard::Num4:
					prymityw = GL_LINE_LOOP;
					break;
				case sf::Keyboard::Num5:
					prymityw = GL_TRIANGLES;
					break;
				case sf::Keyboard::Num6:
					prymityw = GL_TRIANGLE_STRIP;
					break;
				case sf::Keyboard::Num7:
					prymityw = GL_TRIANGLE_FAN;
					break;
				case sf::Keyboard::Num8:
					prymityw = GL_QUADS;
					break;
				case sf::Keyboard::Num9:
					prymityw = GL_QUAD_STRIP;
					break;
				case sf::Keyboard::Num0:
					prymityw = GL_POLYGON;
					break;
				case sf::Keyboard::N:
					dist += 0.01;
					break;
				case sf::Keyboard::M:
					dist -= 0.01;
					break;
				case sf::Keyboard::I:
					tryb = 0;
					break;
				case sf::Keyboard::O:
					tryb = 1;
					break;
				case sf::Keyboard::P:
					tryb = 2;
					break;
				}
				break;

			case sf::Event::MouseMoved:
				ustawKamereMysz(uniView, time.asMicroseconds(), window);
				GLint uniCamPos = glGetUniformLocation(shaderProgram, "camPos");
				glUniform3fv(uniCamPos, 1, glm::value_ptr(cameraPos));
				break;
			}
		}

		ustawKamereKlawisze(uniView, time.asMicroseconds());
		GLint uniCamPos = glGetUniformLocation(shaderProgram, "camPos");
		glUniform3fv(uniCamPos, 1, glm::value_ptr(cameraPos));

		// Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Narysowanie wielokąta na podstawie dowolnej ilości wierzchołków
		//glDrawArrays(prymityw, 0, boki);
		//glDrawArrays(prymityw, 0, 36);

		//ustawianie wielu tekstur
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture2);
		//glDrawArrays(GL_TRIANGLES, 0, 24);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glDrawArrays(GL_TRIANGLES, 24, 36);

		switch (tryb) {
		case 0:
			glViewport(0, 0, window.getSize().x, window.getSize().y);
			glDrawBuffer(GL_BACK_LEFT);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, dist, -0.05);
			glColorMask(true, false, false, false);

			//glBindTexture(GL_TEXTURE_2D, texture2);
			//glDrawArrays(prymityw, 0, 24);
			//glBindTexture(GL_TEXTURE_2D, texture1);
			//glDrawArrays(prymityw, 24, 36);

			glDrawArrays(prymityw, 0, punkty);
			glClear(GL_DEPTH_BUFFER_BIT);

			glDrawBuffer(GL_BACK_RIGHT);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, dist, 0.05);
			glColorMask(false, false, true, false);

			//glBindTexture(GL_TEXTURE_2D, texture2);
			//glDrawArrays(prymityw, 0, 24);
			//glBindTexture(GL_TEXTURE_2D, texture1);
			//glDrawArrays(prymityw, 24, 36);

			glDrawArrays(prymityw, 0, punkty);
			glColorMask(true, true, true, true);
			break;

		case 1:
			glViewport(0, 0, window.getSize().x / 2, window.getSize().y);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, 13, -0.05);

			//glBindTexture(GL_TEXTURE_2D, texture2);
			//glDrawArrays(prymityw, 0, 24);
			//glBindTexture(GL_TEXTURE_2D, texture1);
			//glDrawArrays(prymityw, 24, 36);

			glDrawArrays(prymityw, 0, punkty);

			glViewport(window.getSize().x / 2, 0, window.getSize().x / 2, window.getSize().y);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, 13, 0.05);

			//glBindTexture(GL_TEXTURE_2D, texture2);
			//glDrawArrays(prymityw, 0, 24);
			//glBindTexture(GL_TEXTURE_2D, texture1);
			//glDrawArrays(prymityw, 24, 36);

			glDrawArrays(prymityw, 0, punkty);
			break;
		case 2:
			glActiveTexture(GL_TEXTURE0);
			glViewport(0, 0, window.getSize().x, window.getSize().y);

			glBindTexture(GL_TEXTURE_2D, texture1);
			glDrawArrays(prymityw, 0, tabtekstur[1]);

			glBindTexture(GL_TEXTURE_2D, texture2);
			glDrawArrays(prymityw, tabtekstur[1], tabtekstur[2]);

			glBindTexture(GL_TEXTURE_2D, texture3);
			glDrawArrays(prymityw, tabtekstur[1] + tabtekstur[2], tabtekstur[3]);

			glBindTexture(GL_TEXTURE_2D, texture4);
			glDrawArrays(prymityw, tabtekstur[1] + tabtekstur[2] + tabtekstur[3], tabtekstur[4]);

			break;
		}

		// Wymiana buforów tylni/przedni
		window.display();
	}
	// Kasowanie programu i czyszczenie buforów
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	// Zamknięcie okna renderingu
	window.close();
	return 0;
}