#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <Windows.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tinyobjloader.h"
#include "stb_image.h"
#include "constants.h"
#include "Shader.h"

typedef    unsigned int    Buffer;
typedef    unsigned int    Program;
typedef    unsigned int    Location;
typedef    glm::mat4       Matrix4;
typedef    glm::vec3       XYZPoint;

class Grid;

class Object
{
private:
	bool destroyed = false;

	friend class Grid;
	struct Data {
		/* Object fields for drawing */
		std::vector <float> vertices;
		std::vector <float> colors;
		std::vector <float> normals;
		std::vector <float> tex_coords;
		std::vector <GLuint> indices;
	};

	static std::map <Ball, Data> data; // 7 balls - 7 Data structs

	static bool data_exists(Ball ball);

	Ball ball;

	/* Transforms */
	Matrix4 transform = glm::mat4(1), norm_transform = glm::mat4(1), projection = glm::mat4(1);
	Location transf_loc, norm_transf_loc, proj_loc;

	/* Buffers */
	Buffer vbo, cbo, nbo, ibo, tbo, tex, fbo, rbo, fboTex;
	Location pos_loc, color_loc, norm_loc, tc_loc, tex_loc, depth_loc;

	Program shader;

	void extract_data(std::string_view path_obj);
	void apply_texture(std::string_view path_texture);

	void init_fbo();
	void init_vbo();
	void init_cbo();
	void init_nbo();
	void init_tbo();
	void init_ibo();

	void init_transform();
public:
	Object() = delete;
	Object(Ball ball, std::string_view path_obj, std::string_view path_texture, Program& shader, XYZPoint position = XYZPoint(0.0, 0.0, 0.0));
	Object(std::vector <float> vertices, std::vector <float> colors, std::vector <float> normals, std::vector <unsigned int> indices, Program& shader);
	//Object(const Object& other);

	void draw();

	void set_projection(glm::vec3 cameraPos, glm::vec3 lookAt, float fov, float aspect, float nearClip, float farClip);

	void move_to(XYZPoint point);

	void reinit(Ball ball, XYZPoint position);

	void move_to(Matrix4 transform, Matrix4 norm_transform);

	void rotate(float angle);

	~Object();
};

