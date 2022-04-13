#pragma once
#include "MPL/MPL.h"
#include "System.h"
#include <unordered_map>
#include <typeindex>
#include <typeinfo>

// array<System*>				ϵͳ�б�
// hash_map<EntityID, Entity*>	ʵ���ϣ��
// object_pool<Component>*		�����
// array<Component*>*			����б�

class EntityAdmin
{
public:
	EntityAdmin()
		:
		m_Index(0)
	{}

	void Update(float deltaTime)
	{
		for (auto* it : m_Systems)
		{
			it->Update(deltaTime);
		}
	}

	EntityID CreateEntity()
	{
		m_Entities[m_NextEntityID] = Entity();
		m_Entities[m_NextEntityID].m_EntityID = m_NextEntityID;
		return m_NextEntityID++;
	}

	void DestroyEntity(EntityID entityID)
	{
		m_Entities.erase(entityID);
	}

	// ע��ϵͳ
	template <typename SystemType>
	void RegisterSystem()
	{
		std::add_pointer_t<SystemType> system = new SystemType();
		m_Systems.push_back(system);
	}

	// ע�������ȷ�������Index
	template <typename ComponentType>
	void RegisterComponent()
	{
		if (m_TypeMap.contains(type_index(typeid(ComponentType))))
		{
			return;
		}
		m_TypeMap[type_index(typeid(ComponentType))] = ++m_Index;
	}
private:
	std::vector<ISystem*> m_Systems;
	std::unordered_map<std::type_index, uint64_t> m_TypeMap;
	uint64_t m_Index;

	EntityID m_NextEntityID = 1;
	std::unordered_map<EntityID, Entity> m_Entities;

};