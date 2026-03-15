#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"

// Vertices coordinates
GLfloat vertices[] =
{		//Coordinates		// RGB values        // Texture coordinates
	-0.5f,  -0.5f, 0.0f,   0.8f, 0.3f,  0.02f,   0.0f, 0.0f,	// Lower left corner
	-0.5f,   0.5f, 0.0f,   0.8f, 0.3f,  0.02f,   0.0f, 1.0f,	// Upper left corner
	 0.5f,   0.5f, 0.0f,   1.0f, 0.6f,  0.32f,   1.0f, 1.0f,	// Upper right corner
	 0.5f,  -0.5f, 0.0f,   0.9f, 0.45f, 0.17f,   1.0f, 0.0f		// Lower right corner
};

GLuint indices[] =
{
	0, 2, 1, 
	0, 3, 2
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
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Window", NULL, NULL);

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
	glViewport(0, 0, 800, 800);

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

	// Texture
	Texture moon("moon128x128.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	moon.texUnit(shaderProgram, "tex0", 0);

	


	//The last time of the tick
	double lastTime = 0;
	//Added Scale
	float scale = 0.0f;

	// Main while loop 
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Assigns a value to the uniform variable. We can only do this after the Shader program is activated.
		if (glfwGetTime() - lastTime > 1.0f / 60.0f)
		{
			scale += 0.05f;
			lastTime = glfwGetTime();
		}

		glUniform1f(uniID, 0.2f * sin(scale));
		//Binds texture so that it appears in rendering
		moon.Bind();

		// Bind the VAO so OpenGL knows how to use it
		VAO1.Bind();
		// Draw the triangle using GL_TRIANGLES primitive
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		// Takes care of all GLFW events
		glfwPollEvents();
	}

	// Delete all objects we created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	moon.Delete();

	// Destroying the window before ending the program
	glfwDestroyWindow(window);
	// Uninitialize glfw before ending the program
	glfwTerminate();
	return 0;
}