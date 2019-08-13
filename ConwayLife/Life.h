#pragma once

#include <vector>
#include <memory>

#include "Pair.h"

class Life
{
public:
	typedef bleak::pair::_pair<size_t> Point, Size;
	Life(Size dimensions);
	template <typename I, size_t H, size_t W> Life(I(&arr)[H][W]) : _dims(W, H)
	{
		_cells.reserve((W + 2) * (H + 2));
		_cells.insert(_cells.end(), W + 2, 0);
		for (auto& a : arr)
		{
			_cells.emplace_back(0);
			_cells.insert(_cells.end(), +a, a + W + 2);
			_cells.emplace_back(0);
		}
		_cells.insert(_cells.end(), W + 2, 0);
	}
	bool get(Point pos) const;
	void set(Point pos, bool alive);
	void advance(unsigned int n = 1);
	void clear();
	Size size() const;
private:
	size_t _xy_to_index(Point pos) const;
	std::vector<unsigned char> _cells;
	const Size _dims;
};