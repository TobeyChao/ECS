#pragma once
class Entity;

class ISystem
{
public:
	virtual void Update(float deltaTime) = 0;

	virtual void OnEntityCreated(const Entity& entity) = 0;
	virtual void OnEntityModified(const Entity& entity) = 0;
	virtual void OnEntityDestroyed(const Entity& entity) = 0;
};