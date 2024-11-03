/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 11/03/2024

Description:

View Controller class implementation file. This class handles how the user's point of view is controlled
by the user inputs, managing the projection and view matrices.
*/

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

#include "ViewController.h"

///////////////////////////////////////////////////////////////////////////////
/**
 * @brief Constructor
 * 
 * @arg position: user's initial cartesian coordinates [m, m, m]
 * @arg horizontalAngle : user's horizontal look angle [rad]
 * @arg verticalAngle : user's verticalAngle look angle [rad]
 * @arg speed : user's displacement speed [m/s]
 * @arg mouseSpeed : user's mouse sensitivity [rad/s]
 * @arg fov: Field of view (square) [deg]
 */
ViewController::ViewController(const glm::vec3& position, const float horizontalAngle,  const float verticalAngle, const float speed, const float mouseSpeed, const float fov_deg)
{
    // Define the initial coordinates
	this->position = position;

	// Define speed
	this->speed = speed;			 // Change of position (by the keyboard)
	this->mouseSpeed = mouseSpeed;	 // Change of look angles (by the mouse)

    // Define Fielf of view and Projection matrix
    this->fov_deg = fov_deg;

	// Fill triangles
	this->fillTriangles = false;
	this->fKeyPressed = false;

    // Compute the view matrix
	this->ViewMatrix = glm::lookAt(
								this->position,  			// Camera is here
								glm::vec3(0,0,0),           // and looks here : origin
								glm::vec3(0,1,0)            // Head is up (set to 0,-1,0 to look upside-down)
						    );

	// Compute the look angles
	this->horizontalAngle = horizontalAngle;
	this->verticalAngle = verticalAngle;

    // Compute the projection matrix : 45 deg Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	this->ProjectionMatrix = glm::perspective(glm::radians(this->fov_deg), 4.0f / 3.0f, 0.1f, 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @brief Compute the view and the projection matrices from the user input
 */
void ViewController::computeMatricesFromInputs()
{
    // glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double mousePosX, mousePosY;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	this->horizontalAngle += this->mouseSpeed * float(1024/2 - mousePosX );
	this->verticalAngle   += this->mouseSpeed * float( 768/2 - mousePosY );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(this->verticalAngle) * sin(this->horizontalAngle), 
		sin(this->verticalAngle),
		cos(this->verticalAngle) * cos(this->horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(sin(this->horizontalAngle - 3.14f/2.0f), 0, cos(this->horizontalAngle - 3.14f/2.0f));
	
	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// In-plane front vector 
	glm::vec3 front = glm::normalize(::vec3(direction.x, 0, direction.z));

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS)
	{	// Key Z in AZERTY
		this->position += front * deltaTime * this->speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS)
	{	// Key S in AZERTY
		this->position -= front * deltaTime * this->speed;
	}
	// Move right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS)
	{	// Key D in AZERTY
		this->position += right * deltaTime * this->speed;
	}
	// Move left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS)
	{	// Key Q in AZERTY
		this->position -= right * deltaTime * this->speed;
	}
	// Move up on y axis
	if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS)
	{
		this->position += glm::vec3(0,1,0) * deltaTime * this->speed;
	}
	// Move down on y axis
	if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS)
	{	
		this->position -= glm::vec3(0,1,0) * deltaTime * this->speed;
	}
	// Increase speed
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
	{	
		this->speed += 0.1f;
	}
	// Decrease speed
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS)
	{	
		this->speed -= 0.1f;
		if(this->speed < 1)
		{
			this->speed = 1;
		}
	}
	// Increase FOV
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS)
	{	
		this->fov_deg += 0.1f;
		// Set an upper bound to the FOV
		if(this->fov_deg > 80)
		{
			this->fov_deg = 80;
		}
	}
	// Decrease FOV
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS)
	{	
		this->fov_deg -= 0.1f;
		// Set a lower bound to the FOV
		if(this->fov_deg < 10)
		{
			this->fov_deg = 10;
		}
	}
	// Toggle the fill triangles mode by one press + realase of the F key
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fKeyPressed) 
	{
		this->fillTriangles = !this->fillTriangles;	// Toggle the state
		fKeyPressed = true;  						// Avoid several toggles during a one press
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) 
	{
		fKeyPressed = false;  			// Allow toggling again on next press
	}

	// Actualize the view matrix according to the new position
	ViewMatrix = glm::lookAt(
								this->position,           	// Camera is here
								this->position + direction, // and looks here : at the same position, plus "direction"
								up                  		// Head is up (set to 0,-1,0 to look upside-down)
							);

	// Actualize the projection matrix : Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(this->fov_deg), 4.0f / 3.0f, 0.1f, 100.0f);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

///////////////////////////////////////////////////////////////////////////////
/**
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
 * @brief Get the projection matrix
 * @return glm::mat4
 */
glm::mat4 ViewController::getProjectionMatrix()
{
	return ProjectionMatrix;
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @brief Get the view matrix
 * @return glm::mat4
 */
glm::mat4 ViewController::getViewMatrix()
{
	return ViewMatrix;
}