#pragma once

#include <tuple>
#include <type_traits>

#include "WindowsHider.h"

namespace bleak
{
	namespace pair
	{
		template <typename T> struct _pair;
		template <typename T> struct _rectangle
		{
			static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "Numbers only!");
			typedef T type;
			T x_value_first{}, y_value_first{}, x_value_second{}, y_value_second{};
			constexpr _rectangle() = default;
			constexpr _rectangle(T x1, T y1, T x2, T y2) : x_value_first(x1), y_value_first(y1), x_value_second(x2), y_value_second(y2) {}
			constexpr _rectangle(const _pair<T>& p1, const _pair<T>& p2);
			constexpr _rectangle(const windows::RECT& r) : _rectangle{ r.left, r.top, r.right, r.bottom } {}
			constexpr _rectangle& operator+=(_rectangle rhs)
			{
				x_value_first += rhs.x_value_first;
				y_value_first += rhs.y_value_first;
				x_value_second += rhs.x_value_second;
				y_value_second += rhs.y_value_second;
				return *this;
			}
			constexpr _rectangle& operator+=(T rhs)
			{
				x_value_first += rhs;
				y_value_first += rhs;
				x_value_second += rhs;
				y_value_second += rhs;
				return *this;
			}
			constexpr _rectangle& operator-=(_rectangle rhs)
			{
				x_value_first -= rhs.x_value_first;
				y_value_first -= rhs.y_value_first;
				x_value_second -= rhs.x_value_second;
				y_value_second -= rhs.y_value_second;
				return *this;
			}
			constexpr _rectangle& operator-=(T rhs)
			{
				x_value_first -= rhs;
				y_value_first -= rhs;
				x_value_second -= rhs;
				y_value_second -= rhs;
				return *this;
			}
			constexpr _rectangle& operator*=(const _rectangle& rhs)
			{
				x_value_first *= rhs.x_value_first;
				y_value_first *= rhs.y_value_first;
				x_value_second *= rhs.x_value_second;
				y_value_second *= rhs.y_value_second;
				return *this;
			}
			constexpr _rectangle& operator*=(T rhs)
			{
				x_value_first *= rhs;
				y_value_first *= rhs;
				x_value_second *= rhs;
				y_value_second *= rhs;
				return *this;
			}
			constexpr _rectangle& operator/=(const _rectangle& rhs)
			{
				x_value_first /= rhs.x_value_first;
				y_value_first /= rhs.y_value_first;
				x_value_second /= rhs.x_value_second;
				y_value_second /= rhs.y_value_second;
				return *this;
			}
			constexpr _rectangle& operator/=(T rhs)
			{
				x_value_first /= rhs;
				y_value_first /= rhs;
				x_value_second /= rhs;
				y_value_second /= rhs;
				return *this;
			}
			template <typename U> constexpr operator _rectangle<U>() const
			{
				return { static_cast<U>(x_value_first), static_cast<U>(y_value_first), static_cast<U>(x_value_second), static_cast<U>(y_value_second) };
			}
			constexpr operator windows::RECT() const
			{
				return { x_value_first, y_value_first, x_value_second, y_value_second };
			}
		};
		template <typename T> struct _pair
		{
			static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "Numbers only!");
			typedef T type;
			T x_value{}, y_value{};
			constexpr _pair() = default;
			constexpr _pair(T x, T y) : x_value(x), y_value(y) {}
			constexpr _pair(const windows::POINT& p) : _pair{ p.x, p.y } {}
			constexpr _pair(const windows::POINTS& p) : _pair{ p.x, p.y } {}
			constexpr _pair(const windows::SIZE& p) : _pair{ p.cx, p.cy } {}
			constexpr _pair& operator+=(const _pair& rhs)
			{
				x_value += rhs.x_value;
				y_value += rhs.y_value;
				return *this;
			}
			constexpr _pair& operator+=(T rhs)
			{
				x_value += rhs;
				y_value += rhs;
				return *this;
			}
			constexpr _pair& operator-=(const _pair& rhs)
			{
				x_value -= rhs.x_value;
				y_value -= rhs.y_value;
				return *this;
			}
			constexpr _pair& operator-=(T rhs)
			{
				x_value -= rhs;
				y_value -= rhs;
				return *this;
			}
			constexpr _pair& operator*=(const _pair& rhs)
			{
				x_value *= rhs.x_value;
				y_value *= rhs.y_value;
				return *this;
			}
			constexpr _pair& operator*=(T rhs)
			{
				x_value *= rhs;
				y_value *= rhs;
				return *this;
			}
			constexpr _pair& operator/=(const _pair& rhs)
			{
				x_value /= rhs.x_value;
				y_value /= rhs.y_value;
				return *this;
			}
			constexpr _pair& operator/=(T rhs)
			{
				x_value /= rhs;
				y_value /= rhs;
				return *this;
			}
			template <typename U> constexpr operator _pair<U>() const
			{
				return { static_cast<U>(x_value), static_cast<U>(y_value) };
			}
			constexpr operator windows::POINT() const
			{
				return { x_value, y_value };
			}
			constexpr operator windows::POINTS() const
			{
				return { x_value, y_value };
			}
			constexpr operator windows::SIZE() const
			{
				return { x_value, y_value };
			}
		};

		template <typename T> constexpr _rectangle<T>::_rectangle(const _pair<T>& p1, const _pair<T>& p2) : x_value_first(p1.x_value), y_value_first(p1.y_value), x_value_second(p2.x_value), y_value_second(p2.y_value) {}
		template <typename T> constexpr std::pair<_pair<T>, _pair<T>> operator+(const _rectangle<T>& rhs)
		{
			return { {rhs.x_value_first, rhs.y_value_first}, {rhs.x_value_second, rhs.y_value_second} };
		}

		template <typename T, typename U> constexpr auto operator+(_rectangle<T> lhs, const _rectangle<U>& rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) += rhs;
		}
		template <typename T, typename U> constexpr auto operator+(_rectangle<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) += rhs;
		}
		template <typename T, typename U> constexpr auto operator+(T lhs, _rectangle<U> rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return static_cast<_rectangle<type>>(rhs) += lhs;
		}
		template <typename T, typename U> constexpr auto operator-(_rectangle<T> lhs, const _rectangle<U>& rhs)
		{
			typedef decltype(std::declval<T>() - std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) -= rhs;
		}
		template <typename T, typename U> constexpr auto operator-(_rectangle<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() - std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) -= rhs;
		}
		template <typename T, typename U> constexpr auto operator-(T lhs, _rectangle<U> rhs)
		{
			typedef decltype(std::declval<T>() - std::declval<U>()) type;
			return (-static_cast<_rectangle<type>>(rhs)) + lhs;
			//return { static_cast<type>(lhs) - static_cast<type>(rhs.x_value_first), static_cast<type>(lhs) - static_cast<type>(rhs.y_value_first), static_cast<type>(lhs) - static_cast<type>(rhs.x_value_second), static_cast<type>(lhs) - static_cast<type>(rhs.y_value_second) };
		}
		template <typename T, typename U> constexpr auto operator*(_rectangle<T> lhs, const _rectangle<U>& rhs)
		{
			static_assert(std::is_integral_v<U> || std::is_floating_point_v<U>, "Numbers only!");
			typedef decltype(std::declval<T>() * std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) *= rhs;
		}
		template <typename T, typename U> constexpr auto operator*(_rectangle<T> lhs, U rhs)
		{
			static_assert(std::is_integral_v<U> || std::is_floating_point_v<U>, "Numbers only!");
			typedef decltype(std::declval<T>() * std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) *= rhs;
		}
		template <typename T, typename U> constexpr auto operator*(T lhs, _rectangle<U> rhs)
		{
			typedef decltype(std::declval<T>() * std::declval<U>()) type;
			return static_cast<_rectangle<type>>(rhs) *= lhs;
		}
		template <typename T, typename U> constexpr auto operator/(_rectangle<T> lhs, const _rectangle<U>& rhs)
		{
			typedef decltype(std::declval<T>() / std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) /= rhs;
		}
		template <typename T, typename U> constexpr auto operator/(_rectangle<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() / std::declval<U>()) type;
			return static_cast<_rectangle<type>>(lhs) /= rhs;
		}
		template <typename T, typename U> constexpr auto operator/(T lhs, const _rectangle<U>& rhs)
		{
			typedef decltype(std::declval<T>() / std::declval<U>()) type;
			return _rectangle<type>{ static_cast<type>(lhs) / static_cast<type>(rhs.x_value_first), static_cast<type>(lhs) / static_cast<type>(rhs.y_value_first), static_cast<type>(lhs) / static_cast<type>(rhs.x_value_second), static_cast<type>(lhs) / static_cast<type>(rhs.y_value_second) };
		}
		template <typename T> constexpr _rectangle<T> operator-(const _rectangle<T>& r)
		{
			return { -r.x_value_first, -r.y_value_first, -r.x_value_second, -r.y_value_second };
		}
		template <typename T, typename U> constexpr bool operator==(const _rectangle<T>& lhs, const _rectangle<U>& rhs)
		{
			return lhs.x_value_first == rhs.x_value_first && lhs.y_value_first == rhs.y_value_first && lhs.x_value_second == rhs.x_value_second && lhs.y_value_second == rhs.y_value_second;
		}
		template <typename T, typename U> constexpr bool operator!=(const _rectangle<T>& lhs, const _rectangle<U>& rhs)
		{
			return !(lhs == rhs);
		}
		template <typename T, typename U> constexpr auto operator+(_pair<T> lhs, const _pair<U>& rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) += rhs;
		}
		template <typename T, typename U> constexpr auto operator+(_pair<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) += rhs;
		}
		template <typename T, typename U> constexpr auto operator+(T lhs, _pair<U> rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return static_cast<_pair<type>>(rhs) += lhs;
		}
		template <typename T, typename U> constexpr auto operator-(_pair<T> lhs, const _pair<U>& rhs)
		{
			typedef decltype(std::declval<T>() - std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) -= rhs;
		}
		template <typename T, typename U> constexpr auto operator-(_pair<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() - std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) -= rhs;
		}
		template <typename T, typename U> constexpr auto operator-(T lhs, _pair<U> rhs)
		{
			typedef decltype(std::declval<T>() - std::declval<U>()) type;
			return -static_cast<_pair<type>>(rhs) += lhs;
		}
		template <typename T, typename U> constexpr auto operator*(_pair<T> lhs, const _pair<U>& rhs)
		{
			typedef decltype(std::declval<T>() * std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) *= rhs;
		}
		template <typename T, typename U> constexpr auto operator*(_pair<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() * std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) *= rhs;
		}
		template <typename T, typename U> constexpr auto operator*(T lhs, _pair<U> rhs)
		{
			typedef decltype(std::declval<T>() * std::declval<U>()) type;
			return static_cast<_pair<type>>(rhs) *= lhs;
		}
		template <typename T, typename U> constexpr auto operator/(_pair<T> lhs, const _pair<U>& rhs)
		{
			typedef decltype(std::declval<T>() / std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) /= rhs;
		}
		template <typename T, typename U> constexpr auto operator/(_pair<T> lhs, U rhs)
		{
			typedef decltype(std::declval<T>() / std::declval<U>()) type;
			return static_cast<_pair<type>>(lhs) /= rhs;
		}
		template <typename T, typename U> constexpr auto operator/(T lhs, const _pair<U>& rhs)
		{
			typedef decltype(std::declval<T>() + std::declval<U>()) type;
			return _pair<type>{ static_cast<type>(lhs) / static_cast<type>(rhs.x_value), static_cast<type>(lhs) / static_cast<type>(rhs.y_value) };
		}
		template <typename T> constexpr _pair<T> operator-(const _pair<T>& p)
		{
			return { -p.x_value, -p.y_value };
		}
		template <typename T, typename U> constexpr bool operator==(const _pair<T>& lhs, const _pair<U>& rhs)
		{
			return lhs.x_value == rhs.x_value && lhs.y_value == rhs.y_value;
		}
		template <typename T, typename U> constexpr bool operator!=(const _pair<T>& lhs, const _pair<U>& rhs)
		{
			return !(lhs == rhs);
		}
		template <typename T, typename U, typename V> constexpr auto operator<<(std::tuple<T&, U&> lhs, const _rectangle<V>& rhs)
		{
			lhs = +rhs;
			return lhs;
		}
		template <typename T, typename U, typename V> constexpr auto operator<<(std::pair<T&, U&> lhs, const _rectangle<V>& rhs)
		{
			lhs = +rhs;
			return lhs;
		}
	}
}