#pragma once
#include "ConstantChunkSizeGrowth.h"
#include "FixedChunkCacheFriendlyLink.h"
#include "MultiTypeFixedChunk.h"

template<typename TypeList>
struct DefaultMultiTypeFixedChunk
{
	using Type = MultiTypeFixedChunk<TypeList, ConstantChunkSizeGrowth, FixedChunkCacheFriendlyLink<TypeList>>;
};