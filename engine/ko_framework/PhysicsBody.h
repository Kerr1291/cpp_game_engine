#ifndef __PHYSICS_BODY_H
#define __PHYSICS_BODY_H

#include <Component.h>
#include <globals.h>

class Transform;
class PhysicsManager;
class PhysicsBodyInterface;

class PhysicsBody;
template <typename T> class ComponentPool;

BEGIN_COMPONENT( PhysicsBody )

	friend class PhysicsBodyInterface;
	friend class PhysicsManager;
	friend class Editor;
	template <typename T> friend class ComponentPool;

		PhysicsBodyInterface* _child;
		ComponentPool<PhysicsBody>* _owner;

	public:
        
		Transform& _transform;

	private:

        b2Body* _root_body;
        unsigned _num_fixtures;

	private:

		PhysicsBody(Transform& transform);

		void KillThisComponent();
		void SyncGameObjectTransform();

	public:
		
		PhysicsBody(const PhysicsBody& other);
		~PhysicsBody();
		void operator=(const PhysicsBody& other){}//disabled

		void CreateRootBody(b2BodyType bodytype);

		void AddFixture(std::vector<b2vec2> bounds);

		unsigned GetNumFixtures() const;

		b2Fixture* GetFixture(unsigned fixture_num) const;

		bool RemoveFixture(unsigned fixture_num);
		bool RemoveFixture(b2Fixture* fixture);

		void Serialize(SerializedObject& obj);

};

#endif