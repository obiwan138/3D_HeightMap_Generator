/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:

View Controller class header file. This class handles how the user's point of view is controlled
by the user inputs, managing the projection and view matrices.
*/

#pragma once

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class ViewController 
{
    private:
        // Global variables
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;

        // Cartesian coordinates
        glm::vec3 position;
        
        // Cartesian speed
        float speed;

        // Look angles
        float horizontalAngle;
        float verticalAngle;

        float mouseSpeed;

        // Initial Field of View
        float fov_deg; // [deg]

        // Fill triangles
        bool fillTriangles;
        bool fKeyPressed;


    public:
        // Constructor
        ViewController(const glm::vec3& position, const float horizontalAngle,  const float verticalAngle, const float speed, const float mouseSpeed, const float fov);

        // Actualize the matrices from the user inputs
        void computeMatricesFromInputs();

        // Get the view matrix
        glm::mat4 getViewMatrix();

        // Get the projection matrix
        glm::mat4 getProjectionMatrix();

        int getRenderingMode();

        glm::vec3 getPosition();
};