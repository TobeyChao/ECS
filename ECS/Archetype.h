#pragma once
#include "Memory/DefaultMultiTypeFixedChunk.h"

template<typename TypeList>
struct Archetype
{
	using PoolType = typename DefaultMultiTypeFixedChunk<TypeList>::Type;
	static PoolType Pool;
};

template<typename TypeList>
typename Archetype<TypeList>::PoolType Archetype<TypeList>::Pool = Archetype<TypeList>::PoolType();