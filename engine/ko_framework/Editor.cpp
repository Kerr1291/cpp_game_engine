#include <Editor.h>

#include <BaseGame.h>
#include <Physics.h>
#include <RenderManager.h>

#include <GUIManager.h>
#include <GUIWindow.h>
#include <GUIField.h>
#include <GUIText.h>
#include <GUIBuilder.h>

#include <Editor_GUIBuilder.h>

#include <Camera.h>
#include <BaseObject.h>
#include <Transform.h>
#include <PhysicsObject.h>
#include <Area.h>
#include <GridInfo.h>
#include <Light.h>

#include <DoNothing.h>
#include <LoadArea.h>

//////////////////////////////////////////////
// Tweakbar functions
/*
void TW_CALL SetAreaName(const void* data, void* null)
{
	External::editor->SetCurrentAreaName( *(const std::string*)data );
}

void TW_CALL GetAreaName(void* data, void* null)
{
	Area* a = External::game->GetCurrentArea();
	if( a )
	{
		TwCopyStdStringToLibrary( *(std::string*)data, a->GetName() );
	}
}

void TW_CALL GetAreaGridSize(void* data, void* null)
{
	vec2* s = (vec2*)data;

	vec2 size = vec2(0,0);

	External::editor->GetCurrentAreaGridSize( size );

	*s = size;
}

void TW_CALL DeleteSelectedObjects(void* null)
{
	//delete all the bodies that were picked (if the bodies have BaseObjects attached, those are also deleted)
	External::physics->DeletePickedBodies();
}

void TW_CALL MergeSelectedPhysicsBodies(void* null)
{
	//merges all static bodies that were selected
	External::physics->MergePickedBodies();
}

void TW_CALL ClearPaintingType(void* painting_type)
{
	unsigned short* type = (unsigned short*)painting_type;
	*type = CLEAR_TYPE;
}

void TW_CALL TogglePaintingMode(void* null)
{
	if( External::editor->GetMode() == EditorMode::PAINT )
		External::editor->SetMode( EditorMode::SELECTION );
	else
		External::editor->SetMode( EditorMode::PAINT );
}


void TW_CALL TogglePhysicsMode(void* null)
{
	if( External::editor->GetMode() == EditorMode::PHYSICS )
		External::editor->SetMode( EditorMode::SELECTION );
	else
		External::editor->SetMode( EditorMode::PHYSICS );
}

void TW_CALL ToggleLightsMode(void* null)
{
	if( External::editor->GetMode() == EditorMode::LIGHTS )
		External::editor->SetMode( EditorMode::SELECTION );
	else
		External::editor->SetMode( EditorMode::LIGHTS );
}

void TW_CALL ToggleObjectsMode(void* null)
{
	if( External::editor->GetMode() == EditorMode::OBJECTS )
		External::editor->SetMode( EditorMode::SELECTION );
	else
		External::editor->SetMode( EditorMode::OBJECTS );
}

void TW_CALL GetEditorModeName(void* data, void* null)
{
	unsigned mode = External::editor->GetMode();

	std::string mode_name;
	switch( mode )
	{
		case EditorMode::SELECTION:
			mode_name = "Selection";
			break;
		case EditorMode::PAINT:
			mode_name = "Painting";
			break;
		case EditorMode::PHYSICS:
			mode_name = "Physics";
			break;
		case EditorMode::LIGHTS:
			mode_name = "Lights";
			break;
		case EditorMode::OBJECTS:
			mode_name = "Objects";
			break;
		default:
			mode_name = "Unknown Editor Mode";
			break;
	};

	TwCopyStdStringToLibrary( *(std::string*)data, mode_name );
}


void TW_CALL GetPhysicsBodyTypeName(void* data, void* input)
{
	unsigned type = *(const unsigned*)input;

	std::string type_name;
	switch( type )
	{
		case 0:
			type_name = "None";
			break;
		case 1:
			type_name = "Box";
			break;
		case 2:
			type_name = "Circle";
			break;
		default:
			type_name = "Unknown Type";
			break;
	};

	TwCopyStdStringToLibrary( *(std::string*)data, type_name );
}

void TW_CALL GetAmbientColor(void* data, void* null)
{
	float* p = (float*)data;
	p[0] = External::renderer->GetAmbientLight().r;
	p[1] = External::renderer->GetAmbientLight().g;
	p[2] = External::renderer->GetAmbientLight().b;
}

void TW_CALL SetAmbientColor(const void* data, void* null)
{
	const float* p = (const float*)data;
	External::renderer->SetAmbientLight( vec3( p[0], p[1], p[2] ) );
}



*/
//////////////////////////////////////////////

