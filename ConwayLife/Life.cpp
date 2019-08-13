#include "Life.h"

Life::Life(Size dimensions) : _dims(dimensions)
{
	_cells.resize((_dims.x_value + 2) * (_dims.y_value + 2), 0);
}
bool Life::get(Point pos) const
{
	return _cells[_xy_to_index(pos + 1)] == 1;
}
void Life::set(Point pos, bool alive)
{
	_cells[_xy_to_index(pos + 1)] = alive;
}
void Life::advance(unsigned int n)
{
	if (n == 0)
		return;
	auto next = _cells;
	while (n)
	{
		for (size_t y = 1; y <= _dims.y_value; ++y)
		{
			for (size_t x = 1; x <= _dims.x_value; ++x)
			{
				int neighbors = 
					(_cells[_xy_to_index({ x - 1, y - 1 })] == 1) + 
					(_cells[_xy_to_index({ x, y - 1 })] == 1) +
					(_cells[_xy_to_index({ x + 1, y - 1 })] == 1) +
					(_cells[_xy_to_index({ x - 1, y })] == 1) + 
					(_cells[_xy_to_index({ x + 1, y })] == 1) +
					(_cells[_xy_to_index({ x - 1, y + 1 })] == 1) + 
					(_cells[_xy_to_index({ x, y + 1 })] == 1) +
					(_cells[_xy_to_index({ x + 1, y + 1 })] == 1);

				if (neighbors == 3)
					next[_xy_to_index({ x, y })] = true;
				else if (neighbors == 2)
					next[_xy_to_index({ x, y })] = _cells[_xy_to_index({ x, y })];
				else
					next[_xy_to_index({ x, y })] = false;
			}
		}
		_cells.swap(next);
		--n;
	}
}
void Life::clear()
{
	std::fill(_cells.begin(), _cells.end(), 0);
}
Life::Size Life::size() const
{
	return _dims;
}
size_t Life::_xy_to_index(Point pos) const
{
	return pos.y_value * (_dims.y_value + 2) + pos.x_value;
}
