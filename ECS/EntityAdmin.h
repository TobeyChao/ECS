#ifndef __ENTITY_ADMIN__
#define __ENTITY_ADMIN__
#include "Entity.h"
#include "ISystem.h"
#include "ArchetypeStorage.h"
#include <unordered_map>

// array<System*>				ϵͳ�б�
// hash_map<EntityID, Entity*>	ʵ���ϣ��
// object_pool<Component>*		�����
// array<Component*>*			����б�

class EntityAdmin
{
public:
	EntityAdmin()
	{}

	// ����Entity
	template<typename TypeList>
	EntityID CreateEntity()
	{
		Entity entity;
		entity.Hash = typeid(TypeList).hash_code();
		entity.EntityID = m_NextEntityID;
		entity.MemHandle = ArchetypeStorage::Get(typeid(TypeList).hash_code()).Allocate();
		// ����Entityǩ��
		_SetEntityComHash<TypeList>(entity, std::make_index_sequence<Size<TypeList>()>());
		m_Entities[m_NextEntityID] = std::move(entity);

		NotifySystemEntityCreated(m_Entities[m_NextEntityID]);
		return m_NextEntityID++;
	}

	// ����Component
	template<typename T, typename ...Args>
	T* SetComponentData(EntityID ID, Args&&... args)
	{
		const Entity& entity = m_Entities[ID];
		T* ret = ArchetypeStorage::Get(entity.Hash).template Create<T, Args...>(entity.MemHandle, std::forward<decltype(args)>(args)...);
		return ret;
	}

	// ��ȡComponent
	template<typename T>
	T* GetComponent(EntityID ID)
	{
		const Entity& entity = m_Entities[ID];
		return ArchetypeStorage::Get(entity.Hash).Get<T>(entity.MemHandle);
	}

	// ����Entity
	void DestroyEntity(EntityID ID)
	{
		NotifySystemEntityCreated(m_Entities[ID]);

		ArchetypeStorage::Get(m_Entities[ID].Hash).Free(m_Entities[ID].MemHandle);
		m_Entities.erase(ID);
	}

	// ע��ϵͳ
	template <typename SystemType>
	void RegisterSystem()
	{
		ISystem* system = new SystemType();
		system->SetEntityAdmin(this);
		m_Systems.push_back(system);
	}

	// ע��Archetype
	template <typename TypeList>
	void RegisterArchetype()
	{
		ArchetypeStorage::Get(typeid(TypeList).hash_code()).template Init<TypeList>();
	}

	void Update(float deltaTime)
	{
		for (auto* it : m_Systems)
		{
			it->Update(deltaTime);
		}
	}

private:
	template<typename TypeList, std::size_t... Is>
	constexpr void _SetEntityComHash(Entity& entity, std::index_sequence<Is...>)
	{
		(_SetEntityComHashImpl<typename TypeAt<Is, TypeList>::Type>(entity), ...);
	}

	template<typename T>
	constexpr void _SetEntityComHashImpl(Entity& entity)
	{
		entity.ComponentHash.insert(typeid(T).hash_code());
	}


	void NotifySystemEntityCreated(const Entity& entity)
	{
		for (auto* it : m_Systems)
		{
			it->OnEntityCreated(entity);
		}
	}

	void NotifySystemEntityModified(const Entity& entity)
	{
		for (auto* it : m_Systems)
		{
			it->OnEntityModified(entity);
		}
	}

	void NotifySystemEntityDestroyed(const Entity& entity)
	{
		for (auto* it : m_Systems)
		{
			it->OnEntityDestroyed(entity);
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