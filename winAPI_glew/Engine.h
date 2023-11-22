#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Grid.h"
#include "Object.h"

typedef glm::vec3 SpacePosition;

class Engine
{
private:
	Grid* grid;
	Object* wall;

	float angle = 0.0f, adder = 0.01f;
	/* Shader stuff */
	/*Buffer quadVAO, quadVBO;
	Buffer fbo, fboTexture, rbo;

	float vertices[24] = {
		1.0, -1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0, 0.0,
		-1.0, 1.0, 0.0, 1.0,

		1.0, 1.0, 1.0, 1.0,
		1.0, -1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0, 1.0
	};*/

	Program shader, fbo_shader;
	
	SpacePosition camera, light;
	Location campos, lightpos;

	void init_camera(Program& shader);
	void init_light(Program& shader);

	/* Game Mechanics */
	std::pair <GridPosition, GridPosition> selected;
	Status is_selected = false;
	double press_xpos, press_ypos;
	double release_xpos, release_ypos;

	Direction get_direction(double press_xpos, double press_ypos, double release_xpos, double release_ypos);

	std::pair <GridPosition, GridPosition> get_grid_indices_from_mouse(double xpos, double ypos);

	void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void wrapper_mouse_callback(GLFWwindow* window, int button, int action, int mods);
public:
	Engine() = delete;
	Engine(GLFWwindow* window, Program shader, Program fbo_shader);

	void light_move(SpacePosition point);

	void game_loop();

	~Engine();
};

