#include <utility>
#include <new>

#include "GraphicObject.h"

namespace bleak
{
	namespace graphics
	{
		Color::Color(unsigned char r, unsigned char g, unsigned char b) : red(r), green(g), blue(b) {}
		Color::operator windows::COLORREF() const
		{
			return (red) | (green << 8) | (blue << 16);
		}
		windows::UINT operator+(SystemCursor e)
		{
			return static_cast<windows::UINT>(e);
		}
		windows::UINT operator+(SystemIcon e)
		{
			return static_cast<windows::UINT>(e);
		}

		detail::GraphicObject::_referenced_object::_referenced_object(windows::HGDIOBJ obj) : obj(obj) {}
		detail::GraphicObject::_referenced_object::~_referenced_object()
		{
			windows::DeleteObject(obj);
		}

		detail::GraphicObject::_shallow_object::_shallow_object(windows::HGDIOBJ obj) : _ptr(std::make_shared<detail::GraphicObject::_referenced_object>(obj)) {}
		detail::GraphicObject::_shallow_object::_shallow_object(const bchar* name, windows::UINT type, Size size, windows::UINT flags) : _shallow_object(windows::LoadImage(0, name, type, size.x_value, size.y_value, flags)) {}
		void detail::GraphicObject::_shallow_object::_swap(detail::GraphicObject::_shallow_object& rhs)
		{
			std::swap(_ptr, rhs._ptr);
		}
		windows::HGDIOBJ detail::GraphicObject::_shallow_object::_get_object() const
		{
			return _ptr->obj;
		}

		Bitmap::Bitmap(const bstring& file) : Bitmap()
		{
			_hBit = reinterpret_cast<windows::HBITMAP>(windows::LoadImage(nullptr, file.c_str(), windows::IMAGE_BITMAP, 0, 0, windows::LR_LOADFROMFILE));
			_set_size();
		}
		Bitmap::Bitmap(Size sz) : Bitmap()
		{
			auto t = windows::GetDC(nullptr);
			_hBit = windows::CreateCompatibleBitmap(t, sz.x_value, sz.y_value);
			_size = sz;
			windows::ReleaseDC(nullptr, t);
		}
		Bitmap::Bitmap(const Bitmap& rhs) : Bitmap()
		{
			_hBit = reinterpret_cast<windows::HBITMAP>(windows::CopyImage(rhs._hBit, windows::IMAGE_BITMAP, rhs._size.x_value, rhs._size.y_value, 0));
			_size = rhs._size;
		}
		Bitmap::Bitmap(Bitmap&& rhs) : Bitmap()
		{
			std::swap(_hBit, rhs._hBit);
			std::swap(_size, rhs._size);
		}
		Bitmap::~Bitmap()
		{
			if (_hBit)
				windows::DeleteObject(_hBit);
		}
		Bitmap& Bitmap::operator=(const Bitmap& rhs)
		{
			this->~Bitmap();
			return *(new (this) Bitmap(rhs));
		}
		Bitmap& Bitmap::operator=(Bitmap&& rhs)
		{
			std::swap(_hBit, rhs._hBit);
			std::swap(_size, rhs._size);
			return *this;
		}
		Bitmap::Size Bitmap::size() const
		{
			return _size;
		}
		void Bitmap::swap(Bitmap& rhs)
		{
			std::swap(_hBit, rhs._hBit);
			std::swap(_size, rhs._size);
		}
		windows::HBITMAP Bitmap::get_object() const
		{
			return _hBit;
		}
		Bitmap::Bounds Bitmap::calculate_blt_bounds(Size dest, Size src, AlignMode_t mode)
		{
			Bounds rect;
			if (mode == AlignMode::STRETCH)
			{
				rect.x_value_second = dest.x_value;
				rect.y_value_second = dest.y_value;
			}
			else if (mode == AlignMode::FIT)
			{
				long double x = (long double)src.x_value / (long double)dest.x_value;
				long double y = (long double)src.y_value / (long double)dest.y_value;
				long double scale = (x > y ? x : y);
				scale = 1.0l / scale;
				x = src.x_value * scale;
				y = src.y_value * scale;
				if (dest.x_value - x > dest.y_value - y)
				{
					rect.x_value_first = static_cast<Bounds::type>((dest.x_value - x) / 2.0l);
					rect.x_value_second = static_cast<Bounds::type>(rect.x_value_first + x);
					rect.y_value_second = dest.y_value;
				}
				else
				{
					rect.y_value_first = static_cast<Bounds::type>((dest.y_value - y) / 2.0l);
					rect.x_value_second = dest.x_value;
					rect.y_value_second = static_cast<Bounds::type>(rect.y_value_first + y);
				}
			}
			else
			{
				rect.x_value_first = (dest.x_value - src.x_value) / 2;
				rect.y_value_first = (dest.y_value - src.y_value) / 2;
				rect.x_value_second = rect.x_value_first + src.x_value;
				rect.y_value_second = rect.y_value_first + src.y_value;
				if (mode & AlignMode::LEFT)
				{
					rect.x_value_first = 0;
					rect.x_value_second = src.x_value;
				}
				else if (mode & AlignMode::RIGHT)
				{
					rect.x_value_first = dest.x_value - src.x_value;
					rect.x_value_second = dest.x_value;
				}
				if (mode & AlignMode::TOP)
				{
					rect.y_value_first = 0;
					rect.y_value_second = src.y_value;
				}
				else if (mode & AlignMode::BOTTOM)
				{
					rect.y_value_first = dest.y_value - src.y_value;
					rect.y_value_second = dest.y_value;
				}
			}
			return rect;
		}
		Bitmap::Bitmap() : _hBit(nullptr), _size{ 0, 0 } {}
		void Bitmap::_set_size()
		{
			windows::BITMAP bm;
			windows::GetObject(_hBit, sizeof(bm), &bm);
			_size = { bm.bmWidth, bm.bmHeight };
		}

