#ifndef __PYHSICS_MANAGER_INTERFACE_H
#define __PYHSICS_MANAGER_INTERFACE_H


class PhysicsManager;

class PhysicsManagerInterface
{
		PhysicsManager& _physics_manager;

	public:

		PhysicsManagerInterface(PhysicsManager& physics_manager);
		
        void Cleanup();
		//TODO: add more functions after cleanup of physics manager
};

#endif