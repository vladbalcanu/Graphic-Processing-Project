//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include <chrono>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>

#include <iostream>
using namespace std;

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

GLuint shadowMapFBO;
GLuint depthMapTexture;
float fogDensity = 0.006f;
GLuint fogDensityLoc;

float constant = 1.0f;
float linear = 1.5f;
float quadratic = 3.4f;
bool lightSpike = true;

float lightColorX = 1.0f;
float lightColorY = 1.0f;
float lightColorZ = 1.0f;

bool PRESENTATION = false;
bool PRESmovement = false;




GLuint constantLoc;
GLuint linearLoc;
GLuint quadraticLoc;
glm::vec3 positionPointLight = glm::vec3(4.4, -0.3 , 8.70);
glm::vec3 positionPointLight2 = glm::vec3(-0.0,-0.8,0.0);
GLuint positionLocPointLight;
GLuint positionLocPointLight2;

gps::Camera myCamera(
				glm::vec3(0.0f, 2.0f, 5.5f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.05f;
float pitch = 0.0f;
float yaw = -90.0f;

bool pressedKeys[1024];
float angleY = 0.0f;
float treeAngle = 0.0f;
float handAngleX = 0.0f;
float handAngleY = 0.0f;
float handAngleZ = 0.0f;
float wingCount = 0.0f;
float wingAngle = 0.0f;
float dragonAngle = 0.0f;
float dragonCount = 0.0f;
int handMode = 0;
GLfloat lightAngle;
gps::Model3D scene;
gps::Model3D movingTrees;
gps::Model3D monsterHand;
gps::Model3D Moon;
gps::Model3D wing;
gps::Model3D wing1;
gps::Model3D dragon;

gps::Shader myCustomShader;
gps::Shader depthMapShader;

bool showDepthMap;
bool firstMouse = true;
float lastX = 1920.0f / 2.0f;
float lastY = 1070.0f / 2.0f;


GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (PRESmovement == 0) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		double xnewPos = xpos - lastX;
		double ynewPos = lastY - ypos;

		xnewPos *= 0.0005f;
		ynewPos *= 0.0005f;

		yaw += xnewPos;
		pitch += ynewPos;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		myCamera.rotate(pitch, yaw);
	}
}

long anim;
void processMovement()
{
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}
	if (pressedKeys[GLFW_KEY_O]) {
		handAngleX -= 0.5f;
	}

	if (pressedKeys[GLFW_KEY_P]) {
		handAngleX += 0.5f;
	}
	if (PRESmovement==0) {
		if (pressedKeys[GLFW_KEY_W]) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}

		if (pressedKeys[GLFW_KEY_S]) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}

		if (pressedKeys[GLFW_KEY_A]) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}

		if (pressedKeys[GLFW_KEY_D]) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}

		if (pressedKeys[GLFW_KEY_R]) {
			myCamera.move(gps::MOVE_UP, cameraSpeed);
		}

		if (pressedKeys[GLFW_KEY_F]) {
			myCamera.move(gps::MOVE_DOWN, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_UP]) {
			pitch += 1.0f;
			myCamera.rotate(pitch, yaw);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

		}
		if (pressedKeys[GLFW_KEY_DOWN]) {
			pitch -= 1.0f;
			myCamera.rotate(pitch, yaw);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}
		if (pressedKeys[GLFW_KEY_RIGHT]) {
			yaw += 1.0f;
			myCamera.rotate(pitch, yaw);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}
		if (pressedKeys[GLFW_KEY_LEFT]) {
			yaw -= 1.0f;
			myCamera.rotate(pitch, yaw);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}
	}
	//NORMAL
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//WIREFRAME
	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	//POINT
	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}


	if (pressedKeys[GLFW_KEY_Z])
	{
		if (fogDensity < 1)
			fogDensity += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_X])
	{
		if (fogDensity > 0)
			fogDensity -= 0.001f;

	}

	if (pressedKeys[GLFW_KEY_Q])
	{
		linear += 0.1f;
		quadratic += 0.1f;

	}

	if (pressedKeys[GLFW_KEY_E])
	{
		linear -= 0.1f;
		quadratic -= 0.1f;
	}
	if (pressedKeys[GLFW_KEY_N]) {
		if (PRESENTATION == false) {
			PRESENTATION = true;
			anim = clock();
		}
		else {
			PRESENTATION = false;
		}
	}
	if (pressedKeys[GLFW_KEY_B]) {
		PRESmovement = false;
	}
	
}

