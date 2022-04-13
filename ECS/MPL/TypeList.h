#pragma once
#include <tuple>

#pragma region TypeList
using std::tuple;

template<typename ...TS>
struct TypeList {};

template<typename TypeList>
struct TypeListToTuple {};

template<typename ...TS>
struct TypeListToTuple<TypeList<TS...>>
{
	using Type = std::tuple<TS...>;
};

template<typename ...TS>
using TSToTuple_t = typename TypeListToTuple<TypeList<TS...>>::Type;

template<typename TypeList>
using TypeListToTuple_t = typename TypeListToTuple<TypeList>::Type;
#pragma endregion

#pragma region Size : TypeList
template<typename ...T>
struct TSSize;

template<>
struct TSSize<>
{
	static constexpr size_t Value = 0;
};

template<typename This, typename ...Rest>
struct TSSize<This, Rest...>
{
	static constexpr size_t Value = TSSize<Rest...>::Value + 1;
};

template<typename ...TS>
struct TLSize;

template<typename ...TS>
struct TLSize<TypeList<TS...>>
{
	static constexpr size_t Value = TSSize<TS...>::Value;
};

template<typename TypeList>
constexpr size_t Size()
{
	return TLSize<TypeList>::Value;
}

template<typename ...TS>
constexpr size_t Size_TS()
{
	return TSSize<TS...>::Value;
}
#pragma endregion

#pragma region Contain
template<typename T, typename...TS>
struct TSContain;

template<typename T, typename U>
struct TSContain<T, U>
{
	static constexpr bool Value = std::is_same_v<T, U>;
};

template<typename T, typename U, typename...Rest>
struct TSContain<T, U, Rest...>
{
	static constexpr bool Value = std::is_same_v<T, U> ? true : TSContain<T, Rest...>::Value;
};

template<typename T, typename TypeList>
struct Contain;

template<typename T, typename ...TS>
struct Contain<T, TypeList<TS...>>
{
	static constexpr bool Value = TSContain<T, TS...>::Value;
};

#pragma endregion

#pragma region IndexOf
template<size_t N, typename T, typename...TS>
struct TIndexOf;

template<size_t N, typename T, typename U>
struct TIndexOf<N, T, U>
{
	static constexpr size_t Value = std::is_same_v<T, U> ? N : -1;
};

template<size_t N, typename T, typename U, typename...Rest>
struct TIndexOf<N, T, U, Rest...>
{
	static constexpr size_t Value = std::is_same_v<T, U> ? N : TIndexOf<N + 1, T, Rest...>::Value;
};

template<typename T, typename...TS>
struct TSIndexOf
{
	static constexpr size_t Value = TIndexOf<0, T, TS...>::Value;
};

//template<typename T, typename...TS>
//struct TIndexOf;
//
//// 最后一个匹配，Index就是0
//template<typename T, typename U>
//struct TIndexOf<T, U>
//{
//	static constexpr size_t Value = std::is_same_v<T, U> ? 0 : -1;
//};
//
//template<typename T, typename U, typename...Rest>
//struct TIndexOf<T, U, Rest...>
//{
//	static constexpr size_t Value = std::is_same_v<T, U> ? 0 : TIndexOf<T, Rest...>::Value + 1;
//};
//
//template<typename T, typename...TS>
//struct TSIndexOf
//{
//	static constexpr size_t Value = TIndexOf<T, TS...>::Value;
//};

template<typename T, typename TypeList>
struct IndexOf;

template<typename T, typename ...TS>
struct IndexOf<T, TypeList<TS...>>
{
	static constexpr size_t Value = TSIndexOf<T, TS...>::Value;
};
#pragma endregion