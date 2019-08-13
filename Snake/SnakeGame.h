#pragma once

#include <list>
#include <chrono>
#include <random>

#include "Pair.h"
#include "Draw.h"
using namespace bleak::graphics;

class Game
{
public:
	typedef bleak::pair::_pair<int> Size, Position;
	typedef std::chrono::milliseconds Time;
	enum class Direction { LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3 };

	//This constructor is a big oof
	Game(Size playing_field, int unit_size,
		Position snake_start, Direction snake_facing, int snake_length, float snake_speed, int growth_per_food,
		Color snake_body_color, Color snake_head_color, Color food_color, Color field_color);

	void draw(Draw&);

	void advance(Time);

	void change_direction(Direction);
	int score() const;
	bool game_over() const;
	operator bool() const;
private:
	static constexpr Position _offsets[]{ {-1, 0}, {0, -1}, {1, 0}, {0, 1} };

	void _set_food();

	struct _body
	{
		Position pos;
		Direction dir;
	};

	Size _field_size;
	int _unit_size;
	std::list<_body> _snake_pieces;
	int _snake_growth;
	Direction _next_dir;
	float _snake_speed;
	Position _food_pos;
	int _growth_per_food;
	int _score = 0;
	Brush _snake_brush, _head_brush, _food_brush, _field_brush;
	Pen _snake_pen, _head_pen, _food_pen;
	Time _time{ 0 };
	bool _game_over = false;
	std::mt19937_64 _mt{ std::random_device()() };
};
