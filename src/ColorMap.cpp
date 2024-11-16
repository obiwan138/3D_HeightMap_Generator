/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:
This is the main loop for this program. It loads in all of the objects, runs the draw loop, and updates the view based on user inputs.
*/

#include "ColorMap.hpp"

/**
 * @brief Default constructor
 */
ColorMap::ColorMap()
{
    this->type = ColorMapType::MONOCHROME_LEVELS;
    this->MonochromeColor = glm::vec3(1, 1, 1);
}

/**
 * @brief Constructor
 * @param type : colormap type
 */
ColorMap::ColorMap(ColorMapType type)
{
    this->type = type;
    this->MonochromeColor = glm::vec3(1, 1, 1);
}

/**
 * @brief Get the color vector corresponding to the vertices vector
 * @param vertices : vector of vertices
 */

std::vector<glm::vec3> ColorMap::getColorVector(const std::vector<glm::vec3>& vertices)
{
    // Create the color vector
    std::vector<glm::vec3> colors;

    switch (this->type)
    {
        case ColorMapType::GIST_EARTH:
            getEarthLevels(vertices, colors);
            break;

        case ColorMapType::MONOCHROME_LEVELS:
            getMonochromeLevels(vertices, colors);
            break;
    }

    // Return the color vector
    return colors;
}

/**
 * @brief Get the color vector corresponding to monorchrome levels
 * @param vertices : vector of vertices
 * @param colors : vector of colors
 */

void ColorMap::getMonochromeLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors)
{
    // Get the min and max values
    float min = 0;
    float max = 0;
    for (const auto& vertex : vertices)
    {
        if (vertex.y < min)
        {
            min = vertex.y; // If the vertex y-coord is below the min, set the min to the vertex
        }
        if (vertex.y > max)
        {
            max = vertex.y; // If the vertex y-coord is above the max, set the max to the vertex
        }
    }

    // Associate a color to each vertex
    for (const auto& vertex : vertices)
    {
        // Compute the color
        float value = (vertex.y - min) / (max - min);
        colors.push_back(glm::vec3(value, value, value));
    }
}

/**
 * @brief Get the color vector corresponding to Earth Gist levels
 * @param vertices : vector of vertices
 * @param colors : vector of colors
 */
void ColorMap::getEarthLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors)
{
    // Get the min and max values
    float min = 0;
    float max = 0;

    for (const auto& vertex : vertices)
    {
        if (vertex.y < min)
        {
            min = vertex.y; // If the vertex y-coord is below the min, set the min to the vertex
        }
        if (vertex.y > max)
        {
            max = vertex.y; // If the vertex y-coord is above the max, set the max to the vertex
        }
    }

    // Set the color steps 
    glm::vec3 darkBlue = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 lightBlue = glm::vec3(0.06f, 0.89f, 1.f);
    glm::vec3 ligthGreen = glm::vec3(0.3f, 1.f, 0.3f);
    glm::vec3 brown = glm::vec3(0.5f,0.27f,0.11f);
    glm::vec3 gray = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 white = glm::vec3(1.f, 1.f, 1.f);

    // Associate a color to each vertex
    for (const auto& vertex : vertices)
    {
        if (vertex.y < SEA_LEVEL)
        {
            colors.push_back(interpolateColors(min, SEA_LEVEL, darkBlue, lightBlue, vertex.y));
        }
        else if (vertex.y < MAX_LAND*max)
        {
            colors.push_back(interpolateColors(SEA_LEVEL, MAX_LAND*max, ligthGreen, brown, vertex.y));
        }
        else if (vertex.y < MAX_MOUNTAIN*max)
        {
            colors.push_back(interpolateColors(MAX_LAND*max, MAX_MOUNTAIN*max, brown, gray, vertex.y));
        }
        else if (vertex.y <= MAX_ALT*max)
        {
            colors.push_back(interpolateColors(MAX_MOUNTAIN*max, MAX_ALT*max, gray, white, vertex.y));
        }
        else // Default color
        {
            colors.push_back(white);
        }
    }
}

/**
 * @brief Interpolate between two colors
 * @param y1 : first y value
 * @param y2 : second y value
 * @param c1 : first color
 * @param c2 : second color
 * @param y : y value
 */
glm::vec3 ColorMap::interpolateColors(const float& y1, const float& y2, const glm::vec3& c1, const glm::vec3& c2, const float& y)
{
    return (c1 + ((y - y1)/(y2 - y1)) * (c2 - c1));
}

/**
 * @brief Destructor
 */
ColorMap::~ColorMap(){} 