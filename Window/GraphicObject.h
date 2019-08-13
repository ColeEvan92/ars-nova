#pragma once

#include <memory>

#include "WindowsHider.h"
#include "Pair.h"

namespace bleak
{
	namespace graphics
	{
		class Color
		{
		public:
			unsigned char red = 0, green = 0, blue = 0;
			Color() = default;
			Color(unsigned char r, unsigned char g, unsigned char b);
			Color(const Color&) = default;
			~Color() = default;
			Color& operator=(const Color&) = default;
			operator windows::COLORREF() const;
		};
		enum class SystemCursor : long
		{
			NORMAL = windows::OCR_NORMAL
			, IBEAM = windows::OCR_IBEAM
			, WAIT = windows::OCR_WAIT
			, CROSS = windows::OCR_CROSS
			, UP = windows::OCR_UP
			, SIZENWSE = windows::OCR_SIZENWSE
			, SIZENESW = windows::OCR_SIZENESW
			, SIZEWE = windows::OCR_SIZEWE
			, SIZENS = windows::OCR_SIZENS
			, SIZEALL = windows::OCR_SIZEALL
			, NO = windows::OCR_NO
			, HAND = windows::OCR_HAND
			, APPSTARTING = windows::OCR_APPSTARTING
		};
		enum class SystemIcon : long
		{
			SAMPLE = windows::OIC_SAMPLE
			, HAND = windows::OIC_HAND
			, QUES = windows::OIC_QUES
			, BANG = windows::OIC_BANG
			, NOTE = windows::OIC_NOTE
			, WINLOGO = windows::OIC_WINLOGO
			, WARNING = windows::OIC_WARNING
			, ERROR = windows::OIC_ERROR
			, INFORMATION = windows::OIC_INFORMATION
			, SHIELD = windows::OIC_SHIELD
		};

		class GraphicObject
		{
		public:
			typedef pair::_pair<long> Size;
			typedef pair::_pair<long> Point;
			GraphicObject() = default;
			GraphicObject(const GraphicObject&) = default;
			GraphicObject& operator=(const GraphicObject&) = default;
		};
		namespace detail
		{
			namespace GraphicObject
			{
				struct _referenced_object
				{
					windows::HGDIOBJ obj;
					explicit _referenced_object(windows::HGDIOBJ);
					~_referenced_object();
				};

				class _shallow_object : public GraphicObject
				{
				public:
					_shallow_object(const _shallow_object&) = default;
					_shallow_object(_shallow_object&&) = default;
					_shallow_object& operator=(const _shallow_object&) = default;
					_shallow_object& operator=(_shallow_object&&) = default;
				protected:
					_shallow_object(windows::HGDIOBJ);
					_shallow_object(const bchar*, windows::UINT, Size, windows::UINT);
					void _swap(_shallow_object&);
					windows::HGDIOBJ _get_object() const;
					std::shared_ptr<_referenced_object> _ptr;
				};
			}
		}
		namespace AlignMode
		{
			enum AlignMode : unsigned char
			{
				CENTER = 0,
				LEFT = 1,
				TOP = 2,
				RIGHT = 4,
				BOTTOM = 8,
				STRETCH = 15,
				FIT = 31
			};
		}
		typedef unsigned char AlignMode_t;
		class Bitmap : public GraphicObject
		{
		public:
			typedef pair::_rectangle<long> Bounds;
			Bitmap(const bstring& file);
			Bitmap(Size sz);
			Bitmap(const Bitmap&);
			Bitmap(Bitmap&&);
			~Bitmap();
			Bitmap& operator=(const Bitmap&);
			Bitmap& operator=(Bitmap&&);
			Size size() const;
			void swap(Bitmap& rhs);
			windows::HBITMAP get_object() const;
			static Bounds calculate_blt_bounds(Size dest, Size src, AlignMode_t);
		private:
			Bitmap();
			void _set_size();
			windows::HBITMAP _hBit;
			Size _size;
		};
		class Brush : public detail::GraphicObject::_shallow_object
		{
		public:
			using detail::GraphicObject::_shallow_object::_shallow_object;
			using detail::GraphicObject::_shallow_object::operator=;
			explicit Brush(Color);
			void swap(Brush&);
			windows::HBRUSH get_object() const;
		};
		class Pen : public detail::GraphicObject::_shallow_object
		{
		public:
			using detail::GraphicObject::_shallow_object::_shallow_object;
			using detail::GraphicObject::_shallow_object::operator=;
			Pen(Color, int width);
			void swap(Pen&);
			windows::HPEN get_object() const;
		};
		class Cursor : public detail::GraphicObject::_shallow_object
		{
		public:
			using detail::GraphicObject::_shallow_object::_shallow_object;
			using detail::GraphicObject::_shallow_object::operator=;
			Cursor(const bstring& filename, bool use_def_size);
			Cursor(const bstring& filename, Size);
			Cursor(SystemCursor, bool use_def_size);
			Cursor(SystemCursor, Size);
			void swap(Cursor&);
			windows::HCURSOR get_object() const;
		};
		class Icon : public detail::GraphicObject::_shallow_object
		{
		public:
			using detail::GraphicObject::_shallow_object::_shallow_object;
			using detail::GraphicObject::_shallow_object::operator=;
			Icon(bstring filename, bool use_def_size);
			Icon(bstring filename, Size);
			Icon(SystemIcon, bool use_def_size);
			Icon(SystemIcon, Size);
			void swap(Icon&);
			windows::HICON get_object() const;
		};

	}
}