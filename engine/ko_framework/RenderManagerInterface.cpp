#include <RenderManagerInterface.h>
#include <RenderManager.h>

RenderManagerInterface::RenderManagerInterface(RenderManager& render_manager)
:_render_manager(render_manager)
{}

void RenderManagerInterface::EnableGameRendering(bool state)
{
	_render_manager.EnableGameRendering(state);
}
		