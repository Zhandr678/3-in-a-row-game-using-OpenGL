#include "Grid.h"

XYPoint Grid::position_from_indices(GridPosition i, GridPosition j)
{
	return XYPoint(-75.0f + 15.0f * j, 75.0f - 15.0f * i);
}

void Grid::random_generator()
{
	for (size_t i = 0; i < size_x; i++)
	{
		Row row;
		for (size_t j = 0; j < size_y; j++)
		{
			int rand_ball = rand() % 7;
			Object *obj = new Object(static_cast <Ball>(rand_ball), obj_path, NOFILE, shader);
			XYPoint point = position_from_indices(i, j);
			obj->move_to(XYZPoint(point.x, point.y, 0));
			row.push_back(obj);
		}
		grid.push_back(row);
	}
}

Grid::Grid(Program& shader) :
	shader(shader)
{
	set_projection(XYZPoint(0, 0, 170), XYZPoint(0), 45.0f, 1.0f, 0.01f, 200.0f);
	random_generator();
	update();
}

void Grid::set_projection(glm::vec3 cameraPos, glm::vec3 lookAt, float fov, float aspect, float nearClip, float farClip)
{
	this->projection = Matrix4(1);
	this->projection = glm::perspective(fov, aspect, nearClip, farClip);
	this->projection *= (glm::lookAt(cameraPos, lookAt, glm::vec3(0, 1, 0)));

	this->proj_loc = glGetUniformLocation(shader, "projection");
	glUniformMatrix4fv(this->proj_loc, 1, GL_FALSE, glm::value_ptr(this->projection));
}

void Grid::draw()
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			grid[i][j]->draw();
		}
	}
}

void Grid::rotate(float angle)
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			grid[i][j]->rotate(angle);
		}
	}
}

Grid::~Grid()
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			delete grid[i][j];
		}
	}
}

Status Grid::position_exists(GridPosition x, GridPosition y)
{
	return x < 0 || y < 0 || x >= grid.size() || y >= grid[0].size() ? false : true;
}

void Grid::swap(GridPosition x1, GridPosition y1, GridPosition x2, GridPosition y2)
{
	grid[x1][y1]->move_to(XYZPoint(position_from_indices(x2, y2).x, position_from_indices(x2, y2).y, 0));
	grid[x2][y2]->move_to(XYZPoint(position_from_indices(x1, y1).x, position_from_indices(x1, y1).y, 0));
}

void Grid::gravity(GridPosition x, GridPosition y) 
{
	if (!position_exists(x, y)) { return; }

	std::pair<GridPosition, GridPosition> next = get_next(x, y, Direction::UP);

	if (!position_exists(next.first, next.second)) 
	{
		int rand_ball = rand() % 7;
		XYPoint point = position_from_indices(x, y);
		grid[x][y]->reinit(static_cast<Ball>(rand_ball), XYZPoint(point.x, point.y, 0.0f));
		return;
	}

	if (next.first == x && next.second == y) 
	{
		int rand_ball = rand() % 7;
		XYPoint point = position_from_indices(x, y);
		grid[x][y]->reinit(static_cast<Ball>(rand_ball), XYZPoint(point.x, point.y, 0.0f));
		return;
	}

	this->swap(x, y, next.first, next.second);
	std::swap(grid[x][y], grid[next.first][next.second]);
	return gravity(next.first, next.second);
}


void Grid::update()
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j]->destroyed)
			{
				gravity(i, j);
			}
		}
	}
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (dfs_runner(i, j))
			{
				update();
			}
		}
	}
}

Status Grid::make_move(GridPosition x, GridPosition y, Direction direction)
{
	switch (direction)
	{
	case Direction::DOWN:
		if (position_exists(x, y) and position_exists(x + 1, y)) 
		{ 
			this->swap(x, y, x + 1, y);
			std::swap(grid[x][y], grid[x + 1][y]); 
			if (dfs_runner(x, y) or dfs_runner(x + 1, y)) { update(); }
		}
		return true;
	case Direction::UP:
		if (position_exists(x, y) and position_exists(x - 1, y))
		{
			this->swap(x, y, x - 1, y);
			std::swap(grid[x][y], grid[x - 1][y]);
			if (dfs_runner(x, y) or dfs_runner(x - 1, y)) { update(); }
		}
		return true;
	case Direction::RIGHT:
		if (position_exists(x, y) and position_exists(x, y + 1))
		{
			this->swap(x, y, x, y + 1);
			std::swap(grid[x][y], grid[x][y + 1]);
			if (dfs_runner(x, y) or dfs_runner(x, y + 1)) { update(); }
		}
		return true;
	case Direction::LEFT:
		if (position_exists(x, y) and position_exists(x, y - 1))
		{
			this->swap(x, y, x, y - 1);
			std::swap(grid[x][y], grid[x][y - 1]);
			if (dfs_runner(x, y) or dfs_runner(x, y - 1)) { update(); }
		}
		return true;
	default:
		return false;
	}
	return false;
}

