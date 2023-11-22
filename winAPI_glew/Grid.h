#pragma once
#include <vector>
#include <set>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

typedef                       unsigned int    GridPosition;
typedef              std::vector <Object*>    Row;
typedef                  std::vector <Row>    PlayGround;
typedef                               bool    Status;
typedef                          glm::vec3    XYZPoint;
typedef                          glm::vec2    XYPoint;

class Grid
{
private:
	size_t size_x = 11, size_y = 11;

	Matrix4 projection;
	Location proj_loc;

	PlayGround grid;

	Program shader;

	/* Movement */
	std::map < std::pair <GridPosition, GridPosition>, bool> visited;
	std::vector <std::pair <GridPosition, GridPosition>> labeled;
	std::set <std::pair <GridPosition, GridPosition>> broken;
	Status dfs_runner(GridPosition x, GridPosition y);

	int dfs_horizontal(GridPosition x, GridPosition y, Direction dir, Ball genesis_color, bool is_genesis);
	int dfs_vertical(GridPosition x, GridPosition y, Direction dir, Ball genesis_color, bool is_genesis);

	std::pair <GridPosition, GridPosition> get_next(GridPosition x, GridPosition y, Direction dir);

	XYPoint position_from_indices(GridPosition i, GridPosition j);

	void update();
	void break_at(GridPosition x, GridPosition y);
	void swap(GridPosition x1, GridPosition y1, GridPosition x2, GridPosition y2);
	void gravity(GridPosition x, GridPosition y);

	void random_generator();
public:
	Grid() = delete;
	Grid(Program& shader);
	Status make_move(GridPosition x, GridPosition y, Direction direction);
	Status position_exists(GridPosition x, GridPosition y);
	void set_projection(glm::vec3 cameraPos, glm::vec3 lookAt, float fov, float aspect, float nearClip, float farClip);
	//XYPoint point_at_position(GridPosition x, GridPosition y);
	void draw();
	void rotate(float angle);

	~Grid();
};

