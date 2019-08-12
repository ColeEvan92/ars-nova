#pragma once

#include <type_traits>
#include <string>

#pragma warning(push)
#pragma warning(disable : 4005)

#pragma push_macro("HANDLE_")
#pragma push_macro("MUNDANE_ALIAS_")
#pragma push_macro("EXOTIC_ALIAS_")
#pragma push_macro("ALIAS_")
#pragma push_macro("STRUCT_")
#pragma push_macro("STRUCT_AW_")
#pragma push_macro("FUNCTION_")
#pragma push_macro("FUNCTION_AW_")
#pragma push_macro("MACRO_")
#pragma push_macro("CONSTANT_")

#define HANDLE_(...)
#define MUNDANE_ALIAS_(...)
#define EXOTIC_ALIAS_(...)
#define ALIAS_(...)
#define STRUCT_(...)
#define STRUCT_AW_(...)
#define FUNCTION_(...)
#define FUNCTION_AW_(...)
#define MACRO_(...)
#define CONSTANT_(...)

namespace bleak
{
	namespace windows
	{
#define HANDLE_(identifier) typedef struct identifier##__ { int unused; } *identifier;
#include "WindowsHiderItems.h"
#define HANDLE_(...)

#define MUNDANE_ALIAS_(identifier, alias) using alias = identifier; typedef const alias C##alias;
#define EXOTIC_ALIAS_(identifier, alias) typedef identifier *P##alias, *NP##alias, *LP##alias; typedef const identifier *PC##alias, *NPC##alias, *LPC##alias;
#define ALIAS_(identifier, alias) MUNDANE_ALIAS_(identifier, alias) EXOTIC_ALIAS_(identifier, alias)
#define STRUCT_(identifier, body, alias) struct identifier; ALIAS_(identifier, alias)
#include "WindowsHiderItems.h"
#define ALIAS_(...)
#define EXOTIC_ALIAS_(...)
#define MUNDANE_ALIAS_(...)
#define STRUCT_(...)

#ifdef _UNICODE
#define STRUCT_AW_(identifier) typedef identifier##W identifier;
#else
#define STRUCT_AW_(identifier) typedef identifier##A identifier;
#endif
#include "WindowsHiderItems.h"
#define STRUCT_AW_(...)

#define STRUCT_(identifier, body, alias) struct identifier body;
#include "WindowsHiderItems.h"
#define STRUCT_(...)

		namespace detail
		{
			namespace WindowsHider
			{
#define FUNCTION_(ret, identifier, types, names, both) ret _detail##identifier both;
#define MACRO_(ret, identifier, names, both) ret _detail##identifier both;
#include "WindowsHiderItems.h"
#define FUNCTION_(...)
#define MACRO_(...)
			}
		}

#define FUNCTION_(ret, identifier, types, names, both) constexpr ret (& identifier) both = detail::WindowsHider::_detail##identifier; 
#define MACRO_(ret, identifier, names, both) constexpr ret (& identifier) both = detail::WindowsHider::_detail##identifier;
#include "WindowsHiderItems.h"
#define FUNCTION_(...)
#define MACRO_(...)

#ifdef _UNICODE
#define FUNCTION_AW_(identifier) constexpr std::add_lvalue_reference_t<decltype(identifier##W)> identifier = identifier##W;
#else
#define FUNCTION_AW_(identifier) constexpr std::add_lvalue_reference_t<decltype(identifier##A)> identifier = identifier##A;
#endif
#include "WindowsHiderItems.h"
#define FUNCTION_AW_(...)

		namespace detail
		{
			namespace WindowsHider
			{
				template <typename Expected, typename Actual>
				class ConstantHack
				{
				public:
					constexpr explicit ConstantHack(Actual value) : _value(value) {}
					constexpr ConstantHack(const ConstantHack&) = default;
					constexpr ConstantHack& operator=(const ConstantHack&) = default;
					~ConstantHack() = default;
					operator Expected() const
					{
						return reinterpret_cast<Expected>(_value);
					}
				private:
					const Actual _value;
				};

				template <typename Expected, typename Actual>
				using Constant_t = std::conditional_t<std::is_pointer_v<Expected>, ConstantHack<Expected, Actual>, Expected>;
			}
		}

#define CONSTANT_(type, identifier, value) constexpr detail::WindowsHider::Constant_t<type, decltype(value)> identifier = static_cast<detail::WindowsHider::Constant_t<type, decltype(value)>>(value);
#include "WindowsHiderItems.h"
#define CONSTANT_(...)
	}
	struct StartupData
	{
		windows::HINSTANCE hInstance;
		windows::HINSTANCE hPrevInstance;
		windows::LPSTR lpCmdArgs;
		int nCmdShow;
	};
#ifdef _UNICODE
#define STR(text) L##text
#define CHR(chr) L##chr
	typedef std::wstring bstring;
	typedef wchar_t bchar;
	template <typename T> bstring to_bstring(T&& v) { return std::to_wstring(std::forward<T>(v)); }
	typedef std::wstringstream bstringstream;
#else
#define STR(text) text
#define CHR(chr) chr
	typedef std::string bstring;
	typedef char bchar;
	template <typename T> bstring to_bstring(T&& v) { return std::to_string(std::forward<T>(v)); }
	typedef std::stringstream bstringstream;
#endif
}

#undef HANDLE_
#undef MUNDANE_ALIAS_
#undef EXOTIC_ALIAS_
#undef ALIAS_
#undef STRUCT_
#undef STRUCT_AW_
#undef FUNCTION_
#undef FUNCTION_AW_
#undef MACRO_
#undef CONSTANT_

#pragma pop_macro("HANDLE_")
#pragma pop_macro("MUNDANE_ALIAS_")
#pragma pop_macro("EXOTIC_ALIAS_")
#pragma pop_macro("ALIAS_")
#pragma pop_macro("STRUCT_")
#pragma pop_macro("STRUCT_AW_")
#pragma pop_macro("FUNCTION_")
#pragma pop_macro("FUNCTION_AW_")
#pragma pop_macro("MACRO_")
#pragma pop_macro("CONSTANT_")

#pragma warning(push)