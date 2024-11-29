/*
Author: Matthew Luyten
Class: ECE6122
Last Date Modified: 11/29/2024

Description: This is the header file for the Chunk class. This class manages an NxN chunk of vertices and colors for rendering.
The (x,z) coordinates give the location of the point in 2D while the y coordinate gives the height of the point.

*/

#pragma once

// Standard libraries
#include <vector>

// OpenGL libraries
#include <GL/glew.h>                            // OpenGL Library
#include <glm/glm.hpp>                          // OpenGL Mathematics
#include <glm/gtc/matrix_transform.hpp>         // Transformation matrices

// SFML libraries
#include <SFML/Graphics.hpp>                    // Simple and Fast Multimedia Library
#include <SFML/Window.hpp>                      // Windowing library

// Custom libraries
#include "ColorMap.hpp"                         // Init the color buffer

/**
 * @class Chunk
 * @brief describe a NxN chunk of vertices and colors for rendering
 */
class Chunk {

    private:

        // Specific Attributes
        double m_chunkSize;             // size of the chunk (in meters)
        double m_resolution;            // distance between points (in meters)
        unsigned int m_pointsPerSide;   // N = points per side
        glm::vec2 m_chunkCoords;        // coordinates of the chunk in the chunk map (in chunks)
        bool m_preparedToRender;        // flag to check if the chunk is prepared to render

        // 3D rendering variables
        GLuint vertexArrayObject;  // Vertex Array Object (VAO) for the chunk, contains vertices and colors VBOs and EBO
        GLuint vertexBuffer;       // Vertex Buffer Object (VBO) for vertices
        GLuint colorBuffer;        // Vertex Buffer Object (VBO) for colors 
        GLuint elementBuffer;      // Element Buffer Object (EBO) for indices

        // 2D Rendering variable
        sf::Texture texture2D;     // Texture for the chunk
        sf::Image image;           // Image for the texture

    public:

        // Chunk heightmap
        std::vector<glm::vec3> heightMap;
        
        ////////////////////////// METHODS //////////////////////////////////////
        
        // Default constructor
        Chunk() {}

        // Custom constructor
        Chunk(int64_t seed, double chunkSize, double resolution, glm::vec2 chunkCoords);

        // Init buffers
        void prepareToRender(ColorMap* cmapPointer);

        // Render the 3D chunk
        void renderChunk(GLuint* shaderProgram);
        
        ////////////////////////// GETTERS AND SETTERS //////////////////////////

        // Get the size of the chunk
        double size() { return m_chunkSize; }

        // Get the resolution of the chunk
        double resolution() { return m_resolution; }

        // Get the number of points per side
        int pointsPerSide() { return m_pointsPerSide; }

        // Get the chunk coordinates
        glm::vec2 chunkCoords() { return m_chunkCoords; }

        // Get the 2D texture
        sf::Texture* getTexture() { return &(this->texture2D); }

        // Get the flag preparedToRender
        bool preparedToRender() { return m_preparedToRender; }

        // Destructor : destroys the buffers
        ~Chunk();
};