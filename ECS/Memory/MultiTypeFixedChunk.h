#ifndef __MULTI_TYPE_FIXED_CHUNK__
#define __MULTI_TYPE_FIXED_CHUNK__
#include <typeindex>
#include <typeinfo>
#include <cassert>
#include <memory>
#include "../MPL/TypeList.h"
#include "ChunkHandle.h"

template<typename TypeList, typename GrowthPolicy, typename AllocationPolicy>
class MultiTypeFixedChunk : public GrowthPolicy, public AllocationPolicy
{
public:
	MultiTypeFixedChunk()
	{
		uint32_t bytesToPrealloc = GrowthPolicy::GetBytesToPreAllocate();
		AllocationPolicy::ParseDataStructure(bytesToPrealloc);
		if (bytesToPrealloc > 0)
		{
			AllocationPolicy::Grow(bytesToPrealloc);
		}
	}
	MultiTypeFixedChunk(const MultiTypeFixedChunk&) = delete;
	MultiTypeFixedChunk(MultiTypeFixedChunk&&) = delete;
	MultiTypeFixedChunk& operator=(const MultiTypeFixedChunk&) = delete;
	MultiTypeFixedChunk& operator=(MultiTypeFixedChunk&&) = delete;
	~MultiTypeFixedChunk() = default;

	Handle* Allocate()
	{
		Handle* handle = AllocationPolicy::Pop();
		if (!handle)
		{
			uint32_t growSize = GrowthPolicy::GetBytesToGrow();
			if (growSize > 0)
			{
				AllocationPolicy::Grow(growSize);
				handle = AllocationPolicy::Pop();
			}
		}
		return handle;
	}

	template<typename T, typename ...Args>
	T* Create(Handle* handle, Args&&... args)
	{
		return AllocationPolicy::template Create<T, Args...>(handle, std::forward<decltype(args)>(args)...);
	}

	template<typename T>
	T* Get(Handle* handle)
	{
		return AllocationPolicy::template Get<T>(handle);
	}

	void Free(Handle* handle)
	{
		AllocationPolicy::Destroy(handle);
		AllocationPolicy::Push(handle);
	}

private:
};
#endif // !__MULTI_TYPE_FIXED_CHUNK__