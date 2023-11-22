#include "Engine.h"

void Engine::init_camera(Program& shader)
{
	glUseProgram(shader);
	campos = glGetUniformLocation(shader, "campos");
	glUniform3fv(campos, 1, glm::value_ptr(camera));
}

void Engine::init_light(Program& shader)
{
	glUseProgram(shader);
	lightpos = glGetUniformLocation(shader, "lightpos");
	glUniform3fv(lightpos, 1, glm::value_ptr(light));
}

Direction Engine::get_direction(double press_xpos, double press_ypos, double release_xpos, double release_ypos)
{
	if (release_xpos > press_xpos + 20.0) { return Direction::RIGHT; }
	else if (release_xpos < press_xpos - 20.0) { return Direction::LEFT; }
	else if (release_ypos > press_ypos + 20.0) { return Direction::DOWN; }
	else if (release_ypos < press_ypos - 20.0) { return Direction::UP; }
	else { return Direction::NONE; }
}

std::pair<GridPosition, GridPosition> Engine::get_grid_indices_from_mouse(double xpos, double ypos)
{
	return std::pair<GridPosition, GridPosition>(ceil((ypos - GRID_START_AT) / BALL_BOX_WIDTH - 1), ceil((xpos - GRID_START_AT) / BALL_BOX_WIDTH) - 1);
}

void Engine::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		std::cout << "-------------Registered New Move!---------------\n\n";
		if (action == GLFW_PRESS and !is_selected)
		{
			glfwGetCursorPos(window, &press_xpos, &press_ypos);
			is_selected = true;
			selected = get_grid_indices_from_mouse(press_xpos, press_ypos);
			std::cout << "You selected: " << "[" << press_xpos << ", " << press_ypos << "] -> [" << selected.first << ", " << selected.second << "]" << '\n';
		}
		glfwGetCursorPos(window, &release_xpos, &release_ypos);
		if (is_selected and get_direction(press_xpos, press_ypos, release_xpos, release_ypos) != Direction::NONE)
		{
			Direction dir = get_direction(press_xpos, press_ypos, release_xpos, release_ypos);
			auto test = get_grid_indices_from_mouse(release_xpos, release_ypos);
			is_selected = false;
			std::cout << "Trying to swap: " << "[" << selected.first << ", " << selected.second << "] -> [" << test.first << ", " << test.second << "]" << '\n';
			grid->make_move(selected.first, selected.second, dir);
		}
	}
}

void Engine::wrapper_mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	Engine* engine = static_cast <Engine*>(glfwGetWindowUserPointer(window));
	if (engine != nullptr) { engine->mouse_callback(window, button, action, mods); }
}

Engine::Engine(GLFWwindow* window, Program shader, Program fbo_shader) : 
	shader(shader), fbo_shader(fbo_shader)
{
	grid = new Grid(shader);
	camera = SpacePosition(0, 0, 0);
	light = SpacePosition(-5, -5, -5);

	wall = new Object(
		{
			-120.0f, 120.0f, -10.0f,
			-120.0f, -120.0f, -10.0f,
			120.0f, 120.0f, -10.0f,
			120.0f, -120.0f, -10.0f
		},
		{
			0.41f, 0.41f, 0.41f,
			0.41f, 0.41f, 0.41f,
			0.41f, 0.41f, 0.41f,
			0.41f, 0.41f, 0.41f,
		},
		{
			0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
		},
		{
			0, 1, 2,
			2, 1, 3
		}, shader
	);

	//glGenVertexArrays(1, &quadVAO);
	//glGenBuffers(1, &quadVBO);
	//glBindVertexArray(quadVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	init_camera(shader);
	init_light(shader);

	glfwSetWindowUserPointer(window, this);
	glfwSetMouseButtonCallback(window, Engine::wrapper_mouse_callback);

	/*glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer error: " << fboStatus << '\n';
	}*/
}

void Engine::light_move(SpacePosition point) 
{
	light = point;
	lightpos = glGetUniformLocation(shader, "lightpos");
	glUniform3fv(lightpos, 1, glm::value_ptr(light));
}

void Engine::game_loop()
{
	/*glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(72.0f / 255.0f, 209.0f / 255.0f, 204.0f / 255.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(fbo_shader);
	glBindVertexArray(quadVAO);
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);*/

	glUseProgram(shader);

	grid->draw();
	grid->rotate(1.f);
	wall->draw();

	angle += adder;
	if (angle >= 360) { angle = 0.0; }
	light_move(SpacePosition(cosf(angle) * 50, sinf(angle) * 50, 0));
}

Engine::~Engine()
{
	delete grid, wall;
}