void PRESENT() {
	long end ;
	long dif ;
	if (PRESENTATION == true) {
		myCamera.StartAnimation(glm::vec3(0.5f, -0.75f, 8.0f));
		yaw = -90.0f;
		pitch = 0.0f;
		myCamera.rotate(pitch,yaw);
		PRESmovement = true;
		PRESENTATION = false;
		end = clock();
	}
	if (PRESmovement == true)
	{	
		end = clock();
		dif = end - anim;
		cout << "DIFERENTA" << dif << endl;
		if(dif >= 0 && dif <= 1200)
		myCamera.move(gps::MOVE_FORWARD,cameraSpeed);
		if (dif >= 1200 && dif <= 3750) {
			yaw += 0.325f;
			myCamera.rotate(pitch,yaw);
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 4.0f);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 3.5f);
		}
		if (dif >= 3750 && dif <= 4250) {
			yaw += 1.00f;
			myCamera.rotate(pitch, yaw);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 5.0f);
		}
		if (dif >= 4250 && dif <= 5400) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2.5f);
			myCamera.move(gps::MOVE_UP, cameraSpeed / 10.0f);
		}
		if (dif >= 5400 && dif <= 6750) {
			yaw += 1.0f;
			myCamera.rotate(pitch, yaw);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2.5f);
		}
		if (dif >= 6750 && dif <= 9000) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 5.0f);
			myCamera.move(gps::MOVE_UP, cameraSpeed / 5.0f);
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed / 5.0f);
			yaw += 0.4f;
			pitch -= 0.2f;
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 9000 && dif <= 12250) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 5.0f);
			myCamera.move(gps::MOVE_UP, cameraSpeed / 7.5f);
			pitch -= 0.05f;
			yaw += 0.1f;
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 12250 && dif <= 20500) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2.5f);
			myCamera.move(gps::MOVE_DOWN, cameraSpeed / 5.0f);
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed / 2.5f);
			pitch += 0.05f;
			yaw += 0.1f;
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 20500 && dif <= 22250) {
			pitch += 0.05f;
			myCamera.move(gps::MOVE_DOWN, cameraSpeed / 3.5f);
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 5.0f);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2.5f);
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 22250 && dif <= 25000) {
			yaw += 0.1f;
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2.5f);
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 25000 && dif <= 26500) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);
			yaw += 0.35f;
			myCamera.rotate(pitch, yaw);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2.5f);
		}
		if (dif >= 26500 && dif <= 27500) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);
			myCamera.move(gps::MOVE_DOWN, cameraSpeed / 5.0f);
		
		}
		if (dif >= 27500 && dif <= 29000) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);
		}
		if(dif >= 29000 && dif <= 32500) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);
			myCamera.move(gps::MOVE_UP, cameraSpeed / 2.5f);
			yaw -= 0.25f;
			myCamera.rotate(pitch, yaw);

		}
		if (dif >= 32500 && dif <=  35700) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);

		}
		if (dif >= 35700 && dif <= 36500) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);
			myCamera.move(gps::MOVE_UP, cameraSpeed / 4.0f);
			pitch += 0.1f;
			yaw += 1.0f;
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 36500 && dif <= 38000) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2.5f);
			myCamera.move(gps::MOVE_UP, cameraSpeed / 7.0f);
		}
		if (dif >= 38000 && dif <= 44000) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed / 2.5f);
			pitch -= 0.01f;
			yaw += 1.0f;
			myCamera.rotate(pitch, yaw);
		}
		if (dif >= 44000) {
			PRESmovement = false;
		}
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {

	scene.LoadModel("objects/balcanu/scena.obj");
	movingTrees.LoadModel("objects/balcanu/movingTrees.obj");
	monsterHand.LoadModel("objects/balcanu/MonsterHand.obj");
	Moon.LoadModel("objects/balcanu/Moon.obj");
	wing.LoadModel("objects/balcanu/wing.obj");
	wing1.LoadModel("objects/balcanu/wing1.obj");
	dragon.LoadModel("objects/balcanu/dragon.obj");

}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depth.VERT", "shaders/depth.FRAG");
	depthMapShader.useShaderProgram();
}


void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(20.0f,14.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)); 
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(lightColorX, lightColorY, lightColorZ); //redish light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, fogDensity);

	constantLoc = glGetUniformLocation(myCustomShader.shaderProgram, "constant");
	glUniform1f(constantLoc, constant); //constant
	linearLoc = glGetUniformLocation(myCustomShader.shaderProgram, "linear");
	glUniform1f(linearLoc, linear); //linear
	quadraticLoc = glGetUniformLocation(myCustomShader.shaderProgram, "quadratic");
	glUniform1f(quadraticLoc, quadratic); //quadratic
	positionLocPointLight = glGetUniformLocation(myCustomShader.shaderProgram, "positionPointLight");
	glUniform3fv(positionLocPointLight, 1, glm::value_ptr(positionPointLight));
	constantLoc = glGetUniformLocation(myCustomShader.shaderProgram, "constant");
	glUniform1f(constantLoc, constant); //constant
	linearLoc = glGetUniformLocation(myCustomShader.shaderProgram, "linear");
	glUniform1f(linearLoc, linear); //linear
	quadraticLoc = glGetUniformLocation(myCustomShader.shaderProgram, "quadratic");
	glUniform1f(quadraticLoc, quadratic); //quadratic
	positionLocPointLight2 = glGetUniformLocation(myCustomShader.shaderProgram, "positionPointLight2");
	glUniform3fv(positionLocPointLight2, 1, glm::value_ptr(positionPointLight2));


	//depthMapShader.useShaderProgram();

}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 35.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void wingMovement() {
	wingCount += 0.05f;
	dragonAngle += 0.5f;
	wingAngle = glm::sin(wingCount)*10;
}

