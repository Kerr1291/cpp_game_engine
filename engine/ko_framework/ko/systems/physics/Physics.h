#ifndef PHYSICS_H
#define PHYSICS_H

#include <globals.h>
#include <DebugDraw.h>
#include <PoolManager.h>
#include <PhysicsBody.h>

class GameObject;
class GridInfo;

class PhysicsManager
{
	friend class PhysicsManagerInterface;
	friend class Editor;

		//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////
		//box2d setup stuff (will probably never need to be changed)


        struct CollisionPair
        {
            CollisionPair(GameObject* a = 0, GameObject* b = 0):first(a),second(b){}
            GameObject* first;
            GameObject* second;
        };

        class CollisionListener : public b2ContactListener
        {
			std::vector< CollisionPair >* collisionPairs;
			public:
				void Init(std::vector< CollisionPair >* _collisionPairs) {
						collisionPairs = _collisionPairs;
				}

				// Called when two fixtures begin to touch
				virtual void BeginContact(b2Contact* contact) {
				  GameObject* a = 0; GameObject* b = 0;
				  void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
				  if ( bodyUserData )
						a = static_cast<GameObject*>( bodyUserData );
				  else
						return;

				  bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
				  if ( bodyUserData )
						b = static_cast<GameObject*>( bodyUserData );
				  else
						return;

				  collisionPairs->push_back( CollisionPair(a,b) );
				}
        };
		//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////
				//Physics.cpp
        public:
                PhysicsManager();
                ~PhysicsManager();

				void MakePhysicsBody(GameObject* owner);

                void Init();
                void Update(double dt = 0.016f);
                void Cleanup();
                void SetPause(bool state);
                void SetCollisions(bool state);
                void SetDebugMode(bool state);
                void DoOneStep();
				void CreateLevelBounds(std::vector< std::vector<b2vec2> >& world_edges);
                void DoDebugDraw();
                void ProcessCollisions();
        private:
		//////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////
				//Physics_Picking.cpp
        public:
				void PickWorld(const vec2& pos);
				void PickWorld(float x, float y);
				void PickWorld(const vec2& bottomLeft, const vec2& topRight);
				void ClearPicked();
				void DeletePickedBodies();
				void MergePickedBodies();
				std::vector<GameObject*> GetPickedObjects();
				std::vector<b2Body*>& GetPickedBodiesList();
        private:
		//////////////////////////////////////////////////////////////////////



		//////////////////////////////////////////////////////////////////////
        private:
				b2Body* CombineBodies(b2Body* a, b2Body* b);
				bool IsEdgeShape(b2Body* b);
				bool IsPolygonShape(b2Body* b);
		//////////////////////////////////////////////////////////////////////

				


		//////////////////////////////////////////////////////////////////////
				//Variables
        public:

                bool debugDrawEnabled;
                DebugDraw debugDraw;
                b2World* world;
                CollisionListener collisionListener;
				std::vector<b2Body*> pickedBodies;

                bool ready;

        private:

				PoolManager<PhysicsBody> _physics_bodies;

                std::vector< b2Body* > static_bodies;
				std::vector< b2Body* > world_bounds;
                std::vector< std::vector<b2vec2> > boundingBoxes;
                std::map<std::string, unsigned > loadedBBs;
                std::vector< CollisionPair > collisionPairs;
                bool paused;
                bool oneStep;
                bool collisions;
                bool debugMode;
};



#endif
