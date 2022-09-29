#include <Editor.h>

#include <BaseGame.h>
#include <Physics.h>

#include <BaseObject.h>
#include <Transform.h>
#include <PhysicsObject.h>
#include <Area.h>
#include <GridInfo.h>



void Editor::FillPhysicsInsideBox()
{
	//TODO: rewrite this function to just generate a large static box

	if( !External::game->GetCurrentArea() )
		return;

	GridInfo* grid = External::game->GetCurrentArea()->GetGrid();

	if(!grid)
		return;

	CreateSelectionBox(selection_start, selection_end);

	//TODO: Make a box from selection_start to selection_end
	/*
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
//			External::physics->CreateBodyOnGrid( physics_type, paint_location.x, paint_location.y, world_pos );
		}
	}
	*/
}

void Editor::CreatePhysicsEdge()
{
	std::vector< std::vector<b2vec2> > edges;
	std::vector<b2vec2> edge;
	edge.push_back( Convert::v2(physics_edge_start).to_box2d );
	edge.push_back( Convert::v2(physics_edge_end).to_box2d );
	edges.push_back( edge );

	External::physics->CreateLevelBounds( edges );
}

void Editor::PhysicsKeyboardInput()
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
	}
}


void Editor::SnapToGrid(GridInfo* grid, vec2& point_to_snap)
{
	if( grid )
	{
	    const vec2 half = vec2(grid->GetCellSize().x * 0.5f, grid->GetCellSize().y * 0.5f);
		uvec2 snap_location;
		grid->GetGridLocation( snap_location, point_to_snap + half );
		point_to_snap = grid->GetMapPosition( snap_location.x, snap_location.y );
	}
}

void Editor::PhysicsMouseInput()
{
	if( !External::game->GetCurrentArea() )
		return;

	GridInfo* grid = External::game->GetCurrentArea()->GetGrid();

	//turn physics debug mode on if physics stuff is being done
	if( External::physics->debugMode == 0 )
		External::physics->SetDebugMode(1);

	uvec2 paint_location = uvec2(0,0);

	if( grid )
		grid->GetGridLocation( paint_location, Global::Mouse::WorldPos );

	if( grid && fill_mode )
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

			FillPhysicsInsideBox();
		}
	}
	else
	{
		if( physics_edge_mode )
		{
			if( Global::Mouse::GetState( Button::LEFT_DOWN ) )
			{
				if( physics_edge_point_num == 0 )
				{
					physics_edge_start = Global::Mouse::WorldPos;

					if( grid && snap_to_grid )
						SnapToGrid(grid, physics_edge_start);

					physics_edge_end = Global::Mouse::WorldPos;

					physics_edge_point_num = 1;
				}
				else//physics_edge_point_num == 1
				{
					physics_edge_end = Global::Mouse::WorldPos;

					if( grid && snap_to_grid )
						SnapToGrid(grid, physics_edge_end);

					CreatePhysicsEdge();

					physics_edge_point_num = 0;
				}
			}
		}
		else
		{
			if( grid && ( Global::Mouse::GetState( Button::LEFT_HELD ) || Global::Mouse::GetState( Button::LEFT_DOWN ) ) )
			{
				//TODO: replace physics "box" painting with different functionality
//				External::physics->CreateBodyOnGrid( physics_type, paint_location.x, paint_location.y );
			}
		}
	}


	if( Global::Mouse::GetState( Button::RIGHT_HELD ) || Global::Mouse::GetState( Button::RIGHT_DOWN ) )
	{
		MoveCameraWithMouse();
	}
}
