/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 11/27/2024

Description:
This file is the header file of the ColorMap class. It contains the methods to construct the class, and color the vertices of the terrain.
*/

#pragma once

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <vector>

/**
 * @author Thomas Etheve
 * @enum ColorMapType
 * @brief Enumerate the different colormap types
 */
enum class ColorMapType
{
    GIST_EARTH,     // Imitates the colors used to represent altitude on Geographical maps
    GRAY_SCALE,     // Gray scale colors
};

// Constants (as ratio of min and max altitudes)
#define SEA_LEVEL 0.3f        // multiplies minAlt, which is currently negative
#define MAX_LAND 0.1f         // multiplies maxAlt
#define MAX_MOUNTAIN 0.3f     // multiplies maxAlt
#define MAX_ALT 0.6f          // multiplies maxAlt

/**
 * @author Thomas Etheve
 * @class ColorMap
 * @brief This class is used to color the vertices of the terrain according to their altitude
 */
class ColorMap
{
    private:
        // Color map type
        ColorMapType type;

        // Minimum and maximum value for color scale
        float minAlt;
        float maxAlt;

    public:
        // Default constructor
        ColorMap();

        // Constructor
        ColorMap(ColorMapType type, float minAlt, float maxAlt);

        // Get the color vector corresponding to the vertices vector
        std::vector<glm::vec3> getColorVector(const std::vector<glm::vec3>& vertices);

        // Get the color vector corresponding to monorchrome levels
        void getGrayScale(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors);

        // Get the color vector corresponding to monorchrome levels
        void getEarthLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors);

        // Interpolate between two colors
        glm::vec3 interpolateColors(const float& y1, const float& y2, const glm::vec3& c1, const glm::vec3& c2, const float& y);

        // Destructor
        ~ColorMap();
};

