#ifndef __MULTI_TYPE_FIXED_CHUNK__
#define __MULTI_TYPE_FIXED_CHUNK__
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

	ChunkHandle* Allocate()
	{
		ChunkHandle* handle = AllocationPolicy::Pop();
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
	T* Create(ChunkHandle* handle, Args&&... args)
	{
		return AllocationPolicy::template Create<T, Args...>(handle, std::forward<decltype(args)>(args)...);
	}

	template<typename T>
	T* Get(ChunkHandle* handle)
	{
		return AllocationPolicy::template Get<T>(handle);
	}

	void Free(ChunkHandle* handle)
	{
		AllocationPolicy::Destroy(handle);
		AllocationPolicy::Push(handle);
	}

private:
};
#endif // !__MULTI_TYPE_FIXED_CHUNK__