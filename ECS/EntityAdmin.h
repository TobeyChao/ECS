#ifndef __ENTITY_ADMIN__
#define __ENTITY_ADMIN__
#include "MPL/MPL.h"
#include "System.h"
#include "Archetype.h"
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>

// array<System*>				系统列表
// hash_map<EntityID, Entity*>	实体哈希表
// object_pool<Component>*		组件池
// array<Component*>*			组件列表

class EntityAdmin
{
public:
	EntityAdmin()
	{}

	void Update(float deltaTime)
	{
		for (auto* it : m_Systems)
		{
			it->Update(deltaTime);
		}
	}

	template<typename TypeList>
	EntityID CreateEntity()
	{
		m_Entities[m_NextEntityID] = Entity();
		m_Entities[m_NextEntityID].EntityID = m_NextEntityID;
		m_Entities[m_NextEntityID].MemHandle = Archetype<TypeList>::Pool.Allocate();
		return m_NextEntityID++;
	}

	template<typename TypeList, typename T, typename ...Args>
	T* SetComponentData(EntityID ID, Args&&... args)
	{
		const Entity& entity = m_Entities[ID];
		return Archetype<TypeList>::Pool.template Create<T, Args...>(entity.MemHandle, std::forward<decltype(args)>(args)...);
	}

	template<typename TypeList>
	void ChangeSignature(EntityID ID)
	{

	}

	// 销毁Entity
	template<typename TypeList>
	void DestroyEntity(EntityID entityID)
	{
		Archetype<TypeList>::Pool.template Free(m_Entities[entityID].MemHandle);
		m_Entities.erase(entityID);
	}

	// 注册系统
	template <typename SystemType>
	void RegisterSystem()
	{
		ISystem* system = new SystemType();
		m_Systems.push_back(system);
	}

private:
	// System
	std::vector<ISystem*> m_Systems;
	// Entity
	EntityID m_NextEntityID = 1;
	std::unordered_map<EntityID, Entity> m_Entities;
};
#endif // __ENTITY_ADMIN__