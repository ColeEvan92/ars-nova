#pragma once

#include "WindowsHider.h"
#include "Window.h"

namespace bleak
{
	namespace graphics
	{
		class Draw
		{
		public:
			typedef window::Window::Canvas Canvas;
			typedef window::Window::Size Size;
			typedef window::Window::Point Point;
			typedef window::Window::Rectangle Rectangle;
			typedef Canvas::Scale Scale;
			window::Window& window;
			Draw(window::Window&);
			Draw(const Draw&) = delete;
			Draw(Draw&&);
			~Draw();
			Draw& operator=(const Draw&) = delete;
			void resize(Size);
			void resize(Size, AlignMode_t);
			void set_scale(Scale);
			void set_buffer(unsigned int);
			void set_alignment_mode(AlignMode_t);
			void fill(const Rectangle& bounds, const Brush& back);
			void rectangle(const Rectangle& bounds);
			void rectangle(const Rectangle& bounds, const Brush& back, const Pen& outline);
			void ellipse(const Rectangle& bounds);
			void ellipse(const Rectangle& bounds, const Brush& back, const Pen& outline);
			void bitmap(const Bitmap&, Point pos);
			void bitmap(const Bitmap&, Point dest_pos, Size dest_size, Point src_origin, Size src_size);
			void pixel(Point pos, Color);
			void text(Point pos, const bstring&);
			void text(const Rectangle& bounds, const bstring&);
			void commit();
		private:
			windows::HDC _dc;
			std::optional<Bitmap> _bitmap;
			std::optional<windows::COLORREF> _textc, _textb;
			std::optional<int> _textm;
			std::optional<windows::HGDIOBJ> _brush, _pen, _bm;
		};
	}
}