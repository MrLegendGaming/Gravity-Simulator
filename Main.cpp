#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
glm::vec3 calculateGravity(const glm::vec3& position1, float mass1, const glm::vec3& position2, float mass2);
void gravity(glm::vec3& position, float strength, glm::vec3& speed, glm::vec3 gravityPos);


#include <random>

float random_float() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	return dis(gen);
}

// --------------------- CAMERA ---------------------
// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// --------------------- TIMING MANAGEMENT ---------------------
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// --------------------- VERTICES & INDICES ---------------------
float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

int main()
{

	// --------------------- INITIALIZING GLFW ---------------------
	glfwInit();

	// Tells GLFW what version of OpneGL we are using (v3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tells OpenGL which profile we are using (Core means only modern OpenGL commands)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	// --------------------- WINDOW CREATION ---------------------
	// Creates a window in OpenGL
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	// Error checking for window if it fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Defining a monitor
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// Centerring the window
	glfwSetWindowPos(window, (mode->width - SCR_WIDTH/2) - (mode->width /2), (mode->height - SCR_HEIGHT / 2) - (mode->height / 2));


	// Makes the window current context so we use it
	glfwMakeContextCurrent(window);
	// Changes the viewport to fit the screenwidth
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Tells GLFW to hide and capture the mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Loads GLAD so we can use OpenGL and checks for errors if it fails
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	double previousTime = glfwGetTime();
	int frameCount = 0;

	// --------------------- SHADER STUFF ---------------------
	                     // Shader Program //
	// Creates a vertex & fragment shader and attaches it to the source code for the shader then compiles it
	Shader ourShader("default.vert", "default.frag");


	// --------------------- VERTEX MANAGEMENT ---------------------
	// Generates a Vertex Array Object, Vertex Buffer & Object Index Buffer Objects and stores the vertices into them before sending them to the GPU
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);

	// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 1. then set the vertex attributes pointers
	// Position Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Color Attributes
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/
	// Texture Attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 0. copy our elements array in a buffer for OpenGL to use
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/
	
	Sphere particle(0.25f);
	Sphere sun(2.0f);
	

	

	// --------------------- TRANSFORMATIONS ---------------------

	unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");

	// --------------------- 3D RENDERING ---------------------
	// Fixes the Z-Axis buffer layering when drawing the cube
	glEnable(GL_DEPTH_TEST);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 startPositions[10000];

	const int posNum = sizeof(startPositions) / sizeof(startPositions[0]);

	glm::vec3 newPosition[posNum];

	glm::vec3 initialVelocity[posNum];


	for (int i = 0; i < posNum; i++)
	{
		float f = i;

		if (i > 1000)
		{
			f = ((float)i / 1000) + 5.0f;
		}

		if (i > 100) 
		{
			f = ((float)i / 100) + 5.0f;
		}

		if (f > 50)
		{
			f = 100-f + 1.5f;
		}

		if (i % 2 == 0)
		{
			f *= -1;
		}

		
		std::cout << f << std::endl;
		startPositions[i] = glm::vec3(f * random_float(), f * random_float(), f * random_float());
	}

	for (int i = 0; i < posNum; i++)
	{
		newPosition[i] = startPositions[i];
		//initialVelocity[i] = glm::vec3(random_float(), random_float(), random_float());
		initialVelocity[i] = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	float time;
	

	// --------------------- MAIN WHILE LOOP ---------------------
	while (!glfwWindowShouldClose(window))
	{
		time = glfwGetTime();
		// Per-frame time logic
		float currentFrame = static_cast<float>(time);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		frameCount++;


		if (currentFrame - previousTime >= 1.0)
		{
			std::cout << "FPS: " << frameCount << std::endl;
			frameCount = 0;
			previousTime = currentFrame;
		}

		// Input here
		processInput(window);

		// Renderring Commands 
		// Clears the color & depth buffer and sets a colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Shader

		ourShader.use();

		// Creates the model, view & projection matrix
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		// Look at function (cameraPos, cameraTarget, worldUp)
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

		int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// Drawing the sphere
		for (unsigned int i = 0; i < posNum; i++)
		{
			unsigned int j = i;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, newPosition[i]);
			
			if (i != 0)
			{
				gravity(newPosition[i], -25, initialVelocity[i], newPosition[0]);

				glUniform3f(glGetUniformLocation(ourShader.ID, "ampColor"),
					glm::sin(time* j),
					glm::cos(time* j),
					glm::atan(time* j)
				);

			}
			else
			{
				glUniform3f(glGetUniformLocation(ourShader.ID, "ampColor"),
					time,
					time,
					0
				);
			}

			float angle = 20.0f * j;
			model = glm::rotate(model, (float)time * glm::radians(angle), glm::vec3(2.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);
			if (i == 0)
			{
				sun.draw();
			}
			else
			{
				particle.draw();
			}
			
		}

		// Swaps the back and front buffer of the window and checks events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(UP, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DOWN, deltaTime);
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{

	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);

}

//glm::vec3 calculateGravity(glm::vec3& position1, float mass1, const glm::vec3& position2, float mass2) {
//	// Force = G * m1m2/r^2
//	// F = ma 
//	// Acceleration = G * m/r^2
//
//	float G = 0.0000000000667430f;
//	//float G = 6.67430f;
//
//	glm::vec3 r = position2 - position1;
//    float distance = glm::length(r);
//
//	// Avoid division by zero
//	if (distance == 0.0f)
//		return glm::vec3(0.0f, 0.0f, 0.0f);
//
//	// Normalize the distance vector
//	glm::vec3 r_hat = glm::normalize(r);
//
//	// Calculate the gravitational force
//	glm::vec3 force = -G * ((mass1 * mass2) / (distance * distance)) * r_hat;
//
//	// Calculate the transformation
//	return force;
//
//}

void gravity(glm::vec3 &position, float strength, glm::vec3 &speed, glm::vec3 gravityPos)
{
	float distanceX = position.x - gravityPos.x;
	float distanceY = position.y - gravityPos.y;
	float distanceZ = position.z - gravityPos.z;

	float distance = glm::length(position - gravityPos);

	float inverse_distance = 1.0f / distance;

	float normalized_x = inverse_distance * distanceX;
	float normalized_y = inverse_distance * distanceY;
	float normalized_z = inverse_distance * distanceZ;

	float inverse_square_dropoff = inverse_distance * inverse_distance;

	glm::vec3 acceleration = glm::vec3(normalized_x * strength * inverse_square_dropoff,
									   normalized_y * strength * inverse_square_dropoff,
									   normalized_z * strength * inverse_square_dropoff);

	speed.x += acceleration.x;
	speed.y += acceleration.y;
	speed.z += acceleration.z;

	position.x += speed.x;
	position.y += speed.y;
	position.z += speed.z;


}