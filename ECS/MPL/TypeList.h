#pragma once
#include <tuple>

using std::tuple;

template<typename ...TS>
struct TypeList {};

template<typename TypeList>
struct TypeListToTuple {};

// �����ʵ��ģ���ػ�
template<typename ...TS>
struct TypeListToTuple<TypeList<TS...>>
{
	using Type = std::tuple<TS...>;
};

template<typename ...TS>
using TSToTuple_t = typename TypeListToTuple<TypeList<TS...>>::Type;

template<typename TypeList>
using TypeListToTuple_t = typename TypeListToTuple<TypeList>::Type;
