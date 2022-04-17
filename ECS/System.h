#pragma once
#include "MPL/TypeList.h"

template<typename ...ComponentType>
class System : public ISystem
{
public:
	using Type = TypeList<ComponentType...>;

protected:
	using ComponentTuple = std::tuple<std::add_pointer_t<ComponentType>...>;

public:
	System()
	{
		ParseDataStructure();
	}

	void SetEntityAdmin(EntityAdmin* admin)
	{
		m_Admin = admin;
	}

	void ParseDataStructure()
	{
		_ParseDataStructure<s_NumComponentCount - 1>();

#ifdef _DEBUG
		for (auto& hash : m_ComponentHash)
		{
			std::cout << "Hash: " << hash << std::endl;
		}
#endif // DEBUG
	}

	virtual void OnEntityCreated(const Entity& entity)
	{
		for (const size_t& comHash : m_ComponentHash)
		{
			if (!entity.ComponentHash.contains(comHash))
			{
				return;
			}
		}
		m_EntitiesCache.push_back(entity.EntityID);
		m_EntityIDToIndex[entity.EntityID] = m_EntitiesCache.size() - 1;
	}

	virtual void OnEntityModified(const Entity& entity)
	{
		// 如果改变了的Entity没有符合的组件啦，就把他删掉啦
		for (const size_t& comHash : m_ComponentHash)
		{
			if (!entity.ComponentHash.contains(comHash))
			{
				RemoveEntityCache(entity.EntityID);
				return;
			}
		}
	}

	virtual void OnEntityDestroyed(const Entity& entity)
	{
		RemoveEntityCache(entity.EntityID);
	}

	template<typename T>
	T* Get(EntityID ID)
	{
		return nullptr;
	}

private:
	void RemoveEntityCache(EntityID ID)
	{
		size_t index = m_EntityIDToIndex[ID];
		m_EntitiesCache.erase(m_EntitiesCache.begin() + index);
		m_EntityIDToIndex.erase(ID);
	}

private:
	template<size_t Index>
	void _ParseDataStructure()
	{
		using t = typename TypeAt<Index, Type>::Type;
		m_ComponentHash.insert(typeid(t).hash_code());
		_ParseDataStructure<Index - 1>();
	}

	template<>
	void _ParseDataStructure<0>()
	{
		using t = typename TypeAt<0, Type>::Type;
		m_ComponentHash.insert(typeid(t).hash_code());
	}

public:
	static constexpr uint8_t s_NumComponentCount = sizeof...(ComponentType);

protected:
	std::vector<EntityID> m_EntitiesCache;

private:
	std::unordered_map<EntityID, size_t> m_EntityIDToIndex;
	std::unordered_set<size_t> m_ComponentHash;
	EntityAdmin* m_Admin;
};