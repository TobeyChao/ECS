#include <iostream>
#include "EntityAdmin.h"
#include "System.h"

using namespace std;

struct Position
{
	Position(float _x, float _y, float _z)
		:
		x(_x),
		y(_y),
		z(_z)
	{}

	float x;
	float y;
	float z;
};

struct Velocity
{
	Velocity(float _x, float _y, float _z)
		:
		x(_x),
		y(_y),
		z(_z)
	{}

	float x;
	float y;
	float z;
};

class MoveSystem : public System<Position, Velocity>
{
public:
	virtual void Update(float deltaTime) override
	{
		for (auto& it : m_EntitiesCache)
		{
			Position* pos = Get<Position>(it);
			Velocity* vel = Get<Velocity>(it);

			pos->x *= vel->x * deltaTime;
			pos->y *= vel->y * deltaTime;
			pos->z *= vel->z * deltaTime;
		}
	}
};

int main()
{
	EntityAdmin admin;
	admin.RegisterSystem<MoveSystem>();

	using T = TypeList<Position, Velocity>;
	const EntityID& id = admin.CreateEntity<T>();
	Position* p1 = admin.SetComponentData<T, Position>(id, 1.0f, 1.0f, 1.0f);
	Velocity* p2 = admin.SetComponentData<T, Velocity>(id, 2.0f, 2.0f, 2.0f);

	admin.Update(0.033f);

	return 0;
}