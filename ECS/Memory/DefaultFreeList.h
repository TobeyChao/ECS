#pragma once
#include "FreeList.h"
#include "PlacementNewEmbeddedLink.h"
#include "ConstantGrowth.h"

template <typename T>
struct DefaultFreeList
{
	using Type = FreeList<T, ConstantGrowth, PlacementNewEmbeddedLink<T>>;
};