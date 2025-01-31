/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 11/28/2024

Description:
This is the main loop for this program. It intiates the chunk manager to draw/render and manage chunks of the heigth map in 3D or 2D mode. 
Also, a view controller allows the user to move around the map and change the rendering mode. This program has several arguments. Use the 
shell scripts to run the program with the desired arguments.

*/

// Include libraries
//////////////////////////////////////////////////////////////////////////////////////

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <utility>
#include <iostream>
#include <omp.h>

// Include GLEW
#include <GL/glew.h>						// Init Open GL states
#include <glm/glm.hpp>						// Open GL Math library
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include SFML
#include <SFML/Graphics.hpp>				// SFML 2D rendering
#include <SFML/Window.hpp>					// SFML Window creation and management
#include <SFML/OpenGL.hpp>					// SFML OpenGL integration

// Include boost
#include <boost/program_options.hpp>		// Command line arguments parsing

// Include project header files
#include <common/shader.hpp>				// Load Shaders

// Include project classes
#include "ChunkManager.hpp"
#include "ViewController.hpp"
#include "ColorMap.hpp"
#include "Chunk.hpp"

// Program option namespace
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	/********************************************************************
	 * Parse command line arguments
	 ********************************************************************/
	std::srand(time(NULL)); // Seed random number generator for 
	po::variables_map arguments;  
    try {
		// Define all program options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "print help")
            ("size,s", po::value<size_t>()->default_value(100), "set N, the width of each chunk. Each chunk will be size NxN")
			("resolution,r", po::value<double>()->default_value(0.25), "set the plane resolution of the height map")
			("visibility,v", po::value<unsigned int>()->default_value(1), "set the number of chunks visible in each direction")
			("width,x", po::value<unsigned int>()->default_value(1280), "Width of the window")
			("height,y", po::value<unsigned int>()->default_value(720), "Height of the window")
            ("octaves,o", po::value<int>()->default_value(8), "set number of octaves for fractal perlin noise")
            ("seed", po::value<uint32_t>()->default_value(std::rand()), "set 64bit seed for perlin noise")
            ("freq-start", po::value<double>()->default_value(0.05), "set starting frequency for fractal perlin noise")
            ("freq-rate", po::value<double>()->default_value(2), "set frequency rate for fractal perlin noise")
            ("amp-rate", po::value<double>()->default_value(0.5), "set amplitude decay rate for fractal perlin noise")
            ("mode, m", po::value<int>()->default_value(0), "Noise mode (0 - fractal, 1 - turbulent, 2 - opalescent, 3 - gradient weighting)")
			("max, m", po::value<double>()->default_value(5), "Noise max value")
			("cmap, c", po::value<unsigned int>()->default_value(1), "Color map (0 - GRAY_SCALE, 1 - GIST_EARTH)")
        ;

		// Store program options
        po::store(po::parse_command_line(argc, argv, desc), arguments);
        po::notify(arguments);

		// Print help message if --help is passed
        if (arguments.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }
    }

	// Check the command line arguments are valid
    catch(std::exception& e) {
		// Handle known exceptions
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
		// Handle unknown exceptions
        std::cerr << "Exception of unknown type!\n";
    }

	/********************************************************************
	 * Initialize the SFML Window with OPENGL settings
	 ********************************************************************/

	// Create the window with OpenGL settings
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 0;

	// Window creation
    sf::RenderWindow window(sf::VideoMode(arguments["width"].as<unsigned int>(), arguments["height"].as<unsigned int>()),	// Window size
										 "2D Height Map", 														// Title of the window
										 sf::Style::Default, 													// Default window style
										 settings);																// OpenGL settings
	window.setVerticalSyncEnabled(true);
	window.setVisible(true);				// Make window visible
	window.setActive(true); 				// Create context for OpenGL

	// Hide the mouse cursor and set it to the center of the window
	window.setMouseCursorVisible(false);
	sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

	/********************************************************************
	 * Initialize the OpenGL state machine
	 ********************************************************************/

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		return -1;
	}

	// Dark background
	glClearColor(0.15f, 0.15f, 0.15f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it is closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);  

	/********************************************************************
	 * Load Shaders and MVP matrix
	 ********************************************************************/

	// SHADERS : Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// MATRICES : Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	
	/********************************************************************
	 * View Control
	 ********************************************************************/

	// Initialize the view controller object
	glm::vec3 position = glm::vec3(0.f, 5.f, 0.f);					// User's position
	float horizontalAngle = -3.14f;	float verticalAngle = 0.f; 		// Look angles (Toward -z)
	float speed = 5.f;				float mouseSpeed = 0.005f;		// Speed and Mouse sensitivity
	float fov = 45.f;												// Field of view (degrees)
	
	// ViewController class instance
	ViewController viewController(window.getSize(),
								  position, speed,
								  horizontalAngle,  verticalAngle, mouseSpeed, 
								  fov);

	// Define a mapping between the color map type index (given in argument of the program) and the actual color map type enum
	std::map<unsigned int, ColorMapType> cmapType = {{0, ColorMapType::GRAY_SCALE}, {1, ColorMapType::GIST_EARTH}};

	// Create the color map object
	ColorMap colorMap(cmapType[arguments["cmap"].as<unsigned int>()], // ColorMap type
						-1.f*arguments["max"].as<double>(), 		  // Minimum altitude: Centered on 0 - Max noise
						arguments["max"].as<double>());			      // Maximum altitude: Centered on 0 + Max noise

	/********************************************************************
	 * Create hight map
	 ********************************************************************/

	// Create the chunk manager object (View distance = 3 chunks, color map pointer, using command line arguments)
	ChunkManager manager(&colorMap, arguments);
	std::cout << "manager created" << std::endl;

	/********************************************************************
	 * Main loop
	 ********************************************************************/
	
	// Boolean for the main loop
    bool running = true;

	// Main loop
    while (running)
    {
		//create and destroy chunks as appropriate
		manager.update(viewController.getPosition());

		// Activate the shader program
		glUseProgram(programID);

        /********************************************************************
	 	* Handle closing window event and escape key
	 	********************************************************************/
        sf::Event event;
        while (window.pollEvent(event))
        {
			// Check if the user pressed the escape key
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				// end the program
                running = false;	
			}
			// Check if the user closed the window
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
			// Check if the window was resized
            else if (event.type == sf::Event::Resized)
            {
                // Adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
				viewController.setWindowSize(sf::Vector2u(event.size.width, event.size.height));
            }
        }

		/********************************************************************
	 	* Actualize the scene
	 	********************************************************************/

		// Use the view controller to update the view settins and matrices from user inputs
		viewController.computeMatricesFromInputs(window);

		// Change the triangles rendering mode by pressing the key P ( GL_FILL (Filled Triangles) or GL_LINE (Wireframe))
		glPolygonMode(GL_FRONT_AND_BACK, viewController.getRenderingMode());

		// Compute the Model View Projection matrix
		glm::mat4 MVP = viewController.getProjectionMatrix() 
						* viewController.getViewMatrix() 
						* glm::mat4(1.0f);		

		// Render the chunks in 3D or 2D mode
		if(viewController.getViewMode2D())
		{
			// Push out the OpenGL states
			window.pushGLStates();

			// Clear the window (2D view)
			window.clear();

			// Draw the chunks as 2D maps
			manager.drawChunks(&window);

			// Create the origin as a small red square at then center of the screen
			sf::RectangleShape origin(sf::Vector2f(5,5));						// Rectangle of 5x5 pixels
			origin.setOrigin(2.5, 2.5);											// Origin at the center of the rectangle
			origin.setFillColor(sf::Color::Red);								// Red fill color
			origin.setPosition(window.getSize().x / 2, window.getSize().y / 2); // set position at the center of the window

			// Create a circular shape to locate the user
			sf::CircleShape circle(2.5f);										// Circle of 5 pixels radius
			circle.setOrigin(2.5f, 2.5f); 										// Origin at the center of the circle
			circle.setFillColor(sf::Color(0,0,0,0)); 							// Transparent fill
			circle.setOutlineColor(sf::Color(255,20,147));     					// Pink border
			circle.setOutlineThickness(2.0f);           						// Border thickness

			// Set the position of the circle at the users position
			glm::vec3 userPos = viewController.getPosition();						// Get the user position	
			float chunkSize = static_cast<float>(arguments["size"].as<size_t>());	// Get the chunk size
			circle.setPosition(origin.getPosition().x + userPos.x/static_cast<float>(arguments["resolution"].as<double>()), 	// Set position from the origin (x 3D = x window)
							   origin.getPosition().y + userPos.z/static_cast<float>(arguments["resolution"].as<double>()));	// Set position from the origin (z 3D = y window)

			// Draw the circle
			window.draw(origin);
			window.draw(circle);

			// Restore the OpenGL states (bing back to the current context)
			window.popGLStates();
		}
		else
		{
			// Use the shader program
			glUseProgram(programID);

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// Send the matrix to the shader
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

			// Render the chunks
			manager.renderChunks(&programID);

			// Unbind Open GL states
			glBindVertexArray(0);	// Unbind the VAO
			glUseProgram(0);		// Unbind the shader program
		}

		// End the current frame (internally swaps the front and back buffers of the window)
        window.display();
	}

    // Clean buffers, arrays and shader program
	glDeleteProgram(programID);

	return 0;
}

