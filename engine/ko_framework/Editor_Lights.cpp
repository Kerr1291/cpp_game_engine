#include <Editor.h>

#include <BaseGame.h>
#include <RenderManager.h>

#include <Area.h>
#include <GridInfo.h>
#include <Light.h>
#include <Camera.h>

void Editor::SelectLightsInsideBox()
{
	vec2 box_size = selection_end - selection_start;
	vec2 box_pos = selection_start;

	selected_lights.clear();

	for( unsigned i = 0; i < External::renderer->lights.size(); ++i )
	{
		if( IsInside( External::renderer->lights[ i ]->position, box_pos, box_size ) )
		{
			selected_lights.push_back( External::renderer->lights[i] );
		}
	}
}

void Editor::DeleteSelectedLights()
{
	for(unsigned i = 0; i < selected_lights.size(); ++i)
	{
		truth found = 0;
		unsigned finder = 0;
		for( ; finder < External::renderer->lights.size(); ++finder )
		{
			if( selected_lights[i] == External::renderer->lights[ finder ] )
			{
				found = 1;
				break;
			}
		}

		if( found )
			External::renderer->lights.erase( External::renderer->lights.begin() + finder );

		delete selected_lights[i];
	}

	selected_lights.clear();
}


void Editor::MoveLightsWithMouse()
{
	float move_scale = 0.00333f * External::game->GetCurrentCamera()->GetWindowZoom();
	for(unsigned i = 0; i < selected_lights.size(); ++i)
	{
		selected_lights[i]->position += Global::Mouse::FrameDelta * move_scale;
	}
}

void Editor::LightsKeyboardInput()
{
	const unsigned char KEY_DELETE = Global::KEY::DEL;

	if( Global::Keyboard::KeyPressed( KEY_DELETE ) )
	{
		if( selected_lights.size() > 0 )
		{
			DeleteSelectedLights();
		}
	}
}

void Editor::LightsMouseInput()
{
	if( !External::game->GetCurrentArea() )
		return;

	vec2 paint_location = Global::Mouse::WorldPos;

	if( light_placing_enabled )
	{
		if( Global::Mouse::GetState( Button::LEFT_DOWN ) )
		{
			//create a light
			Light* light = new Light();
			light->color = vec4(0.5f + Global::GetRandomFloat(),0.5f + Global::GetRandomFloat(),0.5f + Global::GetRandomFloat(),1.0f);
			light->intensity = 0.15f; //Global::GetRandomFloat() * 100.0f;
			light->position = Global::Mouse::WorldPos;
			light->radius = 2000.0f;
			light->direction = vec2(0,0);// glm::normalize( vec2( Global::GetRandomFloat(), Global::GetRandomFloat() ) );

			/*
			REGISTER_RW( Light, light->color.x );
			REGISTER_RW( Light, light->color.y );
			REGISTER_RW( Light, light->color.z );
			REGISTER_RW( Light, light->position );
			REGISTER_RW( Light, light->intensity );
			REGISTER_RW( Light, light->radius );
			*/

			External::renderer->AddLight(light);//TODO: turn this light creation into a command, don't directly access the renderer
		}
	}
	else
	{
		if( Global::Mouse::GetState( Button::LEFT_DOWN ) )
		{
			in_selection = 1;
			selection_start = Global::Mouse::WorldPos;
			selection_end = Global::Mouse::WorldPos;
			selected_lights.clear();
		}

		if( Global::Mouse::GetState( Button::LEFT_HELD ) )
		{
			selection_end = Global::Mouse::WorldPos;
		}

		if( Global::Mouse::GetState( Button::LEFT_UP ) )
		{
			in_selection = 0;
			CreateSelectionBox(selection_start, selection_end);
			SelectLightsInsideBox();
		}
	}


	if( selected_lights.size() > 0 )
	{
		if( Global::Mouse::GetState( Button::RIGHT_HELD ) )
		{
			MoveLightsWithMouse();
		}
	}
	else
	{
		if( Global::Mouse::GetState( Button::RIGHT_HELD ) || Global::Mouse::GetState( Button::RIGHT_DOWN ) )
		{
			MoveCameraWithMouse();
		}
	}
}
