/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 11/28/2024

Description: This is the implementation file for the Chunk class. This class manages an NxN chunk of vertices and colors for rendering.
The (x,z) coordinates give the location of the point in 2D while the y coordinate gives the height of the point.

*/

// Standard libraries
#include <iostream>
#include <cmath>

// Header file
#include "Chunk.hpp"

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief Constructor
 * @param seed : seed for the random number generator
 * @param chunkSize : size of the chunk (in meters)
 * @param resolution : distance between points (in meters)
 * @param chunkCoords : coordinates of the chunk in the chunk map (in chunks)
 */
Chunk::Chunk(int64_t seed, double chunkSize, double resolution, glm::vec2 chunkCoords) {
	// Set the parameters
	m_chunkSize = chunkSize;
	m_resolution = resolution;
	m_chunkCoords = chunkCoords;
	m_preparedToRender = false;
	m_pointsPerSide = static_cast<unsigned int>(m_chunkSize / m_resolution);

	// Create the height map with zeros
	heightMap = std::vector<glm::vec3>(m_pointsPerSide * m_pointsPerSide, glm::vec3(0, 0, 0));

	// Create the texture and image (unititialized)
	this->texture2D.create(this->m_pointsPerSide, this->m_pointsPerSide);
	this->image.create(this->m_pointsPerSide, this->m_pointsPerSide);
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Initialize the buffers for the chunk
 * @param cmapPointer : Pointer to the color map
 */
void Chunk::prepareToRender(ColorMap* cmapPointer)
{
	/*
	* 1. 3D RENDERING STUFF (Buffers)
	*/
	// Bind the VAO
	glGenVertexArrays(1, &(this->vertexArrayObject));
	glBindVertexArray(this->vertexArrayObject);

	// Get the associated colors
	std::vector<glm::vec3> colors = cmapPointer->getColorVector(this->heightMap);	
	
	// Index generation for triangle strips
	std::vector<unsigned int> indices_triangles_strips;
	for (unsigned int i = 0; i < this->m_pointsPerSide - 1; i++) {
		// For each row, create a triangle strip
		for (unsigned int j = 0; j < this->m_pointsPerSide; j++) {
			// Add vertex from bottom row
			indices_triangles_strips.push_back(i * this->m_pointsPerSide + j);
			// Add vertex from top row
			indices_triangles_strips.push_back((i + 1) * this->m_pointsPerSide + j);
		}
	}

	// Vertex Buffer Object (VBO) for vertices positions
	glGenBuffers(1, &(this->vertexBuffer));						// Generate the buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);			// Bind the VBO as the active GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, 								// Load data in the active buffer
				 this->heightMap.size() * sizeof(glm::vec3), 		// Size of the data in bytes
				 this->heightMap.data(), 							// Pointer to the data
				 GL_STATIC_DRAW);									// Data is static set once

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
	glGenBuffers(1, &(this->colorBuffer));				// Generate the buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);	// Bind the VBO as the active GL_ARRAY_BUFFER
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
	glGenBuffers(1, &(this->elementBuffer));						// Generate the buffer	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);		// Bind the EBO as the active GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 							// Load data in the active buffer
				indices_triangles_strips.size() * sizeof(unsigned int), 	// Size of the data in bytes
				indices_triangles_strips.data(), 							// Pointer to the data
				GL_STATIC_DRAW);											// Data is static set once

	// Unbind VAO
	glBindVertexArray(0);

	/*
	* 2. 2D RENDERING STUFF (Chunk texture)
	*/

	// Go over the pixels and compute the color of the image
	for(unsigned int i = 0; i < this->m_pointsPerSide; i++)		// Rows - x axis
	{
		for(unsigned int j = 0; j < this->m_pointsPerSide; j++)	// Columns - z axis
		{
			// Get the color for the current pixel from the 1D color vector
			glm::vec3 color = colors[i * this->m_pointsPerSide + j];
			
			// Set the pixel color (window (i,j) = 3d world (x,z))
			if(i==0 || j==0 || i==this->m_pointsPerSide-1 || j==this->m_pointsPerSide-1)
			{
				// If the pixel is on the border, set it to black (we want to see the chunk borders in 2D)
				image.setPixel(i, j, sf::Color::Black);
			}
			else{
				// Set the color of the pixel to the one in 3D
				image.setPixel(i, j, sf::Color(color.x * 255, color.y * 255, color.z * 255));
			}
		}
	}

	// Initialize the texture from the computed image
	this->texture2D.update(this->image);

	// Set the chunk as prepared to render (flag)
	m_preparedToRender = true;
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Render the 3D chunk
 * @param shaderProgram : pointer to the shader program
 */

void Chunk::renderChunk(GLuint* shaderProgramPointer)
{
	// Activate the shader program
	glUseProgram(*shaderProgramPointer);
	
	// Compute the number of strips and vertices per strip
	unsigned int nStrips = this->m_pointsPerSide-1;
	unsigned int nVertsPerStrip = this->m_pointsPerSide*2;
	
	// Bind the VAO
	glBindVertexArray(this->vertexArrayObject);
	
	// Draw the triangles strips by strips
	for(unsigned strip = 0; strip < nStrips; strip++)
	{
		// Draw the triangles
		glDrawElements(
				GL_TRIANGLE_STRIP,		// Drawing mode : triangle strips save the number indices per strip compared to GL_TRIANGLES
				nVertsPerStrip,  		// Number of indices per strip
				GL_UNSIGNED_INT,		// Type of the indices
				(void*)(strip * nVertsPerStrip * sizeof(unsigned int))		// Offset of the first index of the strip in the EBO
			);
	}
	
	// Unbind VAO
	glBindVertexArray(0);
	
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Destructor
 */
Chunk::~Chunk()
{
	// Cleanup VAO, points VBOn colors VBO and EBO
	glDeleteVertexArrays(1, &(this->vertexArrayObject));
	glDeleteBuffers(1, &(this->vertexBuffer));
	glDeleteBuffers(1, &(this->colorBuffer));
	glDeleteBuffers(1, &(this->elementBuffer));
}