/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 11/03/2024

Description:

View Controller class implementation file. This class handles how the user's point of view is controlled
by the user inputs, managing the projection and view matrices.
*/

#include "ViewController.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Constructor
 * 
 * @param windowSize : window size
 * @param position: user's initial cartesian coordinates [m, m, m]
 * @param speed : user's displacement speed [m/s]
 * @param horizontalAngle : user's horizontal look angle [rad]
 * @param verticalAngle : user's verticalAngle look angle [rad]
 * @param mouseSpeed : user's mouse sensitivity [rad/s]
 * @param fov: Field of view (square) [deg]
 */
ViewController::ViewController(const sf::Vector2u& windowSize,
							   const glm::vec3& position, const float speed,
							   const float horizontalAngle,  const float verticalAngle, const float mouseSpeed, 
							   const float fov_deg)
{
	// Define the screen display aspect
	this->windowSize = windowSize;	 // Window size
	this->fillTriangles = false;	 // Wiremesh : do not fill triangles at first
	this->fKeyPressed = false;		 // F key not pressed at first
	this->viewMode2D = false;		 // 2D view at first
	this->vKeyPressed = false; // Right Shift key not pressed at first

	// Define the initial coordinates
	this->position = position;					// Initial position
	this->horizontalAngle = horizontalAngle;	// Initial horizontal angle
	this->verticalAngle = verticalAngle;		// Initial vertical angle

	this->ViewMatrix = glm::lookAt(				// Set up the camera matrix
								this->position,  			// Camera is here
								glm::vec3(0,0,0),           // and looks here : origin
								glm::vec3(0,1,0)            // Head is up (set to 0,-1,0 to look upside-down)
						    );

	// Set up the movement parameters
	this->speed = speed;			 // Change of position (by the keyboard)
	this->mouseSpeed = mouseSpeed;	 // Change of look angles (by the mouse)

    // Define the projection parameters
	this->orthographicProjection = false;	// Orthographic projection mode
	this->perspectiveProjection = true;		// Perspective projection mode
    this->fov_deg = fov_deg;				// Field of view [deg]
	this->aspectRatio = 4.0f / 3.0f;		// Aspect ratio
	this->nearPlane = 0.1f;					// Near plane distance [m]
	this->farPlane = 300.0f;				// Far plane distance [m]	
	this->top = 20.0f;						// Top plane distance [m]
	this->bottom = -20.0f;					// Bottom plane distance [m]
	this->left = -30.0f;					// Left plane distance [m]
	this->right = 30.0f;					// Right plane distance [m]

	// Set the perspective projection matrix
	this->ProjectionMatrix = glm::perspective(glm::radians(this->fov_deg),  		// Field of view
												this->aspectRatio, 					// Aspect ratio
												this->nearPlane, this->farPlane);	// Near and far planes bounds
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Compute the view and the projection matrices from the user input
 */
void ViewController::computeMatricesFromInputs()
{	
	// Update the view mode (2D or 3D)
	this->updateViewMode();

	// Stuff to do in 2D mode
	if(this->viewMode2D)
	{

	}
	// Stuff to do in 3D mode
	else
	{
		// Time difference between current and last frame
		float dt = (this->clock.restart()).asSeconds();

		// Actualize the user look angles and the projection matrix
		this->updateLook();

		// Actualize the user movement and the view matrix matrix
		this->updateMove(dt);

		// Actualize the triangle rendering mode
		this->updateTriangleRendering();
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Update the user look angles according to the user inputs
 */

void ViewController::updateLook()
{
	// Get mouse position
	sf::Vector2i mousePosition = sf::Mouse::getPosition();
	double mousePosX = mousePosition.x;
	double mousePosY = mousePosition.y;

	// Reset mouse position for the next frame
	sf::Mouse::setPosition(sf::Vector2i(this->windowSize.x / 2, this->windowSize.y / 2));

	// Compute new orientation
	this->horizontalAngle += this->mouseSpeed * float(this->windowSize.x/2 - mousePosX );
	this->verticalAngle   += this->mouseSpeed * float(this->windowSize.y/2 - mousePosY );

	// Set bounds to the vertical angle [-pi/2, pi/2] to avoid flipping
	if(this->verticalAngle > 3.14f/2)
	{
		this->verticalAngle = 3.14f/2;
	}
	else if(this->verticalAngle < -3.14f/2)
	{
		this->verticalAngle = -3.14f/2;
	}

	// Increase FOV
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) //(glfwGetKey( window3D, GLFW_KEY_UP ) == GLFW_PRESS)
	{	
		this->fov_deg += 0.1f;
		// Set an upper bound to the FOV
		if(this->fov_deg > 80)
		{
			this->fov_deg = 80;
		}
	}
	// Decrease FOV
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) //(glfwGetKey( window3D, GLFW_KEY_DOWN ) == GLFW_PRESS)
	{	
		this->fov_deg -= 0.1f;
		// Set a lower bound to the FOV
		if(this->fov_deg < 10)
		{
			this->fov_deg = 10;
		}
	}

	// If O is presses, switch to orthographic projection
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) 
	{	
		// Set the orthographic projection flag on
		this->orthographicProjection = true;
		// Set the perspective projection flag off
		this->perspectiveProjection = false;

		// Notify the user in command line
		if(this->orthographicProjection)
		{
			std::cout << "Changed projection type into ORTHOGRAPHIC" << std::endl;
		}
	}

	// If P is presses, switch to perspective projection
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) 
	{	
		// Set the orthographic projection flag off
		this->orthographicProjection = false;
		// Set the perspective projection flag on
		this->perspectiveProjection = true;

		// Notify the user in command line
		if(this->perspectiveProjection)
		{
			std::cout << "Changed projection type into PERSPECTIVE" << std::endl;
		}
	}

	// If R is pressed, reset position and look angles
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		// Change the projection mode to perspective
		this->perspectiveProjection = true;
		this->orthographicProjection = false;

		// Reset the position and look angles
		this->position = glm::vec3(0, 5, 0);
		this->horizontalAngle = 0.f;
		this->verticalAngle = 0.f;
		this->speed = 5.f;
		this->fov_deg = 45;
	}

	// Compute the projection matrix according to the projection mode
	if (this->orthographicProjection)
	{
		// Set the orthographic projection matrix
		this->ProjectionMatrix = glm::ortho(this->left, this->right, 			// Left and right bounds
											this->bottom, this->top, 			// Bottom and top bounds
											this->nearPlane, this->farPlane);   // Near and far planes bounds
	}
	else if(this->perspectiveProjection)
	{
		// Set the perspective projection matrix
		this->ProjectionMatrix = glm::perspective(glm::radians(this->fov_deg),  		// Field of view
												  this->aspectRatio, 					// Aspect ratio
												  this->nearPlane, this->farPlane);	    // Near and far planes bounds
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Update the user position and velocity according to the user inputs
 * and update the view matrix
 * 
 * @param dt : time difference between current and last frame
 */
void ViewController::updateMove(float dt)
{
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 lookingDirection(
		cos(this->verticalAngle) * sin(this->horizontalAngle), 
		sin(this->verticalAngle),
		cos(this->verticalAngle) * cos(this->horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(sin(this->horizontalAngle - 3.14f/2.0f), 0, cos(this->horizontalAngle - 3.14f/2.0f));
	
	// Up vector
	glm::vec3 up = glm::cross(right, lookingDirection);

	// In-plane front vector 
	glm::vec3 front = glm::normalize(glm::vec3(lookingDirection.x, 0, lookingDirection.z));

	// Move forward
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) //glfwGetKey( window3D, GLFW_KEY_W ) == GLFW_PRESS)
	{	// Key Z in AZERTY
		this->position += front * dt * this->speed;
	}
	// Move backward
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) //(glfwGetKey( window3D, GLFW_KEY_S ) == GLFW_PRESS)
	{	// Key S in AZERTY
		this->position -= front * dt * this->speed;
	}
	// Move right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) //(glfwGetKey( window3D, GLFW_KEY_D ) == GLFW_PRESS)
	{	// Key D in AZERTY
		this->position += right * dt * this->speed;
	}
	// Move left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) //(glfwGetKey( window3D, GLFW_KEY_A ) == GLFW_PRESS)
	{	// Key Q in AZERTY
		this->position -= right * dt * this->speed;
	}
	// Move up on y axis
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) //(glfwGetKey( window3D, GLFW_KEY_SPACE ) == GLFW_PRESS)
	{
		this->position += glm::vec3(0,1,0) * dt * this->speed;
	}
	// Move down on y axis
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) //(glfwGetKey( window3D, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS)
	{	
		this->position -= glm::vec3(0,1,0) * dt * this->speed;
	}
	// Increase speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) //(glfwGetKey( window3D, GLFW_KEY_RIGHT ) == GLFW_PRESS)
	{	
		this->speed += 0.1f;
	}
	// Decrease speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) // (glfwGetKey( window3D, GLFW_KEY_LEFT ) == GLFW_PRESS)
	{	
		this->speed -= 0.1f;
		if(this->speed < 1)
		{
			this->speed = 1;
		}
	}

	// Actualize the view matrix according to the new position
	this->ViewMatrix = glm::lookAt(
									this->position,           			// Camera is here
									this->position + lookingDirection,  // and looks here : at the same position, plus "direction"
									up                  				// Head is up (set to 0,-1,0 to look upside-down)
									);
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Toggle the view mode according to the user inputs
 */
