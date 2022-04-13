#include <iostream>
#include "EntityAdmin.h"

using namespace std;

struct Position
{
	float x;
	float y;
	float z;
};

struct Velocity
{
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
	admin.RegisterComponent<Position>();
	admin.RegisterComponent<Velocity>();

	admin.RegisterSystem<MoveSystem>();

	admin.Update(0.033f);

	return 0;
}