#ifndef __FIXED_CHUNK_CACHE_FRIENDLY_LINK__
#define __FIXED_CHUNK_CACHE_FRIENDLY_LINK__
#include <vector>
#include <unordered_map>
#include "ChunkHandle.h"
#include "../MPL/TypeList.h"

template<typename TypeList>
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

	void ParseDataStructure(uint64_t ChunkSize)
	{
		size_t 
		m_ElementMaxCount = ChunkSize / s_ElementSize;
		_ParseDataStructure<Size<TypeList>() - 1>(m_ElementMaxCount * s_ElementSize);

#ifdef _DEBUG
		for (auto& [hash, tpl] : m_TypeOffset)
		{
			auto& [offset, size] = tpl;
			std::cout << "Hash: " << hash << " Addr: " << offset << " Size: " << size << std::endl;
		}
#endif // DEBUG
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
		_Destroy<Size<TypeList>() - 1>(handle);
	}

	void Push(ChunkHandle* handle)
	{
		// 计算得到最后一个数据的Handle
		uint64_t TheLastChunkIndex = m_Chunks.size() - 1;
		uint32_t TheLastDataIndex = m_CurIndex - 1;
		uint64_t TheLastKey = GetHandleKey(TheLastChunkIndex, TheLastDataIndex);
		// 需要释放的
		uint64_t ToReleaseChunkIndex = handle->ChunkIndex;
		uint32_t ToReleaseDataIndex = handle->DataIndex;
		uint64_t ToReleaseKey = GetHandleKey(ToReleaseChunkIndex, ToReleaseDataIndex);

		_Push<Size<TypeList>() - 1>(handle);

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
	template<size_t Index>
	void _ParseDataStructure(uint64_t preOffset)
	{
		using t = typename TypeAt<Index, TypeList>::Type;
		uint64_t offset = preOffset - m_ElementMaxCount * sizeof(t);
		m_TypeOffset[typeid(t).hash_code()] = std::make_tuple(offset, sizeof(t));
		_ParseDataStructure<Index - 1>(offset);
	}

	template<>
	void _ParseDataStructure<0>(uint64_t preOffset)
	{
		using t = typename TypeAt<0, TypeList>::Type;
		m_TypeOffset[typeid(t).hash_code()] = std::make_tuple(preOffset - m_ElementMaxCount * sizeof(t), sizeof(t));
	}

	template<size_t Index>
	void _Destroy(ChunkHandle* handle)
	{
		using t = typename TypeAt<Index, TypeList>::Type;
		void* chunk = m_Chunks[handle->ChunkIndex];
		uint32_t offset = std::get<0>(m_TypeOffset[typeid(t).hash_code()]);
		void* addr = (uint8_t*)chunk + offset + handle->DataIndex * sizeof(t);
		((t*)addr)->~t();
		_Destroy<Index - 1>(handle);
	}

	template<>
	void _Destroy<0>(ChunkHandle* handle)
	{
		using t = typename TypeAt<0, TypeList>::Type;
		void* chunk = m_Chunks[handle->ChunkIndex];
		uint32_t offset = std::get<0>(m_TypeOffset[typeid(t).hash_code()]);
		void* addr = (uint8_t*)chunk + offset + handle->DataIndex * sizeof(t);
		((t*)addr)->~t();
	}

	template<size_t Index>
	void _Push(ChunkHandle* handle)
	{
		void* chunk = m_Chunks[handle->ChunkIndex];
		void* theLastChunk = m_Chunks.back();
		using t = typename TypeAt<Index, TypeList>::Type;
		size_t size = sizeof(t);
		uint32_t typeOffset = std::get<0>(m_TypeOffset[typeid(t).hash_code()]);
		uint32_t dataOffset = handle->DataIndex * sizeof(t);
		uint32_t lastOffset = (m_CurIndex - 1) * sizeof(t);
		memcpy((uint8_t*)chunk + typeOffset + dataOffset, (uint8_t*)theLastChunk + typeOffset + lastOffset, size);
		memset((uint8_t*)theLastChunk + typeOffset + lastOffset, 0, size);
		_Push<Index - 1>(handle);
	}

	template<>
	void _Push<0>(ChunkHandle* handle)
	{
		void* chunk = m_Chunks[handle->ChunkIndex];
		void* theLastChunk = m_Chunks.back();
		using t = typename TypeAt<0, TypeList>::Type;
		size_t size = sizeof(t);
		uint32_t typeOffset = std::get<0>(m_TypeOffset[typeid(t).hash_code()]);
		uint32_t dataOffset = handle->DataIndex * sizeof(t);
		uint32_t lastOffset = (m_CurIndex - 1) * sizeof(t);
		memcpy((uint8_t*)chunk + typeOffset + dataOffset, (uint8_t*)theLastChunk + typeOffset + lastOffset, size);
		memset((uint8_t*)theLastChunk + typeOffset + lastOffset, 0, size);
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

	constexpr static size_t s_ElementSize = SizeSum<TypeList>::Value;
	uint64_t m_ElementMaxCount;
	std::unordered_map<uint64_t, ChunkHandle*> m_Handles;
};

#endif // !__FIXED_CHUNK_CACHE_FRIENDLY_LINK__