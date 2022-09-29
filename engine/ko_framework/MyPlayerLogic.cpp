#include <MyPlayerLogic.h>
#include <GameObject.h>

PlayerLogic::PlayerLogic()
:previous_fps(0.016)
,_control_target(0)
{}

PlayerLogic::~PlayerLogic()
{}

void PlayerLogic::Init()
{}

void PlayerLogic::Input(double fps)
{}

void PlayerLogic::Update(double fps)
{}

void PlayerLogic::SetControlObject(GameObject* object)
{
	_control_target = object;
}

BaseObject* PlayerLogic::GetControlObject()
{
	return _control_target;
}
