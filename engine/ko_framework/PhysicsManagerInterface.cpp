#include <PhysicsManagerInterface.h>
#include <Physics.h>

PhysicsManagerInterface::PhysicsManagerInterface(PhysicsManager& physics_manager)
:_physics_manager(physics_manager)
{}

        
void PhysicsManagerInterface::Cleanup()
{
	_physics_manager.Cleanup();
}