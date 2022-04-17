#ifndef __FIXED_CHUNK_CACHE_FRIENDLY_LINK__
#define __FIXED_CHUNK_CACHE_FRIENDLY_LINK__
#include <vector>
#include <unordered_map>
#include "ChunkHandle.h"
#include "../MPL/TypeList.h"

#ifdef _DEBUG
#include <iostream>
#endif // DEBUG

class FixedChunkCacheFriendlyLink
{
protected:
	~FixedChunkCacheFriendlyLink()
	{
		for (size_t i = 0; i < m_Chunks.size(); i++)
		{
			free(m_Chunks[i]);
		}
		m_Chunks.clear();
	}

	template<typename TypeList>
	bool Init(uint64_t ChunkSize)
	{
		m_ElementSize = SizeSum<TypeList>::Value;
		m_ElementMaxCount = ChunkSize / m_ElementSize;
		_ParseDataStructure<TypeList>(std::make_index_sequence<Size<TypeList>()>());

#ifdef _DEBUG
		for (auto& [hash, tpl] : m_TypeOffset)
		{
			auto& [offset, size] = tpl;
			std::cout << "Hash: " << hash << " Addr: " << offset << " Size: " << size << std::endl;
		}
#endif // DEBUG
		return true;
	}

	void Grow(uint64_t SizeInBytes)
	{
		void* pChunk = malloc(SizeInBytes);
		m_Chunks.push_back(pChunk);
		m_CurIndex = 0;
	}

	ChunkHandle* Pop()
	{
		if (m_CurIndex >= m_ElementMaxCount)
		{
			return nullptr;
		}
		auto DataIndex = m_CurIndex++;
		auto ChunkIndex = m_Chunks.size() - 1;

		ChunkHandle* handle = new ChunkHandle();
		handle->DataIndex = DataIndex;
		handle->ChunkIndex = ChunkIndex;
		m_Handles[GetHandleKey(ChunkIndex, DataIndex)] = handle;
		return handle;
	}

	template<typename T, typename ...Args>
	T* Create(ChunkHandle* handle, Args&&... args)
	{
		void* chunk = m_Chunks[handle->ChunkIndex];
		uint64_t offset = std::get<0>(m_TypeOffset[typeid(T).hash_code()]);
		void* addr = (uint8_t*)chunk + offset + handle->DataIndex * sizeof(T);
		return new(addr)T(std::forward<decltype(args)>(args)...);
	}

	template<typename T>
	T* Get(ChunkHandle* handle)
	{
		void* chunk = m_Chunks[handle->ChunkIndex];
		uint64_t offset = std::get<0>(m_TypeOffset[typeid(T).hash_code()]);
		void* addr = (uint8_t*)chunk + offset + handle->DataIndex * sizeof(T);
		return (T*)addr;
	}

	void Destroy(ChunkHandle* handle)
	{
		_Destroy(handle);
	}

	void Push(ChunkHandle* handle)
	{
		// 计算得到最后一个数据的Handle
		uint64_t TheLastChunkIndex = m_Chunks.size() - 1;
		uint64_t TheLastDataIndex = m_CurIndex - 1;
		uint64_t TheLastKey = GetHandleKey(TheLastChunkIndex, TheLastDataIndex);
		// 需要释放的
		uint64_t ToReleaseChunkIndex = handle->ChunkIndex;
		uint64_t ToReleaseDataIndex = handle->DataIndex;
		uint64_t ToReleaseKey = GetHandleKey(ToReleaseChunkIndex, ToReleaseDataIndex);

		_Push(handle);

		// 更改最后一个数据的Chunk和Data的Index
		m_Handles[TheLastKey]->ChunkIndex = ToReleaseChunkIndex;
		m_Handles[TheLastKey]->DataIndex = ToReleaseDataIndex;
		m_Handles[ToReleaseKey] = m_Handles[TheLastKey];

		m_Handles.erase(TheLastKey);
		delete handle;
		handle = nullptr;

		m_CurIndex--;
	}

private:
	template<typename TypeList, std::size_t... Is>
	constexpr void _ParseDataStructure(std::index_sequence<Is...>)
	{
		uint64_t offset = 0;
		(_ParseDataStructureImpl<typename TypeAt<Is, TypeList>::Type>(offset), ...);
	}

	template<typename T>
	constexpr void _ParseDataStructureImpl(uint64_t& offset)
	{
		m_TypeOffset[typeid(T).hash_code()] = std::make_tuple(offset, sizeof(T));
		offset = offset + m_ElementMaxCount * sizeof(T);
	}

	void _Destroy(ChunkHandle* handle)
	{
		for (auto& [hash, tpl] : m_TypeOffset)
		{
			auto& [offset, size] = tpl;
			void* chunk = m_Chunks[handle->ChunkIndex];
			void* addr = (uint8_t*)chunk + offset + handle->DataIndex * size;
			// TODO 如何调用析构函数
		}
	}

	void _Push(ChunkHandle* handle)
	{
		for (auto& [hash, tpl] : m_TypeOffset)
		{
			auto& [offset, size] = tpl;
			void* chunk = m_Chunks[handle->ChunkIndex];
			void* theLastChunk = m_Chunks.back();
			uint64_t typeOffset = offset;
			uint64_t dataOffset = handle->DataIndex * size;
			uint64_t lastOffset = (m_CurIndex - 1) * size;
			memcpy((uint8_t*)chunk + typeOffset + dataOffset, (uint8_t*)theLastChunk + typeOffset + lastOffset, size);
			memset((uint8_t*)theLastChunk + typeOffset + lastOffset, 0, size);
		}
	}

	uint64_t GetHandleKey(uint64_t ChunkIndex, uint64_t DataIndex)
	{
		return (ChunkIndex << 32) | DataIndex;
	}

private:
	// 所有已经申请的内存
	std::vector<void*> m_Chunks;
	// Chunk Head
	uint64_t m_CurIndex;
	// TypeHash -> [Offset, Size]
	std::unordered_map<size_t, std::tuple<uint64_t, uint64_t>> m_TypeOffset;

	uint64_t m_ElementMaxCount;
	std::unordered_map<uint64_t, ChunkHandle*> m_Handles;

	size_t m_ElementSize = 0;
};

#endif // !__FIXED_CHUNK_CACHE_FRIENDLY_LINK__