void handMovement() {
	if (handMode == 0) {
		handAngleX -= 0.05f;
		handAngleY -= 0.01f;
		handAngleZ -= 0.1f;
		if (handAngleX <= -45.3996f)
			handMode = 1;
	}
	if (handMode == 1) {
		handAngleZ += 0.1f;
		if (handAngleZ >= 0.0f)
			handMode = 2;
	}
	if (handMode == 2) {
		if (handAngleX <=0.0f)
		handAngleX += 0.1f;
		if (handAngleY <= 0.0f)
		handAngleY += 0.05f;
		if (handAngleZ >= 0.0f && handAngleX >= 0.0f && handAngleY >= 0.0f) {
			handMode = 0;
			handAngleX = 0.0f;
			handAngleY = 0.0f;
			handAngleZ = 0.0f;
		}
	}
}

void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	treeAngle += 0.01f;
	float rotationAngle = glm::sin(treeAngle);
	if (rotationAngle <= 0)
		rotationAngle *= 4.0;
	else
		rotationAngle *= 2.0;
	handMovement();
	scene.Draw(shader);
	model = glm::translate(model, glm::vec3(+4.0f, 0.0f, 17.0f));
	model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-4.0f, 0.0f, -17.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	movingTrees.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::translate(model, glm::vec3(-7.45622f, +13.2919f, -25.0f));
	model = glm::rotate(model, glm::radians(handAngleY), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(handAngleZ), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(handAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(+7.45622f, -13.2919f, 25.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	monsterHand.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	Moon.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, glm::radians(dragonAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(14.0f, 11.6215f, +4.72934f));
	model = glm::rotate(model, glm::radians(wingAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-14.0f, -11.6215f, -4.72934f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	wing.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, glm::radians(dragonAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(13.6041f, 11.5078f, +4.88078f));
	model = glm::rotate(model, glm::radians(-wingAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-13.6041f, -11.5078f, -4.88078f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	wing1.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, glm::radians(dragonAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	dragon.Draw(shader);

}
long start = clock();
void lighting() {
	long end = clock();
	long dif = end - start;
	if (dif > 30000) {
		if (dif % 15000 >= 0 && dif % 15000 <= 100) {
			lightColorX = 0.796f;
			lightColorY = 0.764f;
			lightColorZ = 0.89f;
		}
		if (dif % 15000 > 200 && dif % 15000 <= 300) {
			lightColorX = 1.0f;
			lightColorY = -0.4f;
			lightColorZ = -0.4f;
			
		}
		if (dif % 15000 >= 400 && dif % 15000 <= 500) {
			lightColorX = 0.796f;
			lightColorY = 0.764f;
			lightColorZ = 0.89f;
		}
		if (dif % 15000 >= 600 && dif % 15000 <= 700) {
			lightColorX = 1.0f;
			lightColorY = -0.4f;
			lightColorZ = -0.4f;
			
			sndPlaySound("C:\\Users\\VLAD PC\\Desktop\\ProiectPG\\Lab6\\objects\\balcanu\\Thunder\\dragonasu.wav", SND_ASYNC | FALSE);
		}
		if (dif % 15000 >= 700 && dif % 15000 <= 800) {
			lightColorX = 0.796f;
			lightColorY = 0.764f;
			lightColorZ = 0.89f;
	
		}
		if (dif % 15000 >= 800 && dif % 15000 <= 900) {
			lightColorX = 1.0f;
			lightColorY = -0.4f;
			lightColorZ = -0.4f;
		}
	}
}
void renderScene() 
{

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, retina_width, retina_height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myCustomShader.useShaderProgram();
	glUniform1f(fogDensityLoc, fogDensity);
	glUniform1f(linearLoc, linear);
	glUniform3fv(positionLocPointLight, 1, glm::value_ptr(positionPointLight));
	glUniform1f(linearLoc, linear);
	glUniform3fv(positionLocPointLight2, 1, glm::value_ptr(positionPointLight2));
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//bind the shadow map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	drawObjects(myCustomShader, false);
	if(lightColorY >= -0.4f){
		lightColorY -= 0.003f;
		lightColorZ -= 0.003f;
	}
	lighting();
	lightAngle += 0.005f;
	lightColor = glm::vec3(lightColorX, lightColorY, lightColorZ);
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	model = lightRotation;
	model = glm::translate(model, 1.0f * lightDir);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	if (lightSpike == true) {
		quadratic += 0.05f;
		linear += 0.1f;
	}
	else {
		quadratic -= 0.05f;
		linear -= 0.1f;
	}
	if (quadratic >= 4.5f) {
		lightSpike = false;
	}
	if (quadratic <= 2.8f) {
		lightSpike = true;
	}
	PRESENT();
	wingMovement();


	



}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
