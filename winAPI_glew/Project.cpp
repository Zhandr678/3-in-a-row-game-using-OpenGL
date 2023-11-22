#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Engine.h"
#include "constants.h"
#include "Shader.h"

Program create_shader(const char* vs, const char* fs)
{
	Program shp = glCreateProgram();
	Program vsh = glCreateShader(GL_VERTEX_SHADER);

	int vshlen = strlen(vs);
	glShaderSource(vsh, 1, &vs, &vshlen);
	glCompileShader(vsh);
	glAttachShader(shp, vsh);

	Program fsh = glCreateShader(GL_FRAGMENT_SHADER);

	int fshlen = strlen(fs);
	glShaderSource(fsh, 1, &fs, &fshlen);
	glCompileShader(fsh);

	int isCompiled = 0;
	glGetShaderiv(fsh, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(fsh, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fsh, maxLength, &maxLength, &errorLog[0]);
		printf("%s", errorLog.data());
		glDeleteShader(fsh);
		return 0;
	}
	glAttachShader(shp, fsh);
	glLinkProgram(shp);
	glValidateProgram(shp);
	glUseProgram(shp);
	glDeleteProgram(vsh);
	glDeleteProgram(fsh);
	return shp;
}

int main(int argc, char** argv)
{
	srand(time(NULL)); // random seed

	GLFWwindow* window;

	if (!glfwInit())
	{
		std::cout << "[GLFW]: Initialization Error!\n";
		return -1;
	}
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW", NULL, NULL);
	if (!window)
	{
		std::cout << "[GLFW]: Window Creation Error!\n";
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		std::cout << "[GLEW]: Initialization Error!\n";
		return -1;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);

	Program shp = create_shader(
		R"(
		#version 330 core

		in vec3 pos; 
		in vec3 color; 
		in vec3 normal; 
		in vec2 tcoord; 

		out vec3 vcolor; 
		out vec3 vnormal; 
		out vec3 vpos; 
		out vec2 vtcoord; 

		uniform mat4 transform; 
		uniform mat4 rtransform; 
		uniform mat4 projection;

		void main() 
		{
			vcolor = color;
			vtcoord = tcoord;
			vnormal = (rtransform * vec4(normal, 1)).xyz;
			vpos = (transform * vec4(pos, 1)).xyz;
			gl_Position = projection * transform * vec4(pos, 1);
		}
	)",
		R"(
		#version 330 core

		in vec3 vcolor;
		in vec3 vnormal;
		in vec3 vpos;
		in vec2 vtcoord;

		uniform sampler2D tex1;
		uniform vec3 campos;
		uniform vec3 lightpos;

		out vec4 gl_FragColor;

		void main() 
		{
			vec3 camdir = normalize(campos - vpos);
			float lighting = max(dot(normalize(vnormal), normalize(lightpos - vpos)), 0.25);
			vec3 r = reflect(-normalize(lightpos - vpos), normalize(vnormal));
			float specularity = max(pow(dot(camdir, r), 10), 0);
			gl_FragColor = vec4(lighting * vcolor + 2 * specularity * vec3(1), 0);
		}
	)"
	);

	//glUseProgram(fboshader);
	//glUniform1i(glGetUniformLocation(fboshader, "screenTexture"), 0);

	Engine game(window, shp, 855);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(72.0f / 255.0f, 209.0f / 255.0f, 204.0f / 255.0f, 1.0f);

		game.game_loop();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	return 0;
}