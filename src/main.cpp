/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:
This is the main loop for this program. It loads in all of the objects, runs the draw loop, and updates the view based on user inputs.
*/

// Include libraries
//////////////////////////////////////////////////////////////////////////////////////

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <utility>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

#include "chunkManager.hpp"
#include "ViewController.h"
#include "ColorMap.h"
#include "Chunk.hpp"
#include <iostream>

// This struct makes it easier to keep track of the texture and locations of many coppies of the same chess piece asset.
struct Piece {
	Piece(std::string t, std::vector<glm::vec3> p) {
		texturePath = t;
		positions = p;
	}
	Piece()=default;
	std::string texturePath; // Texture for this mesh
	std::vector<glm::vec3> positions; // Locations of the coppies of this mesh
};

void printBuffer(GLuint VBO, GLuint EBO, GLsizei vertexCount, GLsizei indexCount);

int main( void )
{
	/********************************************************************
	 * Initialize the OpenGL window
	 ********************************************************************/

	ChunkManager manager(1, 123, 16.0);
	//Chunk<std::vector<glm::vec3>> mychunk;
	std::cout << "manager created" << std::endl;

	// Initialize GLFW

	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "FinalProject", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.15f, 0.15f, 0.15f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it is closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);  

	/********************************************************************
	 * SCENE MECHANICS
	 ********************************************************************/

	// SHADERS : Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// MATRICES : Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	
	/********************************************************************
	 * View Control
	 ********************************************************************/

	// Create the view controller object
	glm::vec3 position = glm::vec3(0.f, 5.f, 0.f);					// User's position
	float horizontalAngle = 0.f; 	float verticalAngle = -3.14f/2;	// Look angles
	float speed = 3.f;				float mouseSpeed = 0.005f;		// Speed and Mouse sensitivity
	float fov = 45.f;												// Field of view (degrees)
	
	// ViewController class instance
	ViewController viewController(position, horizontalAngle, verticalAngle, speed, mouseSpeed, fov);

	// Color map
	ColorMap colorMap(ColorMapType::GIST_EARTH);

	/********************************************************************
	 * Create hight map
	 ********************************************************************/

	// Bounds
	float LENGTH_X = 50;
	float LENGTH_Z = 50;
	float resolution = 0.1f;

	unsigned int SIZE_X = static_cast<unsigned int>(LENGTH_X / resolution);
	unsigned int SIZE_Z = static_cast<unsigned int>(LENGTH_Z / resolution);

	const unsigned int NUM_STRIPS = SIZE_Z-1;
	const unsigned int NUM_TRIANGLES_PER_STRIP = (SIZE_X-1)*2;
	const unsigned int NUM_VERTS_PER_STRIP = SIZE_X*2;

	// Vertices
	float pi = 3.14159265359f;
	float y_max = 0;
	float y_min = 0;
	std::vector<glm::vec3> vertices;
	for (unsigned int i = 0; i < SIZE_Z; i++) 
	{
		for (unsigned int j = 0; j < SIZE_X; j++) 
		{
			// Create a grid centered at origin, scaled to be easily visible
			float x = -1.f*LENGTH_X/2.f + i*resolution;
			float z = -1.f*LENGTH_Z/2.f + j*resolution;

			float y = sin(2*pi*x/10) * sin(2*pi*z/10);

			vertices.push_back(glm::vec3(x,y,z));

			if (y > y_max) 
			{
				y_max = y;
			}
			if (y < y_min) 
			{
				y_min = y;
			}
		}
	}

	// Get the associated colors
	std::vector<glm::vec3> colors = colorMap.getColorVector(vertices);	
	
	// Index generation for triangle strips
	std::vector<unsigned int> indices_triangles_strips;
	for (unsigned int i = 0; i < SIZE_Z - 1; i++) {
		// For each row, create a triangle strip
		for (unsigned int j = 0; j < SIZE_X; j++) {
			// Add vertex from bottom row
			indices_triangles_strips.push_back(i * SIZE_X + j);
			// Add vertex from top row
			indices_triangles_strips.push_back((i + 1) * SIZE_X + j);
		}
	}

	std::cout << "Created lattice of " << NUM_STRIPS << " strips with " << NUM_TRIANGLES_PER_STRIP << " triangles each" << std::endl;
    std::cout << "Created " << NUM_STRIPS * NUM_TRIANGLES_PER_STRIP << " triangles total" << std::endl;

	// VERTEX ARRAY OBJECT used in the shader
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Vertex Buffer Object (VBO) for vertices positions
	GLuint terrainVBO;									// Declare
	glGenBuffers(1, &terrainVBO);						// Generate the buffer
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);			// Bind the VBO as the active GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, 						// Load data in the active buffer
				 vertices.size() * sizeof(glm::vec3), 		// Size of the data in bytes
				 vertices.data(), 							// Pointer to the data
				 GL_STATIC_DRAW);							// Data is static set once

	glVertexAttribPointer(	// Set the active buffer (VBO) as the attribute 0 of the VAO
		0,                  	// attribute index (0) for vertices positions
		3,                  	// size of each elemeent (3 floats)
		GL_FLOAT,           	// type of each subelement
		GL_FALSE,           	// normalized?
		0,						// Offset between consecutive elements
		(void*)0            	// Array buffer offset
	);

	glEnableVertexAttribArray(0);  // Enable the buffer for the shader

	// Vetex Buffer Object (VBO) for the colors
	GLuint colorVBO;									// Declare
	glGenBuffers(1, &colorVBO);							// Generate the buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);			// Bind the VBO as the active GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, 						// Load data in the active buffer
				 colors.size() * sizeof(glm::vec3), 		// Size of the data in bytes
				 colors.data(), 							// Pointer to the data
				 GL_STATIC_DRAW);							// Data is static set once
	
	glVertexAttribPointer(	// Set the active buffer (VBO) as the attribute 0 of the VAO
		1,                  	// attribute index (1) for colors
		3,                  	// size of each elemeent (3 floats)
		GL_FLOAT,           	// type of each subelement
		GL_FALSE,           	// normalized?
		0,						// Offset between consecutive elements
		(void*)0            	// Array buffer offset
	);

	glEnableVertexAttribArray(1);  // Enable the buffer for the shader

	// Element Buffer Object (EBO)
	GLuint terrainEBO;									// Declare
	glGenBuffers(1, &terrainEBO);						// Generate the buffer	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);	// Bind the EBO as the active GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 				// Load data in the active buffer
				indices_triangles_strips.size() * sizeof(unsigned int), 	// Size of the data in bytes
				indices_triangles_strips.data(), 							// Pointer to the data
				GL_STATIC_DRAW);											// Data is static set once

	/********************************************************************
	 * Main loop
	 ********************************************************************/

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate the shader program
		glUseProgram(programID);

		/********************************************************************
	 	* Actualize the scene
	 	********************************************************************/

		// Use the view controller to get the matrices from the user inputs
		viewController.computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = viewController.getProjectionMatrix();		// Get the projection matrix
		glm::mat4 ViewMatrix = viewController.getViewMatrix();					// Get the view matrix
		glm::mat4 ModelMatrix = glm::mat4(1.0f);								// Model matrix (set at origin)

		// Model View Projection matrix
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send the matrices to the shader
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        /********************************************************************
	 	* Draw the terrain
	 	********************************************************************/

		// Change the triangles rendering mode by pressing the key P ( GL_FILL (Filled Triangles) or GL_LINE (Wireframe))
		glPolygonMode(GL_FRONT_AND_BACK, viewController.getRenderingMode()); 

		// Bind VAO (implicitely binds the EBO to GL_ELEMENT_ARRAY_BUFFER)
		glBindVertexArray(VertexArrayID);

		// Draw the triangles strips by strips
		for(unsigned strip = 0; strip < NUM_STRIPS; strip++)
		{
			// Draw the triangles
			glDrawElements(
					GL_TRIANGLE_STRIP,			// Drawing mode : triangle strips save the number indices per strip compared to GL_TRIANGLES
					NUM_VERTS_PER_STRIP,  		// Number of indices per strip
					GL_UNSIGNED_INT,			// Type of the indices
					(void*)(strip * NUM_VERTS_PER_STRIP * sizeof(unsigned int))		// Offset of the first index of the strip in the EBO
				);
		}
		
		// Unbind VAO
		glBindVertexArray(0);
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	/********************************************************************
	 * End of game
	 ********************************************************************/

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    // Clean buffers, arrays and shader program
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &terrainVBO);
	glDeleteBuffers(1, &colorVBO);
    glDeleteBuffers(1, &terrainEBO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Check the buffers

void printBuffer(GLuint VBO, GLuint EBO, GLsizei vertexCount, GLsizei indexCount) {
    // Bind VBO and read vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::vector<float> vertexData(vertexCount * 3); // Assuming glm::vec3 (3 floats per vertex)
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), vertexData.data());

    std::cout << "Vertex Buffer Object (VBO) Data:" << std::endl;
    for (GLsizei i = 0; i < vertexCount; ++i) {
        std::cout << "Vertex " << i << ": ("
                  << vertexData[i * 3] << ", "
                  << vertexData[i * 3 + 1] << ", "
                  << vertexData[i * 3 + 2] << ")" << std::endl;
    }

    // Bind EBO and read index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    std::vector<GLuint> indexData(indexCount);
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexData.size() * sizeof(GLuint), indexData.data());

    std::cout << "Element Buffer Object (EBO) Data:" << std::endl;
    for (GLsizei i = 0; i < indexCount; ++i) {
        std::cout << "Index " << i << ": " << indexData[i] << std::endl;
    }
}