Editor::Editor()
:previous_gui_focus("")
,mode( EditorMode::SELECTION )
,disabled(1)
,in_selection(0)
,painting_type( CLEAR_TYPE )
,painting_layer(0)
,fill_mode(0)
,physics_type(0)
,physics_edge_mode(0)
,physics_edge_point_num(0)
,physics_edge_start(vec2(0,0))
,physics_edge_end(vec2(0,0))
,snap_to_grid(0)
,light_placing_enabled(0)
{
	selected_bodies = 0;
	External::editor = this;
}
Editor::~Editor()
{
}

void Editor::RegisterTBVariables()
{
	/*
	int bar_alpha = 255;
	TwSetParam(_t, NULL, "alpha", TW_PARAM_INT32, 1, &bar_alpha);
	TwAddVarCB(_t, "DisplayEditorMode", TW_TYPE_STDSTRING, 0, GetEditorModeName, 0, " label='Editor Mode'");

	//Area Loading
	//TwAddButton(_t, "CreateNewArea", CreateNewArea, 0, " group='Area Options' label='Create New Area'");
	TwAddVarCB(_t, "AreaName", TW_TYPE_STDSTRING, SetAreaName, GetAreaName, 0, " group='Area Options' label='Area Name'");
	TwAddVarCB(_t, "AreaGridSize", TW_TYPE_VEC2, 0, GetAreaGridSize, 0, " group='Area Options' label='Area Grid Size'");
	//TwAddVarRW(_t, "NewGridWidth", TW_TYPE_UINT32, &NEWGRID_SIZE_X, " group='Area Options' label='New Grid Width'");
	//TwAddVarRW(_t, "NewGridHeight", TW_TYPE_UINT32, &NEWGRID_SIZE_Y, " group='Area Options' label='New Grid Height'");
	//TwAddButton(_t, "MakeNewGrid", CreateNewAreaGrid, 0, " group='Area Options' label='Create New Grid'");
	//TwAddVarRW(_t, "NewGridAtlasFile", TW_TYPE_STDSTRING, &NEWATLAS_FILENAME, " group='Area Options' label='New Grid Atlas File'" );
	//TwAddButton(_t, "LoadNewGridAtlasFile", SetGridAtlas, 0, " group='Area Options' label='Set Grid Atlas'");


	//General Settings
	TwAddSeparator(_t, "space0", 0);
	TwAddVarRW(_t, "disabled", TW_TYPE_UINT8, &disabled, " group='General Settings' label='Disabled' min=0 max=1");
	TwAddVarRW(_t, "FillMode", TW_TYPE_UINT8, &fill_mode, " group='General Settings' label='Fill Mode' min=0 max=1");
	TwAddButton(_t, "DeleteSelectedObjects", DeleteSelectedObjects, 0, " label='Delete Selected Objects' ");
	TwAddButton(_t, "MergeSelectedPhysicsBodies", MergeSelectedPhysicsBodies, 0, " label='Merge Selected Static Bodies' ");


	//Tile Painting
	TwAddSeparator(_t, "space1", 0);
	TwAddButton(_t, "TogglePainting", TogglePaintingMode, 0, " label='Toggle Painting'");
	TwAddVarRW(_t, "PaintingType", TW_TYPE_UINT16, &painting_type, " group='Painting Options' label='Painting Type' min=0");
	TwAddVarRW(_t, "PaintingLayer", TW_TYPE_UINT8, &painting_layer, " group='Painting Options' label='Painting Layer' min=0 max=3");
	TwAddButton(_t, "ClearPainting", ClearPaintingType, &painting_type, " group='Painting Options' label='Clear Painting Type'");


	//Physics Editing
	//std::stringstream max_body_types;
	//max_body_types << (((int)MAX_COLLISION_BODY_TYPES)-1);


	TwAddSeparator(_t, "space2", 0);
	TwAddButton(_t, "TogglePhysics", TogglePhysicsMode, 0, " label='Toggle Physics'");
	TwAddVarRW(_t, "ToggleSnapToGrid", TW_TYPE_UINT8, &snap_to_grid, " group='Physics Options' label='Snap To Grid' min=0 max=1");
	TwAddVarRW(_t, "ToggleEdgeDrawing", TW_TYPE_UINT8, &physics_edge_mode, " group='Physics Options' label='Create Edges' min=0 max=1");
	//TwAddVarRW(_t, "PhysicsType", TW_TYPE_UINT32, &physics_type, (" group='Physics Options' label='Body Type' min=0 max="+max_body_types.str()).c_str() );
	//TwAddVarCB(_t, "PhysicsTypeName", TW_TYPE_STDSTRING, 0, GetPhysicsBodyTypeName, &physics_type, " group='Physics Options' label='Body Type Name'");


	//Light Editing
	TwAddSeparator(_t, "space3", 0);
	TwAddButton(_t, "ToggleLights", ToggleLightsMode, 0, " label='Toggle Light Editing'");
	TwAddVarRW(_t, "PlacingLights", TW_TYPE_UINT8, &light_placing_enabled, " group='Light Options' label='Placing Lights?' min=0 max=1");
	TwAddVarCB(_t, "AmbientLight", TW_TYPE_COLOR3F, SetAmbientColor, GetAmbientColor, 0, " group='Light Options' colormode='hls' label='Ambient Light Color'");


	//Object Editing
	object_editor.RegisterTBVariables();
	TwAddSeparator(_t, "space4", 0);
	TwAddButton(_t, "ToggleObjects", ToggleObjectsMode, 0, " label='Toggle Object Editing'");
	*/
}

