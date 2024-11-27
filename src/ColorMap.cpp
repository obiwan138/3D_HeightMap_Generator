/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 11/27/2024

Description:
This file is the cpp implementation file of the ColorMap class. It contains the methods to construct the class, and color the vertices of the terrain.
*/

#include "ColorMap.hpp"

/**
 * @author Thomas Etheve
 * @brief Default constructor
 */
ColorMap::ColorMap()
{
    this->type = ColorMapType::GRAY_SCALE;
    this->minAlt = -1.f;
    this->maxAlt = 1.f;
}

/**
 * @author Thomas Etheve
 * @brief Constructor
 * @param type : colormap type
 * @param minAlt : minimum altitude
 * @param maxAlt : maximum altitude
 */
ColorMap::ColorMap(ColorMapType type, float minAlt, float maxAlt)
{
    this->type = type;
    this->minAlt = minAlt;
    this->maxAlt = maxAlt;
}

/**
 * @author Thomas Etheve
 * @brief Get the color vector corresponding to the vertices vector
 * @param vertices : vector of vertices
 */

std::vector<glm::vec3> ColorMap::getColorVector(const std::vector<glm::vec3>& vertices)
{
    // Create the color vector to return
    std::vector<glm::vec3> colors;

    // Use the appropriate function depending on the colormap type
    switch (this->type)
    {
        // Color with Earth Gist colormap
        case ColorMapType::GIST_EARTH:
            this->getEarthLevels(vertices, colors);
            break;

        // Color with Monochrome levels colormap
        case ColorMapType::GRAY_SCALE:
            this->getGrayScale(vertices, colors);
            break;
    }

    // Return the color vector
    return colors;
}

/**
 * @author Thomas Etheve
 * @brief Get the color vector corresponding to monorchrome levels
 * @param vertices : vector of vertices
 * @param colors : vector of colors
 */

void ColorMap::getGrayScale(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors)
{
    // Colors
    glm::vec3 black = glm::vec3(0, 0, 0);   // black
    glm::vec3 white = glm::vec3(1, 1, 1);   // white

    // Associate a color to each vertex
    for (const auto& vertex : vertices)
    {
        // Push back the color corresponding to the interpolation of the altitude value
        colors.push_back(interpolateColors(this->minAlt, this->maxAlt, black, white, vertex.y));
    }
}

/**
 * @brief Get the color vector corresponding to Earth Gist levels
 * @param vertices : vector of vertices
 * @param colors : vector of colors
 */
void ColorMap::getEarthLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors)
{
    // Set the color steps 
    glm::vec3 darkBlue = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 lightBlue = glm::vec3(0.06f, 0.89f, 1.f);
    glm::vec3 ligthGreen = glm::vec3(0.3f, 1.f, 0.3f);
    glm::vec3 brown = glm::vec3(0.5f,0.27f,0.11f);
    glm::vec3 gray = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 white = glm::vec3(1.f, 1.f, 1.f);

    // Associate a color to each vertex by successive altitude levels
    for (const auto& vertex : vertices)
    {
        // Set dark blue to every point below the sea level
        if (vertex.y < SEA_LEVEL*this->minAlt)
        {
            colors.push_back(darkBlue);
        }
        // Interpolate the colors between green and brown for altitudes in Land levels
        else if (vertex.y < MAX_LAND*this->maxAlt)
        {
            colors.push_back(interpolateColors(SEA_LEVEL*this->minAlt, MAX_LAND*this->maxAlt, ligthGreen, brown, vertex.y));
        }
        // Interpolate the colors between brown and grey for altitudes in Mountain levels
        else if (vertex.y < MAX_MOUNTAIN*this->maxAlt)
        {
            colors.push_back(interpolateColors(MAX_LAND*this->maxAlt, MAX_MOUNTAIN*this->maxAlt, brown, gray, vertex.y));
        }
        // Interpolate the colors between grey and white for altitudes in Snow levels
        else if (vertex.y < MAX_ALT*this->maxAlt)
        {
            colors.push_back(interpolateColors(MAX_MOUNTAIN*this->maxAlt, MAX_ALT*this->maxAlt, gray, white, vertex.y));
        }
        // Set white color to every point higher than the snow level
        else if (vertex.y >= MAX_ALT*this->maxAlt)
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
    // Return the interpolated color using the linear interpolation formula
    return (c1 + ((y - y1)/(y2 - y1)) * (c2 - c1));
}

/**
 * @brief Destructor
 */
ColorMap::~ColorMap(){} 