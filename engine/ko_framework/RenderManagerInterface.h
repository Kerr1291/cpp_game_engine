#ifndef __RENDER_MANAGER_INTERFACE_H
#define __RENDER_MANAGER_INTERFACE_H


#include <string>
class RenderManager;

class RenderManagerInterface
{
		RenderManager& _render_manager;

	public:

		RenderManagerInterface(RenderManager& render_manager);
		
		void EnableGameRendering(bool state);
		
		//TODO: add more functions after rewrite of RenderManager
};

#endif