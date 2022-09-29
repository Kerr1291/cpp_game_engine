#include <RenderableInterface.h>
#include <Renderable.h>
#include <globals.h>

RenderableInterface::RenderableInterface()
:_parent(0)
{
}

RenderableInterface::~RenderableInterface()
{
	if( _parent )
		_parent->_child = 0;
}

void RenderableInterface::SetRenderable(Renderable* parent)
{
	if( _parent )
		_parent->_child = 0;

	_parent = parent;
	_parent->_child = this;
}

void RenderableInterface::SetAnimation(const std::string& animation_name, bool reset)
{
	if( !_parent )
		return;

	std::map<std::string, unsigned>::iterator anim = _parent->_animation_map.find( animation_name );
	if( anim == _parent->_animation_map.end() )
		return;

	_parent->_active_animation = anim->second;

	if( reset ) {
		_parent->_animations[ _parent->_active_animation ]._current_frame = 0;
		_parent->_time = 0;
	}

	_parent->_current_animation_complete = false;
}

void RenderableInterface::SetAnimationDirection(int new_direction)
{
	if( !_parent )
		return;

	if( new_direction > 0 )
		_parent->_direction = 1;
	if( new_direction < 0 )
		_parent->_direction = -1;
}
void RenderableInterface::SetAnimationRate(float new_rate)
{
	if( !_parent )
		return;

	_parent->_rate = new_rate;
}

bool RenderableInterface::HasAnimation(const std::string& animation_name) const
{
	if( !_parent )
		return false;

	if( _parent->_animation_map.find( animation_name ) == _parent->_animation_map.end() )
		return false;

	return true;
}

float RenderableInterface::GetAnimationRate() const
{
	if( !_parent )
		return 0;

	return _parent->_rate;
}
std::string RenderableInterface::GetActiveAnimationName() const
{
	if( !_parent )
		return "";

    return _parent->_animations[ _parent->_active_animation ]._name;
}
bool RenderableInterface::CurrentAnimationComplete() const
{
	if( !_parent )
		return true;

    return _parent->_current_animation_complete;
}