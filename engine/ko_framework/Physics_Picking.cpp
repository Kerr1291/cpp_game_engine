#include <Physics.h>

#include <BaseGame.h>
#include <GameObject.h>

class PickingCallback : public b2QueryCallback
{
public:
	std::vector<b2Body*> pickedBodies;

	bool ReportFixture(b2Fixture* fixture) {
		pickedBodies.push_back( fixture->GetBody() );
		return true;
	}
};

void PhysicsManager::PickWorld(const vec2& pos)
{
	PickWorld(pos.x, pos.y);
}

void PhysicsManager::PickWorld(float x, float y)
{
	PickWorld(vec2(x-0.005f,y-0.005f), vec2(x+0.005f,y+0.005f));
}

void PhysicsManager::PickWorld(const vec2& bottomLeft, const vec2& topRight)
{
	b2AABB pickingBox;
	pickingBox.lowerBound = b2vec2(bottomLeft.x,bottomLeft.y);
	pickingBox.upperBound = b2vec2(topRight.x,topRight.y);
	pickedBodies.clear();
	PickingCallback pickedObjects;
	world->QueryAABB( &pickedObjects, pickingBox );
	pickedBodies = pickedObjects.pickedBodies;
}

void PhysicsManager::ClearPicked()
{
	pickedBodies.clear();
}

void PhysicsManager::DeletePickedBodies()
{
	std::set< b2Body* > removed_bodies;
	for(unsigned i = 0; i < pickedBodies.size(); ++i)
	{
		if( removed_bodies.find( pickedBodies[i] ) != removed_bodies.end() )
			continue;

		GameObject* object = 0;
		object = static_cast<GameObject*>(pickedBodies[i]->GetUserData());
		if( object )
			External::game->RemoveObject( object ); //TODO: make into a command so we don't access the game interface directly. want all removes wrapped in commands
		else
			world->DestroyBody( pickedBodies[i] );

		removed_bodies.insert( pickedBodies[i] );
	}
	ClearPicked();
}

void PhysicsManager::MergePickedBodies()
{
	if( pickedBodies.size() < 2 )
		return;

	b2Body* first = 0;
	unsigned start_index = 0;
	for(unsigned i = start_index; i < pickedBodies.size(); ++i)
	{
		//find the first static body, start there
		if( pickedBodies[i]->GetType() == b2_staticBody )
		{
			//if it's not a polygon shape or an edge shape we skip it
			if( IsPolygonShape(pickedBodies[i]) == 0 && IsEdgeShape(pickedBodies[i]) == 0 )
				continue;

			first = pickedBodies[i];
			start_index = i+1;
			break;
		}
	}

	if( !first )
	{
		ClearPicked();
		return;
	}

	for(unsigned i = start_index; i < pickedBodies.size(); ++i)
	{
		//just in case, don't let use merge the same two bodies together
		if( first == pickedBodies[i] )
			continue;

		//if it's not a polygon shape or an edge shape we skip it
		if( IsPolygonShape(pickedBodies[i]) == 0 && IsEdgeShape(pickedBodies[i]) == 0 )
			continue;

		b2Body* result = CombineBodies(first, pickedBodies[i]);
		if( result )
			first = result;
	}
	ClearPicked();
}

std::vector<GameObject*> PhysicsManager::GetPickedObjects()
{
	std::vector<GameObject*> list;
	for(unsigned i = 0; i < pickedBodies.size(); ++i)
	{
		GameObject* object = 0;
		object = static_cast<GameObject*>(pickedBodies[i]->GetUserData());
		if( object )
		{
			list.push_back( object );
		}
	}
	return list;
}

std::vector<b2Body*>& PhysicsManager::GetPickedBodiesList()
{
	return pickedBodies;
}

void PhysicsManager::DoDebugDraw()
{
    if(debugDrawEnabled)
    {
        //debug draw physics
        debugDraw.DrawString(5, 15, "Physics Debug Drawing Enabled");

        world->DrawDebugData();
    }
}
