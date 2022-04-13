#pragma once
#include "ISystem.h"
#include "Entity.h"
#include <tuple>
#include <unordered_map>
#include <vector>

// Update(float)
// NotifyComponent(Component*)

template<typename ...ComponentType>
class System : public ISystem
{
public:
	virtual void OnEntityCreated(const Entity& entity)
	{

	}

	virtual void OnEntityModified(const Entity& entity)
	{

	}

	virtual void OnEntityDestroyed(const Entity& entity)
	{

	}

public:
	static constexpr uint8_t s_NumComponentCount = sizeof...(ComponentType);

protected:
	using ComponentTuple = std::tuple<std::add_pointer_t<ComponentType>...>;
	std::vector<ComponentTuple> m_ComponentTuples;

private:
	std::unordered_map<EntityID, size_t> m_EntityIDToIndex;
};