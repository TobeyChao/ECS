#include <iostream>
#include <thread>
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
			Position* pos = GetComponent<Position>(it);
			Velocity* vel = GetComponent<Velocity>(it);

			pos->x += vel->x * deltaTime;
			pos->y += vel->y * deltaTime;
			pos->z += vel->z * deltaTime;

			//cout << "Now Pos: (" << pos->x << ", " << pos->y << ", " << pos->z << ")" << endl;
		}
	}
};

//struct MyStruct
//{
//	Position* pos;
//	Velocity* vel;
//};
//
//std::vector<MyStruct*> g_Vec;

int main()
{
	EntityAdmin* admin = new EntityAdmin();
	admin->RegisterSystem<MoveSystem>();

	using T = TypeList<Position, Velocity>;
	admin->RegisterArchetype<T>();

	for (size_t i = 0; i < 683; i++)
	{
		const EntityID& id = admin->CreateEntity<T>();
		admin->SetComponentData<Position>(id, 1.0f, 1.0f, 1.0f);
		admin->SetComponentData<Velocity>(id, 2.0f, 2.0f, 2.0f);
	}
	
	//while (true)
	//{
	//	auto start = std::chrono::high_resolution_clock::now();
	//	admin->Update(0.033f);
	//	auto end = std::chrono::high_resolution_clock::now();
	//	std::chrono::duration<double, std::milli> elapsed = end - start;
	//	std::cout << "Waited " << elapsed.count() << " ms\n";
	//}

	admin->DestroyEntity(681);

	const EntityID& id = admin->CreateEntity<T>();
	admin->SetComponentData<Position>(id, 1.0f, 1.0f, 1.0f);
	admin->SetComponentData<Velocity>(id, 2.0f, 2.0f, 2.0f);

	admin->Shutdown();
	delete admin;

	//for (size_t i = 0; i < 10000; i++)
	//{
	//	MyStruct* s = new MyStruct();
	//	s->pos = new Position(1.0f, 1.0f, 1.0f);
	//	s->vel = new Velocity(2.0f, 2.0f, 2.0f);
	//	g_Vec.push_back(s);
	//}

	//while (true)
	//{
	//	auto start = std::chrono::high_resolution_clock::now();
	//	
	//	for (size_t i = 0; i < g_Vec.size(); i++)
	//	{
	//		Position* pos = g_Vec[i]->pos;
	//		Velocity* vel = g_Vec[i]->vel;

	//		pos->x += vel->x * 0.033f;
	//		pos->y += vel->y * 0.033f;
	//		pos->z += vel->z * 0.033f;
	//	}

	//	auto end = std::chrono::high_resolution_clock::now();
	//	std::chrono::duration<double, std::milli> elapsed = end - start;
	//	std::cout << "Waited " << elapsed.count() << " ms\n";
	//}

	return 0;
}