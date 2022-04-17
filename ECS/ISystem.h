#pragma once
class Entity;
class EntityAdmin;

class ISystem
{
public:
	virtual void SetEntityAdmin(EntityAdmin* admin) = 0;

	virtual void Update(float deltaTime) = 0;

	virtual void OnEntityCreated(const Entity& entity) = 0;
	virtual void OnEntityModified(const Entity& entity) = 0;
	virtual void OnEntityDestroyed(const Entity& entity) = 0;
};