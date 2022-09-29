#include <Editor.h>

#include <BaseGame.h>
#include <Physics.h>

#include <Camera.h>
#include <BaseObject.h>
#include <Transform.h>
#include <PhysicsObject.h>
#include <Area.h>
#include <GridInfo.h>

#include <CustomObject.h>

///////////////////////////////////////////////////////////////////////////////
// Tweakbar Functions

/*

void TW_CALL CreateNewObject(void* null)
{
	if( !External::game->GetCurrentArea() )
		return; //cannot add objects to a null area

	vec2 creation_position = External::game->GetCurrentCamera()->GetPos();//Global::Mouse::WorldPos;
	//External::gi->AddCommand( new LoadGameObject<CustomObject>( External::gi->GetCurrentArea(), creation_position ) );
}

void TW_CALL SetBuildMode(const void* data, void* var_to_set)
{
	*(truth*)var_to_set = *(const truth*)data;
	External::editor->SetObjectEditorMode( *(truth*)var_to_set );
}

void TW_CALL GetBuildMode(void* data, void* var_to_get)
{
	*(truth*)data = *(truth*)var_to_get;
}

*/

///////////////////////////////////////////////////////////////////////////////
// ObjectEditor

ObjectEditor::ObjectEditor()
:object_build_mode(0)
{
}

ObjectEditor::~ObjectEditor()
{
}

void ObjectEditor::RegisterTBVariables()
{
	//int bar_alpha = 255;
	//TwSetParam(_t, NULL, "alpha", TW_PARAM_INT32, 1, &bar_alpha);
	//TwAddVarCB(_t, "BuildMode", TW_TYPE_UINT8, SetBuildMode, GetBuildMode, &object_build_mode, " label='Build Mode' min=0 max=1");
}

void ObjectEditor::InitEditMode()
{
	//TwAddButton(_t, "CreateNewObject", CreateNewObject, 0, " label='Create New Object'");

	//TwAddSeparator(_t, "ObjectsSpacer", 0);
}

void ObjectEditor::ExitEditMode()
{
	//TwRemoveVar(_t, "CreateNewObject");

	//TwRemoveVar(_t, "ObjectsSpacer");
}

///////////////////////////////////////////////////////////////////////////////


void Editor::ExposeObjects()
{
	//for(unsigned i = 0; i < selected_objects.size(); ++i)
	//{
	//	selected_objects[i]->ExposeToTweakbar( object_editor.GetBar() );
	//}
}


void Editor::ClearExposedObjects()
{
	//for(unsigned i = 0; i < selected_objects.size(); ++i)
	//{
	//	selected_objects[i]->RemoveFromTweakbar( object_editor.GetBar() );
	//}
}

void Editor::SetObjectEditorMode(truth mode)
{
	if( mode == 1 )
		object_editor.InitEditMode();
	else // == 0
		object_editor.ExitEditMode();

	RefreshObjectEditor();
}

void Editor::RefreshObjectEditor()
{
	ClearExposedObjects();
	ExposeObjects();
}


void Editor::ObjectsMouseInput()
{
	if( Global::Mouse::GetState( Button::LEFT_DOWN ) )
	{
		in_selection = 1;
		selection_start = Global::Mouse::WorldPos;
		selection_end = Global::Mouse::WorldPos;

		ClearExposedObjects();
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
		ExposeObjects();
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

void Editor::ObjectsKeyboardInput()
{
	const unsigned char KEY_DELETE = Global::KEY::DEL;

	if( Global::Keyboard::KeyPressed( KEY_DELETE ) )
	{
		if( selected_objects.size() > 0 )
		{
			ClearExposedObjects();
			External::physics->DeletePickedBodies();
			selected_objects.clear();
		}
	}
}
