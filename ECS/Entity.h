#pragma once
#include "TypeID.h"
#include "Memory/ChunkHandle.h"
#include <unordered_set>

class Entity
{
public:
	EntityID EntityID;
	Handle* MemHandle;
};