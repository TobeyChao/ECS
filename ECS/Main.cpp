#include <iostream>
#include "EntityAdmin.h"

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
		for (auto& it : m_ComponentTuples)
		{
			Position* pos = std::get<Position*>(it);
			Velocity* vel = std::get<Velocity*>(it);

			pos->x *= vel->x * deltaTime;
			pos->y *= vel->y * deltaTime;
			pos->z *= vel->z * deltaTime;
		}
	}
};

int main()
{
	EntityAdmin admin;
	//admin.RegisterComponent<Position>();
	//admin.RegisterComponent<Velocity>();

	admin.RegisterSystem<MoveSystem>();

	using t = TypeList<Position, Velocity>;

	const EntityID& id = admin.CreateEntity<t>();
	Position* p1 = admin.SetComponentData<t, Position>(id, 1, 1, 1);
	Velocity* p2 = admin.SetComponentData<t, Velocity>(id, 2, 2, 2);

	admin.Update(0.033f);

	return 0;
}