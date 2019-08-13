#include "SnakeGame.h"

using namespace std::chrono_literals;

size_t operator+(Game::Direction e)
{
	return static_cast<std::underlying_type_t<Game::Direction>>(e);
}

Game::Game(Size playing_field, int unit_size,
	Position snake_start, Direction snake_facing, int snake_length, float snake_speed, int growth_per_food,
	Color snake_body_color, Color snake_head_color, Color food_color, Color field_color)
	: _field_size(playing_field), _unit_size(unit_size), _snake_speed(snake_speed), _growth_per_food(growth_per_food),
	_snake_brush(snake_body_color), _snake_pen(snake_body_color, 1), _head_brush(snake_head_color), _head_pen(snake_head_color, 1), _food_brush(food_color), _food_pen(food_color, 1), _field_brush(field_color),
	_next_dir(snake_facing)
{
	_snake_pieces.push_back({ snake_start, snake_facing });

	_snake_growth = snake_length - 1;
	_set_food();
}
void Game::draw(Draw& d)
{
	d.fill(Draw::Rectangle({ 0, 0 }, _field_size * _unit_size), _field_brush);

	long long ms_p_unit = static_cast<long long>(1000.0f / _snake_speed);

	float t = static_cast<float>(_time.count() % ms_p_unit) / static_cast<float>(ms_p_unit);

	auto calc_rect = [this](const _body& b, float progress) { auto p = (b.pos * _unit_size) + (_offsets[+b.dir] * _unit_size * progress); return Draw::Rectangle{ p, p + _unit_size }; };

	d.window.canvas.set_def_brush(_snake_brush);
	d.window.canvas.set_def_pen(_snake_pen);

	if (_snake_growth != 0)
		d.ellipse(Draw::Rectangle(_snake_pieces.front().pos, _snake_pieces.front().pos + 1) * _unit_size);

	for (auto i = _snake_pieces.cbegin(), e = --_snake_pieces.cend(); i != e; ++i)
		d.ellipse(calc_rect(*i, t));

	d.ellipse(calc_rect(_snake_pieces.back(), t), _head_brush, _head_pen);

	d.ellipse(Draw::Rectangle(_food_pos, _food_pos + 1) * _unit_size, _food_brush, _food_pen);
}
void Game::advance(Time t)
{
	if (_game_over)
		return;
	long long ms_p_unit = static_cast<long long>(1000.0f / _snake_speed);

	while (t.count() > 0)
	{
		auto step = std::min(ms_p_unit - (_time.count() % ms_p_unit), t.count());

		t -= std::chrono::milliseconds(step);
		_time += std::chrono::milliseconds(step);

		if (_time.count() % ms_p_unit == 0)
		{
			if (_snake_growth == 0)
				_snake_pieces.erase(_snake_pieces.begin());
			else
				--_snake_growth;

			_snake_pieces.push_back({ _snake_pieces.back().pos + _offsets[+_snake_pieces.back().dir], _next_dir });

			const auto& pos = _snake_pieces.back().pos;
			if (pos.x_value < 0 || pos.y_value < 0 || pos.x_value >= _field_size.x_value || pos.y_value >= _field_size.y_value)
			{
				_game_over = true;
				return;
			}
			for (auto i = _snake_pieces.cbegin(), e = --_snake_pieces.cend(); i != e; ++i)
			{
				if (i->pos == _snake_pieces.back().pos)
				{
					_game_over = true;
					return;
				}
			}
if (pos == _food_pos)
			{
				_snake_growth += _growth_per_food;
				_set_food();
				++_score;
			}
		}
	}
}
void Game::change_direction(Direction d)
{
	if (_offsets[+d] + _offsets[+_snake_pieces.back().dir] != Position{})
		_next_dir = d;
}
int Game::score() const
{
	return _score;
}
bool Game::game_over() const
{
	return _game_over;
}
Game::operator bool() const
{
	return !game_over();
}
void Game::_set_food()
{
	std::uniform_int_distribution<int> x_gen(0, _field_size.x_value - 1), y_gen(0, _field_size.y_value - 1);
	bool done;
	do
	{
		done = true;
		_food_pos = { x_gen(_mt), y_gen(_mt) };
		for (const auto& piece : _snake_pieces)
		{
			if (piece.pos == _food_pos)
			{
				done = false;
				break;
			}
		}
	} while (!done);
}
