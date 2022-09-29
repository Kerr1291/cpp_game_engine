#include <CreateArea.h>
#include <BaseGame.h>
#include <Area.h>
#include <Physics.h>
#include <GridInfo.h>

CreateArea::CreateArea(const std::string& name, unsigned grid_size_x, unsigned grid_size_y)
:Command(CommandType::BLOCKING)
,area_name(name)
,size_x(grid_size_x)
,size_y(grid_size_y)
{}

CreateArea::~CreateArea()
{}

truth CreateArea::Execute()
{
	Global::SetDebugMode(0);
	External::game->UnloadCurrentArea();

	Area* new_area = new Area();

	External::game->SetArea( new_area );
	External::game->InitCurrentArea();

    if( area_name.empty() )
        area_name = "NewCustomArea";

    //set the name, size, and boundries
	new_area->_name = area_name;

	//default values just in case to prevent a zero sized level
	//TODO: post a warning about zero sized levels
	if( size_x == 0 )
		size_x = 10;
	if( size_y == 0 )
		size_y = 10;

    //create a new grid to place in the area
    GridInfo* grid = new GridInfo();

    //these are kindof magic numbers... TODO: rework so setcellsize is removed. size of 1 is all that we should ever use
	grid->SetCellSize( ivec2(1,1) );
	//the grid's position should never be anywhere else, this is basically the origin...
	grid->SetPosition( vec2(10,10) );

    //allocate memory for the tile cells and 'zero' it out
    unsigned short* cells = new unsigned short[size_x*size_y*4];
	memset(cells,255,size_x*size_y);

    //assign these cells to the grid (TODO: since the cell creation is sorta-constant now, internalize this inside a function inside the GridInfo class)
    grid->SetCells(cells, size_x, size_y);

    //assign a null atlas (wait for the user to assign one later)
    grid->SetAtlas( 0 );

    new_area->_grid = grid;

    //as a bonus, let's assume the grid's bounds are the level's edge and
    //we'll make some physics bounds around the edge
	b2vec2 bottom_left = b2vec2( 10.0f, 10.0f + 1.0f );
	b2vec2 top_right = b2vec2( 10.0f + size_x * 1.0f, 10.0f + 1.0f + size_y * 1.0f);

	std::vector< std::vector<b2vec2> > edges;

	std::vector<b2vec2> l_wall;
	l_wall.push_back( bottom_left );
	l_wall.push_back( b2vec2(bottom_left.x, top_right.y) );
	edges.push_back( l_wall );

	std::vector<b2vec2> r_wall;
	r_wall.push_back( b2vec2(top_right.x, bottom_left.y) );
	r_wall.push_back( top_right );
	edges.push_back( r_wall );

	std::vector<b2vec2> t_wall;
	t_wall.push_back( top_right );
	t_wall.push_back( b2vec2(bottom_left.x, top_right.y) );
	edges.push_back( t_wall );

	std::vector<b2vec2> b_wall;
	b_wall.push_back( bottom_left );
	b_wall.push_back( b2vec2(top_right.x, bottom_left.y) );
	edges.push_back( b_wall );

	External::physics->CreateLevelBounds( edges );

	complete = 1;
	return 1;
}
