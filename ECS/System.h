#pragma once
#include "ISystem.h"
#include "Entity.h"
#include "MPL/TypeList.h"
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
	}

	virtual void OnEntityModified(const Entity& entity)
	{
		size_t index = m_EntityIDToIndex[entity.EntityID];
		const ComponentTuple& tpl = m_ComponentTuples[index];
	}

	virtual void OnEntityDestroyed(const Entity& entity)
	{
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
	std::vector<ComponentTuple> m_ComponentTuples;

private:
	std::unordered_map<EntityID, size_t> m_EntityIDToIndex;
	std::unordered_map<size_t, EntityID> m_IndexToEntityID;

	std::unordered_set<size_t> m_ComponentHash;
};