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
void gravity(glm::vec3& position, float strength, glm::vec3& speed, glm::vec3 gravityPos, float playSpeed);

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

bool firstClick = true;

// --------------------- TIMING MANAGEMENT ---------------------
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float playingSpeed = 0.0f;
bool rewindPlay = false;

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gravity Simulator", NULL, NULL);
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
	// Generates a Vertex Array Object & Sphere Objects and stores the vertices into them before sending them to the GPU
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// Binds Vertex Array Object
	glBindVertexArray(VAO);

	// Creates the sphere models for dawing
	Sphere particle(0.25f);
	Sphere sun(2.0f);

	// --------------------- TRANSFORMATIONS ---------------------

	unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");

	// --------------------- 3D RENDERING ---------------------
	// Fixes the Z-Axis buffer layering when drawing the cube
	glEnable(GL_DEPTH_TEST);

	// Creates the positions and intiial velocities of the particles & sun
	// Sun's coordinate is index 0 or (first in the array list)
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
		//startPositions[i] = glm::vec3(f);
		
	}

	for (int i = 0; i < posNum; i++)
	{
		newPosition[i] = startPositions[i];
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
				gravity(newPosition[i], -25, initialVelocity[i], newPosition[0], playingSpeed);

				glUniform3f(glGetUniformLocation(ourShader.ID, "ampColor"),
					glm::sin(time * j),
					glm::cos(time * j),
					glm::atan(time * j)
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
	glDeleteShader(ourShader.ID);
	delete[] startPositions;
	delete[] newPosition;
	delete[] initialVelocity;

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	// -- WINDOW --
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// -- MOVEMENT --
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
	
	//  -- MOUSE --
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (firstClick) 
		{
			glfwSetCursorPosCallback(window, mouse_callback);
			// Tells GLFW to hide and capture the mouse
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			firstClick = false;
		}
	}

	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		if (!firstClick)
		{
			glfwSetCursorPosCallback(window, NULL);
			// Tells GLFW to hide and capture the mouse
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClick = true;
			firstMouse = true;
		}
	}
	// -- PLAY SPEED --
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!rewindPlay)
		{
			playingSpeed *= -1;
			rewindPlay = true;
		}

	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (rewindPlay)
		{
			playingSpeed *= -1;
			rewindPlay = false;
		}
	}

	// Play and Pause
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		playingSpeed = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		playingSpeed = 0.0f;
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

void gravity(glm::vec3 &position, float strength, glm::vec3 &speed, glm::vec3 gravityPos, float playSpeed)
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

	speed.x += acceleration.x * playSpeed;
	speed.y += acceleration.y * playSpeed;
	speed.z += acceleration.z * playSpeed;

	position.x += speed.x * playSpeed;
	position.y += speed.y * playSpeed;
	position.z += speed.z * playSpeed;

}