void Editor::Init()
{
	RegisterTBVariables();

	GUI::EditorGUIBuilder::InitBuilder(this);
	//GUI::EditorGUIBuilder::CreateMainWindow();

}


void Editor::SetMode(unsigned new_mode)
{
	if( new_mode > EditorMode::NUM_MODES )
		return;
	mode = new_mode;
}

void Editor::UpdateEditorMode()
{
	int result = GUI::EditorGUIBuilder::GetActiveWindowModeType();
	if( result < 0 )
		return;

	SetMode( static_cast<unsigned>( result ) );
}


void Editor::MoveCameraWithMouse()
{
	Camera* c = External::game->GetCurrentCamera();
	if( c )
	{
		float move_scale = 0.00333f * c->GetWindowZoom();
		c->MoveWindowPos( Global::Mouse::FrameDelta.x * move_scale, Global::Mouse::FrameDelta.y * move_scale );
	}
}


void Editor::ProcessKeyboardInput()
{
    if( External::gui->GetFocus() )
        return;

	const unsigned char ESCAPE = Global::KEY::ESC;

	if( Global::Keyboard::KeyPressed( ESCAPE ) )
	{
		if( physics_edge_point_num == 1 )
		{
			physics_edge_point_num = 0;
		}
		else
		{
			if( External::physics->debugMode == 1 )
				External::physics->SetDebugMode(0);

			mode = EditorMode::SELECTION;
			ClearSelections();
		}
	}

	switch( mode )
	{
		case EditorMode::SELECTION:
			SelectionKeyboardInput();
		break;
		case EditorMode::PAINT:
			PaintingKeyboardInput();
		break;
		case EditorMode::PHYSICS:
			PhysicsKeyboardInput();
		break;
		case EditorMode::LIGHTS:
			LightsKeyboardInput();
		break;
		case EditorMode::OBJECTS:
			ObjectsKeyboardInput();
		break;
		default:
		break;
	};
}


