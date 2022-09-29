#include <BaseObject.h>
#include <PhysicsObject.h>
#include <RenderObject.h>
#include <sstream>


void TW_CALL SetPlayerControl(void* base_object)
{
	//External::player->SetControlTarget( (BaseObject*)base_object );
	//TODO: update to work with new systems
}

void BaseObject::SetColor(float r, float g, float b, float a)
{
	gfx->SetColor(r,g,b,a);
}

void BaseObject::SetPosition(vec2 pos)
{
    t.loc = pos;
    if( phys )
		phys->SetPosition( pos );
}

void BaseObject::SyncTransformToPhysics()
{
	if( phys )
		phys->SyncParentTransform();
}

void BaseObject::SetSleepState(truth can_sleep)
{
    if( phys )
		phys->SetSleepingState(can_sleep);
}

const std::string BaseObject::StringID() const
{
	std::stringstream tostr;
	tostr << unique_id;
	return tostr.str();
}

void BaseObject::ExposeToTweakbar( TwBar* bar )
{
	std::string parent = ("object " + StringID());
	std::string id_name = parent + "ID";
	TwAddVarRO(bar, id_name.c_str(), TW_TYPE_UINT32, &unique_id, (" group='"+parent+"' label='ID'").c_str());
	
	TwAddButton(bar, ("SetPlayerControl"+parent).c_str(), SetPlayerControl, this, (" group='"+parent+"' label='Set Player Control'").c_str());
	
	t.ExposeToTweakbar(bar, parent);
	prop.ExposeToTweakbar(bar, parent);

	if(gfx)
		gfx->ExposeToTweakbar(bar, parent);

	if(phys)
		phys->ExposeToTweakbar(bar, parent);
}

void BaseObject::RemoveFromTweakbar( TwBar* bar )
{
	std::string parent = ("object " + StringID());
	std::string id_name = parent + "ID";
	TwRemoveVar(bar, id_name.c_str());
	TwRemoveVar(bar, ("SetPlayerControl"+parent).c_str());
	t.RemoveFromTweakbar(bar, parent);
	prop.RemoveFromTweakbar(bar, parent);

	if(gfx)
		gfx->RemoveFromTweakbar(bar, parent);
	
	if(phys)
		phys->RemoveFromTweakbar(bar, parent);
}