std::pair <GridPosition, GridPosition> Grid::get_next(GridPosition x, GridPosition y, Direction dir)
{
	switch (dir)
	{
	case Direction::UP:
		if (position_exists(x - 1, y))
			return { x - 1, y };
		return { x, y };
	case Direction::DOWN:
		if (position_exists(x + 1, y))
			return { x + 1, y };
		return { x, y };
	case Direction::RIGHT:
		if (position_exists(x, y + 1))
			return { x, y + 1 };
		return { x, y };
	case Direction::LEFT:
		if (position_exists(x, y - 1))
			return { x, y - 1 };
		return { x, y };
	default:
		return { x, y };
	}
	return { x, y };
}

void Grid::break_at(GridPosition x, GridPosition y)
{
	grid[x][y]->destroyed = true;
}

int Grid::dfs_horizontal(GridPosition x, GridPosition y, Direction dir, Ball genesis_color, bool is_genesis = true)
{
	if (!position_exists(x, y) || grid[x][y]->ball != genesis_color) { return 0; }
	
	labeled.push_back({ x, y });
	visited[{x, y}] = true;
	
	int count = 1;
	std::pair <GridPosition, GridPosition> next;

	if (is_genesis)
	{
		next = get_next(x, y, Direction::RIGHT);
		if (!visited[next]) { count += dfs_horizontal(next.first, next.second, Direction::RIGHT, genesis_color, false); }

		next = get_next(x, y, Direction::LEFT);
		if (!visited[next]) { count += dfs_horizontal(next.first, next.second, Direction::LEFT, genesis_color, false); }
	}
	else 
	{
		next = get_next(x, y, dir);
		if (!visited[next]) { count += dfs_horizontal(next.first, next.second, dir, genesis_color, false); }
	}
	return count;
}

int Grid::dfs_vertical(GridPosition x, GridPosition y, Direction dir, Ball genesis_color, bool is_genesis = true)
{
	if (!position_exists(x, y) || grid[x][y]->ball != genesis_color) { return 0; }

	labeled.push_back({ x, y });
	visited[{ x, y }] = true;

	int count = 1;
	std::pair <GridPosition, GridPosition> next;

	if (is_genesis)
	{
		next = get_next(x, y, Direction::UP);
		if (!visited[next]) { count += dfs_horizontal(next.first, next.second, Direction::UP, genesis_color, false); }

		next = get_next(x, y, Direction::DOWN);
		if (!visited[next]) { count += dfs_horizontal(next.first, next.second, Direction::DOWN, genesis_color, false); }
	}
	else
	{
		next = get_next(x, y, dir);
		if (!visited[next]) { count += dfs_horizontal(next.first, next.second, dir, genesis_color, false); }
	}
	return count;
}

Status Grid::dfs_runner(GridPosition x, GridPosition y)
{
	int horizontal_match = dfs_horizontal(x, y, Direction::NONE, grid[x][y]->ball);
	visited.clear();

	if (horizontal_match >= POPPING_THRESHOLD)
	{
		for (const std::pair <GridPosition, GridPosition>& p : labeled)
		{
			break_at(p.first, p.second);
		}
	}
	labeled.clear();

	int vertical_match = dfs_vertical(x, y, Direction::NONE, grid[x][y]->ball);
	visited.clear();

	if (vertical_match >= POPPING_THRESHOLD)
	{
		for (const std::pair <GridPosition, GridPosition>& p : labeled)
		{
			break_at(p.first, p.second);
		}
	}
	labeled.clear();

	return (horizontal_match >= POPPING_THRESHOLD || vertical_match >= POPPING_THRESHOLD) ? true : false;
}