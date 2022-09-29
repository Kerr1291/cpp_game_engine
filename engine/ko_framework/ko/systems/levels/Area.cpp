#include <Area.h>
#include <GameObject.h>
#include <TileMap.h>


Area::Area()
:_name("New Area")
{}

/* //TODO: Maybe re-implement? Don't really want areas being copied for now...
Area::Area(const Area& other)
:_name(other._name)
,_tile_map(other._tile_map)
,_game_objects(other._game_objects)
,_delete_list(other._delete_list)
{
	Area& non_const_other = const_cast<Area&>( other );
	non_const_other._game_objects.clear();
	non_const_other._delete_list.clear();
}
		
Area& Area::operator =(const Area& other)
{
	_name		  = other._name;
	_tile_map	  = other._tile_map;
	_game_objects = other._game_objects;
	_delete_list  = other._delete_list;

	Area& non_const_other = const_cast<Area&>( other );
	non_const_other._game_objects.clear();
	non_const_other._delete_list.clear();

	return *this;
}
*/

Area::~Area()
{
    RemoveAllObjects();
}
		
std::string Area::GetName() const
{
	return _name;
}


void Area::Load(const std::string& filename)
{
	//TODO: load an area from a file
}

void Area::Init()
{
	//TODO: call init all objects in an area (this will initialize all the components on an object)
}

void Area::ClearDeleteList()
{
	std::set<GameObject*>::iterator iter = _delete_list.begin();
	while( iter != _delete_list.end() )
	{
		delete *iter;
		++iter;
	}
	_delete_list.clear();
}
		
void Area::Unload()
{
	RemoveAllObjects();
	_tile_map.UnloadMap();
}

const TileMap& Area::GetTileMap() const
{
	return _tile_map;
}

void Area::AddObject( GameObject* obj )
{
	if(!obj)
		return;

	_game_objects.insert( obj );
}

void Area::RemoveObject( GameObject* obj )
{
	if(!obj)
		return;

	std::set<GameObject*>::iterator finder = _game_objects.find( obj );
	if( finder == _game_objects.end() )
		return;

	_delete_list.insert( *finder );
	_game_objects.erase( finder );
}

void Area::ForceRemoveObject( GameObject* obj )
{
	if(!obj)
		return;
	
	std::set<GameObject*>::iterator finder = _game_objects.find( obj );
	if( finder == _game_objects.end() )
		return;

	_game_objects.erase( finder );
	delete obj;
}

void Area::RemoveAllObjects()
{
	std::set<GameObject*>::iterator iter = _game_objects.begin();
	while( iter != _game_objects.end() )
	{
		_delete_list.insert( *iter );
		++iter;
	}

	_game_objects.clear();
	_delete_list.clear();
}

