#pragma once

namespace detail
{
	template <char...> struct Container {};
	template <class> struct MakeString;
	template <char... C> struct MakeString<Container<C...>>
	{
		static constexpr char value[] = { C..., '\0' };
	};
	template <size_t I, char... C> struct IToS //Creates Container for the number
	{
		typedef typename IToS<I / 10, static_cast<char>(I % 10 + '0'), C...>::type type;
	};
	template <char... C> struct IToS<0, C...>
	{
		typedef Container<C..., '\n'> type;
	};
	template <size_t I, bool = I % 3 == 0, bool = I % 5 == 0> struct GetChars //Gets the output for the number
	{
		typedef typename IToS<I>::type type;
	};
	template <size_t I> struct GetChars<I, true, false>
	{
		typedef Container<'F', 'I', 'Z', 'Z', '\n'> type;
	};
	template <size_t I> struct GetChars<I, false, true>
	{
		typedef Container<'B', 'U', 'Z', 'Z', '\n'> type;
	};
	template <class, class> struct Join;
	template <char... C1, char... C2> struct Join < Container<C1...>, Container<C2...>>
	{
		typedef Container<C1..., C2...> type;
	};
	template <size_t I> struct GetChars<I, true, true>
	{
		typedef Container<'F', 'I', 'Z', 'Z', 'B', 'U', 'Z', 'Z', '\n'> type;
	};
	template <size_t Min, size_t Max> struct Split //Log template recursion depth instead of linear
	{
		typedef typename Join<typename Split<Min, (Min + Max) / 2>::type, typename Split<(Min + Max) / 2 + 1, Max>::type>::type type;
	};
	template <size_t I> struct Split<I, I>
	{
		typedef typename GetChars<I>::type type;
	};
}

template <size_t I> using ConstexprFizzBuzz = detail::MakeString<typename detail::Split<0, I>::type>;
template <size_t I> constexpr auto& ConstexprFizzBuzz_v = ConstexprFizzBuzz<I>::value;