/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:

View Controller class header file. This class handles how the user's point of view is controlled
by the user inputs, managing the projection and view matrices.
*/

#pragma once

// Include GLEW
#include <GL/glew.h>                          // OpenGL Library (use OpenGL definitions)

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include SFML
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// Standard libraries
#include <map>

// Include project header files
#include "ChunkManager.hpp"

/**
 * @class ViewController
 * 
 * @details This class handles how the user's view is controlled by the user inputs
 */
class ViewController 
{
    private:

        // Screen display variables
        sf::Vector2u windowSize;        // Window size
        bool fillTriangles;             // Fill triangles flag
        bool fKeyPressed;               // F key pressed flag (to avoid multiple toggles on triangle flag)
        bool view2D;                    // 2D map view flag

        // Point of view variables
        glm::vec3 position;             // User's position in cartesian cooedinates [m, m, m]
        float horizontalAngle;          // Horizontal look angle [rad]
        float verticalAngle;            // Vertical look angle [rad]
        glm::mat4 ViewMatrix;           // View matrix
        
        // Movement variables
        sf::Clock clock;                // Clock for the time difference between current and last frame
        float speed;                    // User's displacement speed [m/s]
        float mouseSpeed;               // User's mouse sensitivity [rad/s]

        // Projection variables 
        bool perspectiveProjection;     // Perspective projection mode
        float fov_deg;                  // Field of View [deg]
        float aspectRatio;              // Aspect ratio
        float nearPlane;                // Near plane distance [m]
        float farPlane;                 // Far plane distance [m]
        bool orthographicProjection;    // Orthographic projection mode
        float top;                      // Top plane distance [m]
        float bottom;                   // Bottom plane distance [m]
        float left;                     // Left plane distance [m]
        float right;                    // Right plane distance [m]
        glm::mat4 ProjectionMatrix;     // Projection matrix

    public:
        // Constructor
        ViewController(const sf::Vector2u& windowSize,
                        const glm::vec3& position, const float speed,
                        const float horizontalAngle,  const float verticalAngle, const float mouseSpeed, 
                        const float fov_deg);

        // Actualize the matrices from the user inputs
        void computeMatricesFromInputs();

        // Update the user looking direction and fov then compute the projection matrix
        void updateLook();

        // Update the user position and compute the view matrix
        void updateMove(float deltaTime);  

        // Update the triangle rendering mode
        void updateTriangleRendering();

        // Set the 2D map view
        void ViewController::view2DMap(sf::RenderWindow* window, const Edge2D& edges);

        /////////////////// Getters & setters //////////////////////

        // Get the window size
        sf::Vector2u getWindowSize();

        // Set the window size
        void setWindowSize(const sf::Vector2u& windowSize);

        // Get the view matrix
        glm::mat4 getViewMatrix();

        // Get the projection matrix
        glm::mat4 getProjectionMatrix();

        // Get the rendering mode
        int getRenderingMode();

};