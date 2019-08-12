#include <type_traits>

template <typename From, typename To>
struct Caster
{
	From _value;
	Caster(const From& f) : _value(f) {}
	operator To() { return *reinterpret_cast<To*>(static_cast<void*>(&_value)); }
};

template <typename From, typename To>
using Caster_t = std::conditional_t<std::is_same_v<From, To>, To, Caster<From, To>>;

template <class, class> struct Kuzuki;

template <typename FR, typename... FA, typename TR, typename... TA>
struct Kuzuki<FR(FA...), TR __stdcall(TA...)>
{
	template <TR(__stdcall* F)(TA...), typename... Args>
	static FR call(Args&& ... args)
	{
		return Caster_t<TR, FR>(F(Caster_t<FA, TA>(args)...));
	}
};

#include "WindowsHider.h"
#include <Windows.h>

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
#pragma push_macro("UTILITY_")
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
#define UTILITY_(...)
#define CONSTANT_(...)

namespace bleak
{
	namespace windows
	{
#define FUNCTION_(ret, identifier, types, names, both) ret detail::WindowsHider::_detail##identifier both { return ::Kuzuki<ret types, decltype(::identifier)>::call<&::identifier> names; }
#define MACRO_(ret, identifier, names, both) UTILITY_(ret detail::WindowsHider::_detail##identifier both { return Caster_t<decltype( identifier names ), ret>(identifier names); })
#define UTILITY_(...) __VA_ARGS__ 
#include "WindowsHiderItems.h"
	}
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
#undef UTILITY_
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
#pragma pop_macro("UTILITY_")
#pragma pop_macro("CONSTANT_")

#pragma warning(push)