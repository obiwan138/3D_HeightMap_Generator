/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:
This is the main loop for this program. It loads in all of the objects, runs the draw loop, and updates the view based on user inputs.
*/

#pragma once

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <vector>

enum class ColorMapType
{
    GIST_EARTH,
    MONOCHROME_LEVELS,
};

#define SEA_LEVEL 0.f
#define MAX_LAND 0.33f
#define MAX_MOUNTAIN 0.66f
#define MAX_ALT 1.f

class ColorMap
{
    private:
        // Color map type
        ColorMapType type;
        glm::vec3 MonochromeColor;

    public:
        // Default constructor
        ColorMap();

        // Constructor
        ColorMap(ColorMapType type);

        // Change the colormap type
        void changeType(ColorMapType type);

        // Get the color vector corresponding to the vertices vector
        std::vector<glm::vec3> getColorVector(const std::vector<glm::vec3>& vertices);

        // Get the color vector corresponding to monorchrome levels
        void getMonochromeLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors);

        // Get the color vector corresponding to monorchrome levels
        void getEarthLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors);

        // Interpolate between two colors
        glm::vec3 interpolateColors(const float& y1, const float& y2, const glm::vec3& c1, const glm::vec3& c2, const float& y);

        // Destructor
        ~ColorMap();
};

