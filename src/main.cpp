/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:
This is the main loop for this program. It loads in all of the objects, runs the draw loop, and updates the view based on user inputs.
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
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include SFML
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

// Include boost
#include <boost/program_options.hpp>

// Include project header files
#include <common/shader.hpp>
#include "ChunkManager.hpp"
#include "ViewController.hpp"
#include "ColorMap.hpp"
#include "Chunk.hpp"

namespace po = boost::program_options;

void printBuffer(GLuint VBO, GLuint EBO, GLsizei vertexCount, GLsizei indexCount);

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
            ("octaves,o", po::value<int>()->default_value(8), "set number of octaves for fractal perlin noise")
            ("seed", po::value<uint32_t>()->default_value(std::rand()), "set 64bit seed for perlin noise")
            ("freq-start", po::value<double>()->default_value(0.05), "set starting frequency for fractal perlin noise")
            ("freq-rate", po::value<double>()->default_value(2), "set frequency rate for fractal perlin noise")
            ("amp-rate", po::value<double>()->default_value(0.5), "set amplitude decay rate for fractal perlin noise")
            ("mode, m", po::value<int>()->default_value(0), "Noise mode (0 - fractal, 1 - turbulent, 2 - opalescent, 3 - gradient weighting)")
			("max, m", po::value<double>()->default_value(5), "Noise max value")
        ;

		// Store program options
        po::store(po::parse_command_line(argc, argv, desc), arguments);
        po::notify(arguments);

        if (arguments.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }
    }
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
	 * Initialize the SFML Window
	 ********************************************************************/

	// Create the window with OpenGL settings
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 0;

	// Window creation
    sf::RenderWindow window(sf::VideoMode(1280, 760), "2D Height Map", sf::Style::Default, settings);
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
	 * SCENE MECHANICS
	 ********************************************************************/

	// SHADERS : Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// MATRICES : Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	
	/********************************************************************
	 * View Control
	 ********************************************************************/

	// Create the view controller object
	glm::vec3 position = glm::vec3(0.f, 5.f, 0.f);					// User's position
	float horizontalAngle = 0.f; 	float verticalAngle = 0.f;	// Look angles
	float speed = 5.f;				float mouseSpeed = 0.005f;		// Speed and Mouse sensitivity
	float fov = 45.f;												// Field of view (degrees)
	
	// ViewController class instance
	ViewController viewController(window.getSize(),
								  position, speed,
								  horizontalAngle,  verticalAngle, mouseSpeed, 
								  fov);

	// Color map
	ColorMap colorMap(ColorMapType::GIST_EARTH, -1.f*arguments["max"].as<double>(), arguments["max"].as<double>());

	/********************************************************************
	 * Create hight map
	 ********************************************************************/

	// Bounds
	float LENGTH_X = 15;
	float LENGTH_Z = 15;
	float resolution = 0.25f;

	unsigned int SIZE_X = static_cast<unsigned int>(LENGTH_X / resolution);
	unsigned int SIZE_Z = static_cast<unsigned int>(LENGTH_Z / resolution);

	const unsigned int NUM_STRIPS = SIZE_Z-1;
	const unsigned int NUM_TRIANGLES_PER_STRIP = (SIZE_X-1)*2;
	const unsigned int NUM_VERTS_PER_STRIP = SIZE_X*2;

	ChunkManager manager(1, arguments["seed"].as<uint32_t>(), LENGTH_X, resolution, &colorMap, arguments);
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

		// Compute the Model View Projection matrix
		glm::mat4 MVP = viewController.getProjectionMatrix() 
						* viewController.getViewMatrix() 
						* glm::mat4(1.0f);		

		// Render the chunks in 3D or 2D mode
		if(viewController.getViewMode2D())
		{
			window.pushGLStates();

			
			window.clear();
			manager.drawChunks(&window);
        	

			// Restore the OpenGL states (bing back to the current context)
			window.popGLStates();
		}
		else
		{
			// Use the shader program
			glUseProgram(programID);

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Change the triangles rendering mode by pressing the key P ( GL_FILL (Filled Triangles) or GL_LINE (Wireframe))
			glPolygonMode(GL_FRONT_AND_BACK, viewController.getRenderingMode()); 		

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

// Check the buffers

void printBuffer(GLuint VBO, GLuint EBO, GLsizei vertexCount, GLsizei indexCount) {
    // Bind VBO and read vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::vector<float> vertexData(vertexCount * 3); // Assuming glm::vec3 (3 floats per vertex)
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), vertexData.data());

    std::cout << "Vertex Buffer Object (VBO) Data:" << std::endl;
    for (GLsizei i = 0; i < vertexCount; ++i) {
        std::cout << "Vertex " << i << ": ("
                  << vertexData[i * 3] << ", "
                  << vertexData[i * 3 + 1] << ", "
                  << vertexData[i * 3 + 2] << ")" << std::endl;
    }

    // Bind EBO and read index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    std::vector<GLuint> indexData(indexCount);
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexData.size() * sizeof(GLuint), indexData.data());

    std::cout << "Element Buffer Object (EBO) Data:" << std::endl;
    for (GLsizei i = 0; i < indexCount; ++i) {
        std::cout << "Index " << i << ": " << indexData[i] << std::endl;
    }
}

