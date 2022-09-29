#include <Editor.h>

#include <BaseGame.h>
#include <Physics.h>

#include <Camera.h>
#include <BaseObject.h>
#include <Transform.h>
#include <PhysicsObject.h>
#include <Area.h>
#include <GridInfo.h>


void Editor::ClearSelections()
{
	//TODO: restore object properties that are lost when the selection logic changes them
	for(unsigned i = 0; i < selected_objects.size(); ++i)
	{
		PhysicsObject* phys = selected_objects[i]->GetPhysicsObject();
		if( phys )
		{
			phys->SetSleepingState(1);
			phys->SetFixedRotation(0);
			phys->SetGravityScale(1.0f);
		}
	}

	/* //TODO: give the editor access to the player somehow? rethink how this part should work
	if( External::player->GetControlTarget() )
	{
		PhysicsObject* phys = External::player->GetControlTarget()->GetPhysicsObject();
		if( phys )
		{
			phys->SetSleepingState(0);
			phys->SetFixedRotation(1);
			phys->SetGravityScale(1.0f);
		}
	}
	*/

	selected_objects.clear();
	External::physics->ClearPicked();
}


void Editor::SelectInsideBox()
{
	CreateSelectionBox(selection_start, selection_end);

	External::physics->PickWorld( selection_start, selection_end  );
	selected_objects = External::physics->GetPickedObjects();
	selected_bodies = External::physics->GetPickedBodiesList();

	//TODO: save object properties so that they may be restored later after selection is done
	for(unsigned i = 0; i < selected_objects.size(); ++i)
	{
		PhysicsObject* phys = selected_objects[i]->GetPhysicsObject();
		if( phys )
		{
			phys->SetSleepingState(0);
			phys->SetFixedRotation(1);
			phys->SetGravityScale(0);
		}
	}
}

void Editor::MoveObjectsWithMouse()
{
	float move_scale = 0.00333f * External::game->GetCurrentCamera()->GetWindowZoom();
	for(unsigned i = 0; i < selected_objects.size(); ++i)
	{
		if( selected_objects[i]->GetPhysicsObject() )
		{
			vec2 move = Global::Mouse::FrameDelta * move_scale;
			selected_objects[i]->GetPhysicsObject()->MovePosition( move );
		}
		else
		{
			selected_objects[i]->t.loc += Global::Mouse::FrameDelta * move_scale;
		}
	}
}


void Editor::SelectionMouseInput()
{
	if( Global::Mouse::GetState( Button::LEFT_DOWN ) )
	{
		in_selection = 1;
		selection_start = Global::Mouse::WorldPos;
		selection_end = Global::Mouse::WorldPos;

		ClearSelections();
	}

	if( Global::Mouse::GetState( Button::LEFT_HELD ) )
	{
		selection_end = Global::Mouse::WorldPos;
	}

	if( Global::Mouse::GetState( Button::LEFT_UP ) )
	{
		in_selection = 0;
		SelectInsideBox();
	}

	if( selected_objects.size() > 0 )
	{
		if( Global::Mouse::GetState( Button::RIGHT_HELD ) )
		{
			MoveObjectsWithMouse();
		}
	}
	else
	{
		if( Global::Mouse::GetState( Button::RIGHT_HELD ) )
		{
			MoveCameraWithMouse();
		}

		if( Global::Mouse::WheelDelta != 0 )
		{
			Camera* c = External::game->GetCurrentCamera();
			if( c )
				c->ZoomWindow( (float)Global::Mouse::WheelDelta );
		}
	}
}

void Editor::SelectionKeyboardInput()
{
	const unsigned char KEY_DELETE = Global::KEY::DEL;


	if( Global::Keyboard::KeyPressed( 'm' ) ) //merge key
	{
		//merges all static bodies that were selected
		External::physics->MergePickedBodies();
	}

	if( Global::Keyboard::KeyPressed( KEY_DELETE ) )
	{
		//delete all the bodies that were picked (if the bodies have BaseObjects attached, those are also deleted)
		External::physics->DeletePickedBodies();
		selected_objects.clear();
	}
}