		Brush::Brush(Color c) : detail::GraphicObject::_shallow_object(windows::CreateSolidBrush(c)) {}
		void Brush::swap(Brush& rhs)
		{
			_swap(rhs);
		}
		windows::HBRUSH Brush::get_object() const
		{
			return reinterpret_cast<windows::HBRUSH>(_ptr->obj);
		}

		Pen::Pen(Color c, int width) : detail::GraphicObject::_shallow_object(windows::CreatePen(windows::PS_SOLID, width, c)) {}
		void Pen::swap(Pen& rhs)
		{
			_swap(rhs);
		}
		windows::HPEN Pen::get_object() const
		{
			return reinterpret_cast<windows::HPEN>(_ptr->obj);
		}

		Cursor::Cursor(const bstring& filename, bool use_def_size) : detail::GraphicObject::_shallow_object(filename.c_str(), windows::IMAGE_CURSOR, { 0, 0 }, windows::LR_LOADFROMFILE | (use_def_size ? windows::LR_DEFAULTSIZE : 0)) {}
		Cursor::Cursor(const bstring& filename, Size size) : detail::GraphicObject::_shallow_object(filename.c_str(), windows::IMAGE_CURSOR, size, windows::LR_LOADFROMFILE) {}
		Cursor::Cursor(SystemCursor e, bool use_def_size) : detail::GraphicObject::_shallow_object(windows::MAKEINTRESOURCE(+e), windows::IMAGE_CURSOR, { 0, 0 }, windows::LR_SHARED | (use_def_size ? windows::LR_DEFAULTSIZE : 0)) {}
		Cursor::Cursor(SystemCursor e, Size size) : detail::GraphicObject::_shallow_object(windows::MAKEINTRESOURCE(+e), windows::IMAGE_CURSOR, size, windows::LR_SHARED) {}
		void Cursor::swap(Cursor& rhs)
		{
			_swap(rhs);
		}
		windows::HCURSOR Cursor::get_object() const
		{
			return reinterpret_cast<windows::HCURSOR>(_ptr->obj);
		}

		Icon::Icon(bstring filename, bool use_def_size) : detail::GraphicObject::_shallow_object(filename.c_str(), windows::IMAGE_ICON, { 0, 0 }, windows::LR_LOADFROMFILE | (use_def_size ? windows::LR_DEFAULTSIZE : 0)) {}
		Icon::Icon(bstring filename, Size size) : detail::GraphicObject::_shallow_object(filename.c_str(), windows::IMAGE_ICON, size, windows::LR_LOADFROMFILE) {}
		Icon::Icon(SystemIcon e, bool use_def_size) : detail::GraphicObject::_shallow_object(windows::MAKEINTRESOURCE(+e), windows::IMAGE_ICON, { 0, 0 }, windows::LR_SHARED | (use_def_size ? windows::LR_DEFAULTSIZE : 0)) {}
		Icon::Icon(SystemIcon e, Size size) : detail::GraphicObject::_shallow_object(windows::MAKEINTRESOURCE(+e), windows::IMAGE_ICON, size, windows::LR_SHARED) {}
		void Icon::swap(Icon& rhs)
		{
			_swap(rhs);
		}
		windows::HICON Icon::get_object() const
		{
			return reinterpret_cast<windows::HICON>(_ptr->obj);
		}
	}
}