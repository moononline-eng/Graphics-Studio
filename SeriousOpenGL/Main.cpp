#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"

const unsigned int width = 1200;
const unsigned int height = 1200;


// Vertices coordinates
GLfloat vertices[] =
{//		 Coordinates	 /     RGB values        /Texture coordinates
	-0.5f,   0.0f,  0.5f,   0.83f, 0.70f, 0.44f,   0.0f, 0.0f,
	-0.5f,   0.0f, -0.5f,   0.83f, 0.70f, 0.44f,   5.0f, 0.0f,
	 0.5f,   0.0f, -0.5f,   0.83f, 0.70f, 0.44f,   0.0f, 0.0f,	
	 0.5f,   0.0f,  0.5f,   0.83f, 0.70f, 0.44f,   5.0f, 0.0f,	
	 0.0f,   0.8f,  0.0f,   0.92f, 0.86f, 0.76f,   2.5f, 5.0f
};

GLuint indices[] =
{
	0,1,2,
	0,2,3,
	0,1,4,
	1,2,4,
	2,3,4,
	3,0,4
};

int main() 
{
	// Initialize GLFW
	glfwInit();

	// Telling GLFW which major and minor version of OpenGL we want to use (we are using 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tell GLFW we are using the CORE profile
	//This means we only have modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create a GLFWwindow object with the pixels 800 x 800 and naming it "OpenGL Window"
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);

	//Error handling to see if we have a window or not
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// We tell OpenGL that we want to use the window we created
	glfwMakeContextCurrent(window);
	// Load GLAD so it configures OpenGL
	gladLoadGL();

	// Tell OpenGL which part of the screen we want to render. Here we want to render from the bottom left (0,0) to the top right (800,800)
	glViewport(0, 0, width, height);

	// Create a Shader Program with Vertex shader from default.vert and Fragment shader from default.frag
	Shader shaderProgram("default.vert", "default.frag");

	//Generate a Vertex Array Object for us to use
	VAO VAO1;
	// Make it the currently used VAO
	VAO1.Bind();

	//Create a Vertex Buffer Object for us to bind ot the VAO 
	VBO VBO1(vertices, sizeof(vertices));
	// Create an EBO that is automatically binded on instantiation (check class)
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attritubes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Get a reference for the uniform float "scale" in the Shader Program. The uniform float scale is defined in the Fragment shader
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Texture choice. Textures can bug if the wrong color format is used.
	Texture testTexture("moon.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	testTexture.texUnit(shaderProgram, "tex0", 0);


	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	glEnable(GL_DEPTH_TEST);

	// Main while loop 
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		double currentTime = glfwGetTime();
		if (currentTime - prevTime >= 1 / 60)
		{
			rotation += 0.5f;
			prevTime = currentTime;
		}

		// Initialize the Identity matrix (Enhetsmatrisen)
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f); 
		glm::mat4 proj = glm::mat4(1.0f); 


		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));		// Move the camera down 0.5 and forward 2.0. Z axis is positive towards us and negative away from us
		proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);

		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Assigns a value to the uniform variable. We can only do this after the Shader program is activated.
		glUniform1f(uniID, 0.5f);
		//Binds texture so that it appears in rendering
		testTexture.Bind();

		// Bind the VAO so OpenGL knows how to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the front and back buffers
		glfwSwapBuffers(window);

		// Takes care of all GLFW events
		glfwPollEvents();
	}

	// Delete all objects we created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	testTexture.Delete();

	// Destroying the window before ending the program
	glfwDestroyWindow(window);
	// Uninitialize glfw before ending the program
	glfwTerminate();
	return 0;
}