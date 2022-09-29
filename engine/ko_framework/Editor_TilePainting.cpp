#include <Editor.h>

#include <BaseGame.h>
#include <RenderManager.h>

#include <Camera.h>
#include <BaseObject.h>
#include <Area.h>
#include <GridInfo.h>

//Note: This will get set to a valid type next frame if something invalid is passed in
void Editor::ForceSelectPaintingType(unsigned type)
{
	painting_type = type;
}

truth Editor::IsMouseOverPaintingGUI() const
{
	const unsigned size = 32;
	vec2 mouse = Global::Mouse::ScreenPos;

	unsigned num_textures = (Global::GameWindow->GetW() - size/2) / size;
	if( num_textures%2 == 0 )
		num_textures -= 1;

	if( mouse.x < size/2 || mouse.x > size * num_textures )
		return 0;
	if( mouse.y < size/2 || mouse.y > size * 2 )
		return 0;

	return 1;
}

unsigned short Editor::GetPaintingTypeFromGUI(const vec2& screen_pos) const
{
	const unsigned short size = 32;
	unsigned short result = (unsigned short)-1;

	unsigned num_textures = (Global::GameWindow->GetW() - size/2) / size;
	if( num_textures%2 == 0 )
		num_textures -= 1;
	unsigned half = num_textures/2;

	unsigned short offset = screen_pos.x;
	offset -= size/2;
	offset /= size;

	if( painting_type > half )
		result = (painting_type + offset) - half;
	else
		result = offset;

	return result;
}

void Editor::SelectPaintingTypeFromGUI()
{
	vec2 mouse = Global::Mouse::ScreenPos;
	painting_type = GetPaintingTypeFromGUI( mouse );
}

void Editor::DrawSelectedAtlasTextures()
{
	GridInfo* grid = 0;
	if( External::game->GetCurrentArea() )
		grid = External::game->GetCurrentArea()->GetGrid();

	if(!grid)
		return;

    //don't try to draw the atlas if there's not one loaded
    if(!grid->GetAtlas())
        return;

	const unsigned size = 32;
	const float size_x = (float)size;
	const float size_y = (float)size;
	unsigned num_textures = (Global::GameWindow->GetW() - size/2) / size;
	if( num_textures%2 == 0 )
		num_textures -= 1;
	gui_atlas_textures = std::vector<unsigned>( num_textures, CLEAR_TYPE );

	unsigned half = num_textures/2;
	for(unsigned i = 0; i < num_textures; ++i)
	{
		unsigned type = painting_type - half + i;

		if( type < 0 || type > GetNumTextures( *grid->atlas ) - 1)
			continue;

		gui_atlas_textures[i] = type;
	}

	//int tbpos[2] = {0,0};
	//TwGetParam(_t, NULL, "position", TW_PARAM_INT32, 2, tbpos);
	External::renderer->ShowAtlasTextures( vec2(size_x,size_y), vec2(size_x,size_y), gui_atlas_textures, grid->atlas, painting_type );
}

void Editor::FillInsideBox()
{
	if( !External::game->GetCurrentArea() )
		return;

	GridInfo* grid = External::game->GetCurrentArea()->GetGrid();

	if(!grid)
		return;

	CreateSelectionBox(selection_start, selection_end);

	vec2 distance = glm::abs(selection_end - selection_start);
	uvec2 num_locations = uvec2(distance.x + grid->cell_size.x, distance.y + grid->cell_size.y);
	num_locations.x /= grid->cell_size.x;
	num_locations.y /= grid->cell_size.y;
	for( unsigned i = 0; i < num_locations.x; ++i)
	{
		for( unsigned j = 0; j < num_locations.y; ++j)
		{
			vec2 world_pos = selection_start + vec2(i * grid->cell_size.x, j * grid->cell_size.y);
			uvec2 paint_location;
			grid->GetGridLocation( paint_location, world_pos );
			grid->UpdateCell_NoRefresh( painting_type, painting_layer, paint_location.x, paint_location.y );
		}
	}
	grid->RefreshCellTexture();
}



void Editor::PaintingKeyboardInput()
{
	const unsigned char KEY_DELETE = Global::KEY::DEL;

	if( !External::game->GetCurrentArea() )
		return;

	GridInfo* grid = External::game->GetCurrentArea()->GetGrid();

	if(!grid)
		return;

    //don't process painter input if there's no atlas loaded
    //and force the type to the clear type
    if(!grid->GetAtlas())
    {
		painting_type = CLEAR_TYPE;
        return;
    }

	if( Global::Keyboard::KeyPressed( KEY_DELETE ) )
	{
		painting_type = CLEAR_TYPE;
	}

	uvec2 paint_location;
	grid->GetGridLocation( paint_location, Global::Mouse::WorldPos );
}


void Editor::PaintingMouseInput()
{
	if( !External::game->GetCurrentArea() )
		return;

	GridInfo* grid = External::game->GetCurrentArea()->GetGrid();

    //don't paint if the grid isn't loaded
	if(!grid)
		return;

    //don't paint if there is no atlas loaded
    if(!grid->GetAtlas())
        return;

	uvec2 paint_location;
	grid->GetGridLocation( paint_location, Global::Mouse::WorldPos );

	if( IsMouseOverPaintingGUI() && in_selection == 0 )
	{
		if( Global::Mouse::GetState( Button::LEFT_DOWN ) )
		{
			SelectPaintingTypeFromGUI();
		}
		return;
	}

	if( fill_mode )
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

			FillInsideBox();
		}
	}
	else
	{
		if( Global::Mouse::GetState( Button::LEFT_HELD ) || Global::Mouse::GetState( Button::LEFT_DOWN ) )
		{
			grid->UpdateCell( painting_type, painting_layer, paint_location.x, paint_location.y );
		}
	}

	if( Global::Mouse::GetState( Button::RIGHT_HELD ) || Global::Mouse::GetState( Button::RIGHT_DOWN ) )
	{
		//grid->UpdateCell( CLEAR_TYPE, painting_layer, paint_location.x, paint_location.y );
		MoveCameraWithMouse();
	}

	if( Global::Mouse::WheelDelta != 0 )
	{
		//int modified_painting_layer = painting_layer + Global::Mouse::WheelDelta;
		//painting_layer = (unsigned char)glm::clamp<int>( modified_painting_layer, 0, 3 );
		int modified_painting_type = painting_type + Global::Mouse::WheelDelta;
		painting_type = glm::clamp<int>( modified_painting_type, 0, (int)(GetNumTextures( *grid->atlas ) - 1) );

		//TwRefreshBar(_t);
	}
}
