#include "Draw.h"

namespace bleak
{
	namespace graphics
	{
		Draw::Draw(window::Window& w) : window(w)
		{
			auto tdc = windows::GetDC(w.native_handle());
			_dc = windows::CreateCompatibleDC(tdc);
			windows::ReleaseDC(w.native_handle(), tdc);
			if (window.canvas._brush)
				_brush = windows::SelectObject(_dc, window.canvas._brush->get_object());
			if (window.canvas._pen)
				_pen = windows::SelectObject(_dc, window.canvas._pen->get_object());
			if (window.canvas._textc)
				_textc = windows::SetTextColor(_dc, *window.canvas._textc);
			if (window.canvas._textb)
				_textb = windows::SetBkColor(_dc, *window.canvas._textb);
			if (window.canvas._textm)
				_textm = windows::SetBkMode(_dc, !*window.canvas._textm + 1);
			_bitmap = window.canvas._bitmap;
			if (_bitmap)
				_bm = windows::SelectObject(_dc, _bitmap->get_object());
		}
		Draw::Draw(Draw&& rhs) : window(rhs.window), _bitmap(std::move(rhs._bitmap)), _dc(rhs._dc), _textc(rhs._textc), _textb(rhs._textb), _textm(rhs._textm), _brush(rhs._brush), _pen(rhs._pen), _bm(rhs._bm)
		{
			rhs._dc = nullptr;
		}
		Draw::~Draw()
		{
			if (!_dc)
				return;
			if (_bitmap)
			{
				std::lock_guard<std::mutex> lock(window.canvas._guard);
				std::swap(_bitmap, window.canvas._bitmap);
			}
			if (_textc)
				windows::SetTextColor(_dc, *_textc);
			if (_textb)
				windows::SetBkColor(_dc, *_textb);
			if (_textm)
				windows::SetBkMode(_dc, *_textm);
			if (_brush)
				windows::SelectObject(_dc, *_brush);
			if (_pen)
				windows::SelectObject(_dc, *_pen);
			if (_bm)
				windows::SelectObject(_dc, *_bm);
			windows::DeleteDC(_dc);
			windows::InvalidateRect(window.native_handle(), nullptr, false);
		}
		void Draw::resize(Size sz)
		{
			window.canvas._bitmap = Bitmap(sz);
		}
		void Draw::resize(Size sz, AlignMode_t mode)
		{
			if (!window.canvas._bitmap)
			{
				resize(sz);
				return;
			}
			Bitmap bm(sz);
			auto tdc = windows::CreateCompatibleDC(_dc);
			auto old = windows::SelectObject(tdc, bm.get_object());
			auto bounds = Bitmap::calculate_blt_bounds(sz, window.canvas._bitmap->size(), mode);
			windows::StretchBlt(tdc, bounds.x_value_first, bounds.y_value_first, bounds.x_value_second - bounds.x_value_first, bounds.y_value_second - bounds.y_value_first, _dc, 0, 0, window.canvas._bitmap->size().x_value, window.canvas._bitmap->size().y_value, windows::SRCCOPY);
			windows::SelectObject(tdc, old);
			windows::DeleteDC(_dc);
			window.canvas._bitmap = std::move(bm);
		}
		void Draw::set_scale(Scale s)
		{
			window.canvas._scale = s;
		}
		void Draw::set_buffer(unsigned int b)
		{
			window.canvas._buffer = b;
		}
		void Draw::set_alignment_mode(AlignMode_t m)
		{
			window._align_mode = m;
		}
		void Draw::fill(const Rectangle& bounds, const Brush& back)
		{
			windows::RECT r = bounds;
			windows::FillRect(_dc, &r, back.get_object());
		}
		void Draw::rectangle(const Rectangle& bounds)
		{
			windows::Rectangle(_dc, bounds.x_value_first, bounds.y_value_first, bounds.x_value_second, bounds.y_value_second);
		}
		void Draw::rectangle(const Rectangle& bounds, const Brush& back, const Pen& outline)
		{
			auto b = windows::SelectObject(_dc, back.get_object()), p = windows::SelectObject(_dc, outline.get_object());
			rectangle(bounds);
			windows::SelectObject(_dc, b);
			windows::SelectObject(_dc, p);
		}
		void Draw::ellipse(const Rectangle& bounds)
		{
			windows::Ellipse(_dc, bounds.x_value_first, bounds.y_value_first, bounds.x_value_second, bounds.y_value_second);
		}
		void Draw::ellipse(const Rectangle& bounds, const Brush& back, const Pen& outline)
		{
			auto b = windows::SelectObject(_dc, back.get_object()), p = windows::SelectObject(_dc, outline.get_object());
			ellipse(bounds);
			windows::SelectObject(_dc, b);
			windows::SelectObject(_dc, p);
		}
		void Draw::bitmap(const Bitmap& bm, Point pos)
		{
			auto tdc = windows::CreateCompatibleDC(_dc);
			auto b = windows::SelectObject(tdc, bm.get_object());
			windows::BitBlt(_dc, pos.x_value, pos.y_value, bm.size().x_value, bm.size().y_value, tdc, 0, 0, windows::SRCCOPY);
			windows::SelectObject(tdc, b);
			windows::DeleteDC(tdc);
		}
		void Draw::bitmap(const Bitmap& bm, Point dest_pos, Size dest_size, Point src_origin, Size src_size)
		{
			auto tdc = windows::CreateCompatibleDC(_dc);
			auto b = windows::SelectObject(tdc, bm.get_object());
			windows::StretchBlt(_dc, dest_pos.x_value, dest_pos.y_value, dest_size.x_value, dest_size.y_value, tdc, src_origin.x_value, src_origin.y_value, src_size.x_value, src_size.y_value, windows::SRCCOPY);
			windows::SelectObject(tdc, b);
			windows::DeleteDC(tdc);
		}
		void Draw::pixel(Point pos, Color c)
		{
			windows::SetPixel(_dc, pos.x_value, pos.y_value, c);
		}
		void Draw::text(Point pos, const bstring& str)
		{
			size_t start = 0;
			do
			{
				size_t end = std::min(str.size(), str.find_first_of("\n\r"));
				bstring line = str.substr(start, end - start);
				windows::SIZE sz{};
				windows::GetTextExtentPoint32(_dc, line.c_str(), line.size(), &sz);
				windows::RECT bounds{ pos.x_value, pos.y_value, pos.x_value + sz.cx, pos.y_value + sz.cy };
				windows::DrawText(_dc, line.c_str(), line.size(), &bounds, windows::DT_NOCLIP);
				start = str.find_first_not_of("\n\r", end);
				pos.y_value += sz.cy;
			} while (start < str.size());
		}
		void Draw::text(const Rectangle& bounds, const bstring& str)
		{
			windows::RECT r = bounds;
			windows::DrawText(_dc, str.c_str(), str.size(), &r, 0);
		}
		void Draw::commit()
		{
			std::lock_guard lock(window.canvas._guard);
			window.canvas._bitmap = _bitmap;
		}
	}
}