void ViewController::updateViewMode()
{
	// Check if Right Shift key is pressed and wasn't pressed before
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::V) && !this->vKeyPressed)
	{
		// Toggle the view mode
		this->viewMode2D = !(this->viewMode2D);
		this->vKeyPressed = true;

		// Notify the user in command line
		if(this->viewMode2D)
		{
			std::cout << "Changed view mode into 2D" << std::endl;
		}
		else
		{
			std::cout << "Changed view mode into 3D" << std::endl;
		}
	}
	// Check if Right Shift key is released to reset the flag
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::V) && this->vKeyPressed)
	{
		this->vKeyPressed = false; // Reset flag to allow toggling on the next press
	}
}
///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Toggle the triangle rendering mode according to the user inputs
 */
void ViewController::updateTriangleRendering()
{
    // Check if F key is pressed and wasn't pressed before
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !this->fKeyPressed)
    {
        this->fillTriangles = !this->fillTriangles; // Toggle the state
        this->fKeyPressed = true;  // Set flag to prevent multiple toggles during a press

		// Notify the user in command line
		if(this->fillTriangles)
		{
			std::cout << "Changed triangle rendering into FILL" << std::endl;
		}
		else
		{
			std::cout << "Changed triangle rendering into WIRE MESH" << std::endl;
		}
    }
    // Check if F key is released to reset the flag
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F) && this->fKeyPressed)
    {
        this->fKeyPressed = false; // Reset flag to allow toggling on the next press
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Get the window size
 * @return sf::Vector2u
 */
sf::Vector2u ViewController::getWindowSize()
{
	return this->windowSize;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Set the window size
 * @param windowSize : window size
 */
void ViewController::setWindowSize(const sf::Vector2u& windowSize)
{
	this->windowSize = windowSize;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Get the rendering mode
 * @return int
 */
int ViewController::getRenderingMode()
{
	if(this->fillTriangles)
	{
		return GL_FILL;
	}
	else
	{
		return GL_LINE;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Get the 2D view mode
 * @return bool
 */
bool ViewController::getViewMode2D()
{
	return this->viewMode2D;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Get the projection matrix
 * @return glm::mat4
 */
glm::mat4 ViewController::getProjectionMatrix()
{
	return ProjectionMatrix;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Get the view matrix
 * @return glm::mat4
 */
glm::mat4 ViewController::getViewMatrix()
{
	return ViewMatrix;
}