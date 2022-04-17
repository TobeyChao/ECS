#pragma once
#include "Memory/DefaultMultiTypeFixedChunk.h"
#include <memory>

struct Archetype
{
	using PoolType = typename DefaultMultiTypeFixedChunk::Type;
};

namespace ArchetypeStorage
{
	std::unordered_map<size_t, Archetype::PoolType*> g_Pools;

	Archetype::PoolType& Get(size_t hash)
	{
		if (!g_Pools.contains(hash))
		{
			g_Pools[hash] = new Archetype::PoolType();
		}
		return *g_Pools[hash];
	}
}