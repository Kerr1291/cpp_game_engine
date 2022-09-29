#include <Physics.h>

#include <GridInfo.h>
#include <Area.h>

#include <fstream>


PhysicsManager::PhysicsManager()
{
    world = 0;
    paused = 0;
    oneStep = 0;
    collisions = 1;
    debugDrawEnabled = 0;
    debugMode = 0;
    ready = 0;
}

PhysicsManager::~PhysicsManager()
{
	delete world;
}

void PhysicsManager::MakePhysicsBody(GameObject* owner)
{
	_physics_bodies.MakeComponent( owner );
}

void PhysicsManager::Init()
{
    b2Vec2 gravity = b2Vec2(0, -40);
    world = new b2World(gravity);

    world->SetDebugDraw(&debugDraw);
    debugDraw.SetFlags(b2Draw::e_shapeBit);

    collisionListener.Init( &collisionPairs );
    world->SetContactListener( &collisionListener );
}

void PhysicsManager::Update(double dt) 
{
	if(paused)
		return;

	if(oneStep)
	{
		oneStep = false;
		paused = true;
	}

	world->Step( static_cast<float32>(dt), 6, 2);
	world->ClearForces();

	if(collisions)
		ProcessCollisions();
}


void PhysicsManager::Cleanup()
{
	for( unsigned i = 0; i < world_bounds.size(); ++i )
	{
       world->DestroyBody(world_bounds[i]);
	}
	world_bounds.clear();

	for( unsigned i = 0; i < static_bodies.size(); ++i )
	{
       world->DestroyBody(static_bodies[i]);
	}
	static_bodies.clear();

	_physics_bodies.ClearPools();
}

void PhysicsManager::SetPause(bool state)
{
    paused = state;
    oneStep = false;
}

void PhysicsManager::SetCollisions(bool state)
{
    collisions = state;
}

void PhysicsManager::SetDebugMode(bool state)
{
	if(state)
		debugDrawEnabled = true;
	else
		debugDrawEnabled = false;
	debugMode = state;
}

void PhysicsManager::DoOneStep()
{
    paused = false;
    oneStep = true;
}


void PhysicsManager::ProcessCollisions()
{
    for(unsigned i = 0; i < collisionPairs.size(); ++i)
    {
            collisionPairs[i].first->DoCollision( collisionPairs[i].second );
            collisionPairs[i].second->DoCollision( collisionPairs[i].first );
    }
    collisionPairs.clear();
}

void PhysicsManager::CreateLevelBounds(std::vector< std::vector<b2vec2> >& world_edges)
{
	for( unsigned i = 0; i < world_edges.size(); ++i )
	{
		b2BodyDef bd;
		b2Body* ground = world->CreateBody(&bd);
		for(unsigned j = 1; j < world_edges[i].size(); ++j )
		{
			b2EdgeShape shape;
			shape.Set(world_edges[i][j-1], world_edges[i][j]);
			ground->CreateFixture(&shape, 0.0f);
		}
		world_bounds.push_back( ground );
	}
}