void Editor::ProcessMouseInput()
{
    if( External::gui->GetFocus() )
        return;

	switch( mode )
	{
		case EditorMode::SELECTION:
			SelectionMouseInput();
		break;
		case EditorMode::PAINT:
			PaintingMouseInput();
		break;
		case EditorMode::PHYSICS:
			PhysicsMouseInput();
		break;
		case EditorMode::LIGHTS:
			LightsMouseInput();
		break;
		case EditorMode::OBJECTS:
			ObjectsMouseInput();
		break;
		default:
		break;
	};
}


void Editor::Update(double fps)
{
	if( External::gui->GetFocus() )
	{
		if( previous_gui_focus != External::gui->GetFocus()->GetFullName() )
			UpdateEditorMode();

		previous_gui_focus = External::gui->GetFocus()->GetFullName();
	}

#if DEBUG_BUILD == 1
	GUI::EditorGUIBuilder::Hotkey_Check();
#endif

	if(disabled)
		return;

	/*
	int tbiconified = 0;
	int tbvisible = 0;
	int tbpos[2] = {0,0};
	int tbsize[2] = {0,0};
	TwGetParam(_t, NULL, "iconified", TW_PARAM_INT32, 1, &tbiconified);
	TwGetParam(_t, NULL, "visible", TW_PARAM_INT32, 1, &tbvisible);
	TwGetParam(_t, NULL, "position", TW_PARAM_INT32, 2, tbpos);
	TwGetParam(_t, NULL, "size", TW_PARAM_INT32, 2, tbsize);

	int tb_global_iconified = 0;
	int tb_global_visible = 0;
	int tb_global_pos[2] = {0,0};
	int tb_global_size[2] = {0,0};
	TwGetParam(Debug::globalBar, NULL, "iconified", TW_PARAM_INT32, 1, &tb_global_iconified);
	TwGetParam(Debug::globalBar, NULL, "visible", TW_PARAM_INT32, 1, &tb_global_visible);
	TwGetParam(Debug::globalBar, NULL, "position", TW_PARAM_INT32, 2, tb_global_pos);
	TwGetParam(Debug::globalBar, NULL, "size", TW_PARAM_INT32, 2, tb_global_size);


	int tb_object_editor_iconified = 0;
	int tb_object_editor_visible = 0;
	int tb_object_editor_pos[2] = {0,0};
	int tb_object_editor_size[2] = {0,0};
	TwGetParam(object_editor.GetBar(), NULL, "iconified", TW_PARAM_INT32, 1, &tb_object_editor_iconified);
	TwGetParam(object_editor.GetBar(), NULL, "visible", TW_PARAM_INT32, 1, &tb_object_editor_visible);
	TwGetParam(object_editor.GetBar(), NULL, "position", TW_PARAM_INT32, 2, tb_object_editor_pos);
	TwGetParam(object_editor.GetBar(), NULL, "size", TW_PARAM_INT32, 2, tb_object_editor_size);
	*/


	const vec2& mpos = Global::Mouse::ScreenPos;

	GridInfo* grid = 0;
	if( External::game->GetCurrentArea() )
		grid = External::game->GetCurrentArea()->GetGrid();

	if( selected_objects.size() > 0 )
	{
		for(unsigned i = 0; i < selected_objects.size(); ++i)
		{
			if( selected_objects[i]->GetPhysicsObject() )
			{
				selected_objects[i]->GetPhysicsObject()->SetAngularVelocity(0);
				selected_objects[i]->GetPhysicsObject()->SetLinearVelocity(vec2(0,0));
			}
		}
	}

	//clamp painting type when we have a loaded grid AND a loaded atlas
	if( grid && grid->GetAtlas() && painting_type != CLEAR_TYPE )
		painting_type = glm::clamp( (int)painting_type, 0, (int)(GetNumTextures( *grid->atlas ) - 1) );

    //clamp painting layer, always. (grids always have 4 layers)
	painting_layer = glm::clamp<unsigned char>( painting_layer, 0, 3 );

	/*

	//do not process updates furthur if the bar is hidden or the mouse is over the bar
	if( tbvisible && tbiconified == 0 && in_selection == 0 )
	{
		if( IsInside(mpos, vec2(tbpos[0],tbpos[1]), vec2(tbsize[0],tbsize[1])) )
			return;
	}

	if( tb_global_visible && tb_global_iconified == 0 && in_selection == 0 )
	{
		if( IsInside(mpos, vec2(tb_global_pos[0],tb_global_pos[1]), vec2(tb_global_size[0],tb_global_size[1])) )
			return;
	}

	if( tb_object_editor_visible && tb_object_editor_iconified == 0 && in_selection == 0 )
	{
		if( IsInside(mpos, vec2(tb_object_editor_pos[0],tb_object_editor_pos[1]), vec2(tb_object_editor_size[0],tb_object_editor_size[1])) )
			return;
	}

	*/

	//Don't update the editor's state if we're over the GUI
	if( External::gui->MouseOverGUI() )
		return;

	ProcessKeyboardInput();
	ProcessMouseInput();
}



