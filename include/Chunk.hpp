/*
Author: Matthew Luyten
Class: ECE4122
Last Date Modified: 10/26/2024

Description:
This is the header for the Chunk class. This class manages an NxN chunk of heightmap.
*/

#pragma once

#include <vector>

#include <GL/glew.h>                            // OpenGL Library
#include <glm/glm.hpp>                          // OpenGL Mathematics
#include <glm/gtc/matrix_transform.hpp>         // Transformation matrices

#include <SFML/Graphics.hpp>                    // Simple and Fast Multimedia Library
#include <SFML/Window.hpp>                      // Windowing library

#include "ColorMap.hpp"                         // Init the color buffer

class Chunk {
private:
    double m_chunkSize;
    double m_resolution; //distance between points
    unsigned int m_pointsPerSide; //points per side
    glm::vec2 m_chunkCoords;
    bool m_preparedToRender;

    // OpenGL variables for 3D rendering
    GLuint vertexArrayObject;  // Vertex Array Object (VAO) for the chunk, contains vertices and colors VBOs and EBO
    GLuint vertexBuffer;       // Vertex Buffer Object (VBO) for vertices
    GLuint colorBuffer;        // Vertex Buffer Object (VBO) for colors 
    GLuint elementBuffer;      // Element Buffer Object (EBO) for indices

    sf::Texture texture2D;     // Texture for the chunk
    sf::Image image;           // Image for the texture

public:
    //constructors
    Chunk() {}
    Chunk(int64_t seed, double chunkSize, double resolution, glm::vec2 chunkCoords);
    //Chunk(size_t sz, glm::vec3 pos) : sz(sz), pos(pos) {}
    //~Chunk() {}

    //heightmap
    std::vector<glm::vec3> heightMap;

    //getters and setters
    double size() { return m_chunkSize; }
    double resolution() { return m_resolution; }
    int pointsPerSide() { return m_pointsPerSide; }
    glm::vec2 chunkCoords() { return m_chunkCoords; }
    sf::Texture* getTexture() { return &(this->texture2D); }
    bool preparedToRender() { return m_preparedToRender; }

    // Init buffers
    void prepareToRender(ColorMap* cmapPointer);

    // Render the 3D chunk
    void renderChunk(GLuint* shaderProgram);

    // Destructor
    ~Chunk();

    //void setHeightMap(T hm) { this->hm = hm; }
    //void setSize(size_t sz) { this->sz = sz; }
    //void setPosition(glm::vec3 pos) { this->pos = pos; }
};