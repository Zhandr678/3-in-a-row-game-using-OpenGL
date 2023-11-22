#pragma once
#include <string>
#include <filesystem>
#include <vector>

#define NOFILE "NONE"

constexpr char obj_path[48] = "D:/C++ projects/FINAL COMP GRAPH/Balls/ball.obj";
constexpr size_t WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
constexpr float ball_diameter = 10.0f;

constexpr double GRID_START_AT = 50.0;
constexpr double BALL_BOX_WIDTH = 83.0;
constexpr int POPPING_THRESHOLD = 3;

enum class Ball
{
	Red = 0, Blue = 1, Green = 2, Pink = 3, Yellow = 4, Orange = 5, Black = 6, Nothing = 100
};

enum class Direction { NONE = 0, UP = 1, DOWN = 2, RIGHT = 3, LEFT = 4 };