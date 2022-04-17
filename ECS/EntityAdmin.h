#ifndef __ENTITY_ADMIN__
#define __ENTITY_ADMIN__
#include "Entity.h"
#include "ISystem.h"
#include "ArchetypeStorage.h"
#include <unordered_map>

// array<System*>				系统列表
// hash_map<EntityID, Entity*>	实体哈希表
// object_pool<Component>*		组件池
// array<Component*>*			组件列表

class EntityAdmin
{
public:
	EntityAdmin()
	{}

	// 创建Entity
	template<typename TypeList>
	EntityID CreateEntity()
	{
		m_Entities[m_NextEntityID] = Entity();
		m_Entities[m_NextEntityID].EntityID = m_NextEntityID;
		m_Entities[m_NextEntityID].MemHandle = Archetype<TypeList>::Pool.Allocate();
		m_Entities[m_NextEntityID].Hash = typeid(TypeList).hash_code();
		return m_NextEntityID++;
	}

	// 设置Component
	template<typename TypeList, typename T, typename ...Args>
	T* SetComponentData(EntityID ID, Args&&... args)
	{
		const Entity& entity = m_Entities[ID];
		return Archetype<TypeList>::Pool.template Create<T, Args...>(entity.MemHandle, std::forward<decltype(args)>(args)...);
	}

	template<typename TypeList, typename T>
	T* GetComponent(EntityID ID)
	{
		const Entity& entity = m_Entities[ID];
		return Archetype<TypeList>::Pool.template Get(entity.MemHandle, typeid(T).hash_code());
	}

	// 销毁Entity
	template<typename TypeList>
	void DestroyEntity(EntityID ID)
	{
		Archetype<TypeList>::Pool.template Free(m_Entities[ID].MemHandle);
		m_Entities.erase(ID);
	}

	// 注册系统
	template <typename SystemType>
	void RegisterSystem()
	{
		ISystem* system = new SystemType();
		m_Systems.push_back(system);
	}

	void Update(float deltaTime)
	{
		for (auto* it : m_Systems)
		{
			it->Update(deltaTime);
		}
	}

private:
	// System
	std::vector<ISystem*> m_Systems;
	// Entity
	EntityID m_NextEntityID = 1;
	std::unordered_map<EntityID, Entity> m_Entities;
};
#endif // __ENTITY_ADMIN__