void Editor::Draw()
{
	if(disabled)
		return;

	GridInfo* grid = 0;
	if( External::game->GetCurrentArea() )
		grid = External::game->GetCurrentArea()->GetGrid();

	//draw selection box
	if( in_selection )
	{
		glEnable(GL_BLEND);
		glColor4fv( glm::value_ptr( vec4(0.5f) ) );
		glBegin(GL_QUADS);

		glVertex2f( selection_start.x, selection_start.y );
		glVertex2f( selection_start.x, selection_end.y );
		glVertex2f( selection_end.x, selection_end.y );
		glVertex2f( selection_end.x, selection_start.y );

		glEnd();
		glDisable(GL_BLEND);
	}

	if( mode == EditorMode::PHYSICS && physics_edge_mode )
	{
		//if the first point hasn't been placed yet, draw a point under the mouse
		if( physics_edge_point_num == 0 )
		{
			vec2 draw_point = Global::Mouse::WorldPos;
			if( snap_to_grid )
				SnapToGrid(grid, draw_point);

			glPointSize(6);
			glDisable(GL_BLEND);
			glBegin(GL_POINTS);
			glColor3f(0,1,0);
				glVertex2f(draw_point.x, draw_point.y);
			glEnd();
			glPointSize(1);
		}
		else//if the first point has been placed, draw a line to it and draw both points
		{
			vec2 draw_point_a = physics_edge_start;
			vec2 draw_point_b = Global::Mouse::WorldPos;
			if( snap_to_grid )
				SnapToGrid(grid, draw_point_b);

			glColor4f(1,0,0,1.0f);
			glLineWidth(4.0f);
			glBegin(GL_LINES);
				glVertex2f(draw_point_a.x, draw_point_a.y);
				glVertex2f(draw_point_b.x, draw_point_b.y);
			glEnd();
			glLineWidth(1.0f);

			glPointSize(6);
			glDisable(GL_BLEND);
			glBegin(GL_POINTS);
			glColor3f(0,1,0);
				glVertex2f(draw_point_a.x, draw_point_a.y);
				glVertex2f(draw_point_b.x, draw_point_b.y);
			glEnd();
			glPointSize(1);
		}
	}

	//draw picked bodies
	if( selected_bodies )
	{
		glColor4f(1,0,0,1.0f);
		//glEnable(GL_BLEND);
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		for(unsigned i = 0; i < selected_bodies->size(); ++i)
		{
			b2Body* b = (*selected_bodies)[i];

			b2Fixture* fixture = b->GetFixtureList();
			for(; fixture; fixture = fixture->GetNext())
			{
				const b2AABB aabb = fixture->GetAABB(0);
				glVertex2f(aabb.lowerBound.x, aabb.lowerBound.y);
				glVertex2f(aabb.lowerBound.x, aabb.upperBound.y);

				glVertex2f(aabb.upperBound.x, aabb.upperBound.y);
				glVertex2f(aabb.upperBound.x, aabb.lowerBound.y);

				glVertex2f(aabb.lowerBound.x, aabb.upperBound.y);
				glVertex2f(aabb.upperBound.x, aabb.upperBound.y);

				glVertex2f(aabb.lowerBound.x, aabb.lowerBound.y);
				glVertex2f(aabb.upperBound.x, aabb.lowerBound.y);
			}
		}
		glEnd();
		glLineWidth(1.0f);
		//glDisable(GL_BLEND);
	}

	//draw points on the selected objects
	if( selected_objects.size() > 0 )
	{
		glPointSize(6);
		glDisable(GL_BLEND);
		glBegin(GL_POINTS);
		glColor3f(1,1,1);
		for(unsigned i = 0; i < selected_objects.size(); ++i)
		{
			const vec2 p = selected_objects[i]->t.loc;
			glVertex2f(p.x, p.y);
		}
		glEnd();
		glPointSize(1);
	}

	//draw lines around selected lights
	if( selected_lights.size() > 0 )
	{
		const float zoom_factor = glm::max( External::game->GetCurrentCamera()->GetWindowZoom(), 0.001f ) * 33.33f;
		for( unsigned j = 0; j < selected_lights.size(); ++j )
		{
			const float32 k_segments = 16.0f;
			const float32 k_increment = 2.0f * b2_pi / k_segments;
			float32 theta = 0.0f;
			glColor3f(selected_lights[j]->color.r * 1.2f, selected_lights[j]->color.g * 1.2f, selected_lights[j]->color.b * 1.2f);
			glBegin(GL_LINE_LOOP);

			for (int32 i = 0; i < k_segments; ++i)
			{
				vec2 v = selected_lights[j]->position + (selected_lights[j]->radius / zoom_factor) * vec2(cosf(theta), sinf(theta));
				glVertex2f(v.x, v.y);
				theta += k_increment;
			}
			glEnd();
		}
	}

	//draw editor UI thingss
	if( grid && mode == EditorMode::PAINT )
	{
		DrawSelectedAtlasTextures();
	}
}

GridInfo* Editor::GetCurrentAreaGrid()
{
	if( External::game->GetCurrentArea() && External::game->GetCurrentArea()->GetGrid() )
	{
		return External::game->GetCurrentArea()->GetGrid();
	}
	return 0;
}

void Editor::GetCurrentAreaGridSize( vec2& out_size )
{
	if( External::game->GetCurrentArea() && External::game->GetCurrentArea()->GetGrid() )
	{
		out_size = vec2( External::game->GetCurrentArea()->GetGrid()->GetGridSize() );
	}
}

void Editor::SetCurrentAreaGrid( GridInfo* in_grid )
{
	if( !External::game->GetCurrentArea() || !in_grid )
		return;

	Area* c = External::game->GetCurrentArea();

	if( c->_grid )
	{
		delete c->_grid;
	}

	c->_grid = in_grid;
}

void Editor::SetCurrentAreaName( const std::string& name )
{
	Area* a = 0;
	a = External::game->GetCurrentArea();
	if( a )
		a->_name = name;
}

void Editor::ToggleDisabled()
{
    disabled = disabled ? 0 : 1;
}

void Editor::ToggleFillMode()
{
    fill_mode = fill_mode ? 0 : 1;
}
