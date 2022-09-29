#include <TileMap.h>

TileMap::TileMap()
:_size(uvec2(0,0))
,_tile_texture(0)
,_tiles(0)
{
}

TileMap::TileMap(const TileMap& other)
:_size(other._size)
,_tile_texture(0)
,_tile_texture_map(other._tile_texture_map)
,_tiles(0)
{
	const unsigned k_tiles_size = _size.x * _size.y;
	_tiles = new unsigned short[ k_tiles_size ];
	memcpy( _tiles, other._tiles, k_tiles_size );
	
	GenTileTexture();
}

TileMap& TileMap::operator =(const TileMap& other)
{
	_size = other._size;
	_tile_texture = 0;
	_tile_texture_map = other._tile_texture_map;
	_tiles = 0;

	const unsigned k_tiles_size = _size.x * _size.y;
	_tiles = new unsigned short[ k_tiles_size ];
	memcpy( _tiles, other._tiles, k_tiles_size );

	GenTileTexture();
}

TileMap::~TileMap()
{
	UnloadMap();
}

void TileMap::UnloadMap()
{
	_size = uvec2(1,1);

	delete[] _tiles;
	_tiles = 0;

	glDeleteTextures(1, &_tile_texture);
}

bool TileMap::IsInMap(float world_x, float world_y) const
{
	return IsInMap( vec2(world_x, world_y) );
}

bool TileMap::IsInMap(const vec2& world_pos) const
{
	const float l = 0;
	const float r = static_cast<float>(_size.x);
	const float t = static_cast<float>(_size.y);
	const float b = 0;

	if( world_pos.x < l )
		return false;
	if( world_pos.x > r )
		return false;
	if( world_pos.y < b )
		return false;
	if( world_pos.y > t )
		return false;

	return true;
}

void TileMap::WorldToMap(uvec2& out_vec, float world_x, float world_y) const
{
	WorldToMap( out_vec, vec2( world_x, world_y ) );
}

//maps a floating point world position to
//an integer map position. clamps if outside map
void TileMap::WorldToMap(uvec2& out_vec, const vec2& world_pos) const
{
	if( _tiles == 0 )
		return;

	if( _size.x < 2 || _size.y < 2 )
		return;

	unsigned size_x =_size.x-1;
	unsigned size_y =_size.y-1;

	uvec2 clamped = glm::clamp( uvec2(world_pos.x,world_pos.y), 
							    uvec2(0,0), 
							    uvec2(size_x,size_y) );

	out_vec = clamped;
}


uvec2 TileMap::GetMapSize() const
{
	return _size;
}

//copy w*h data from tiles
void TileMap::UploadTileMapData(unsigned short* tiles, unsigned w, unsigned h)
{
    if( !tiles )
        return;

	//no silly dimensions on w or h
	if( w < 2 || w < 2 )
		return;

	if( _tiles )
		delete[] _tiles;

	const size_t k_tiles_size = w*h;

	_tiles = new unsigned short[k_tiles_size];
	memcpy( _tiles, tiles, k_tiles_size );

	_size.x = w;
	_size.y = h;

	//texture_position_offset = vec2( (float)(cell_size.x*grid_size.x)*0.5f,(float)(cell_size.y*grid_size.y)*0.5f + cell_size.y );

	//gen the texture
    GenTileTexture();
}

//change one tile in the map, automatically regenerates the map texture
void TileMap::UpdateTileID(unsigned short data, unsigned char layer, unsigned x, unsigned y)
{
	if( _tiles == 0 )
		return;

	//since re-generating the texture is costly, let's only do it if the data is actually new
	if( _tiles[ GetTileIndex(layer, x, y) ] != data )
	{
		_tiles[ GetTileIndex(layer, x, y) ] = data;

		//since something was modified, regenerate the texture
		GenTileTexture();
	}
}

//change one tile in the map, automatically regenerates the map texture
void TileMap::UpdateTileID(unsigned short data, unsigned char layer, uvec2 map_pos)
{
	UpdateTileID(data,layer,map_pos.x,map_pos.y);
}

//use to change several tiles at once, remember to call RefreshMapTexture
void TileMap::UpdateTileID_NoRefresh(unsigned short data, unsigned char layer, unsigned x, unsigned y)
{
	if( _tiles == 0 )
		return;

	_tiles[ GetTileIndex(layer, x, y) ] = data;
	

	//if( cells[(y * (unsigned)grid_size.x + x)*4 + layer] != data )
	//{
	//	cells[(y * (unsigned)grid_size.x + x)*4 + layer] = data;
	//}
}

//use to change several tiles at once, remember to call RefreshMapTexture
void TileMap::UpdateTileID_NoRefresh(unsigned short data, unsigned char layer, uvec2 map_pos)
{
	UpdateTileID_NoRefresh(data,layer,map_pos.x,map_pos.y);
}

void TileMap::RefreshMapTexture()
{
	ReGenTileTexture();
}


//only layers from 0 to 3 are valid
/* //old method, keep function here for now
unsigned GridInfo::GetCell(unsigned char layer, unsigned x, unsigned y)
{
	if( !_cells || (layer > 3) )
		return (unsigned)(-1);

	return cells[(y * (unsigned)grid_size.x + x)*4 + layer];
}
*/


//only layers from 0 to 3 are valid
void TileMap::GetTileID(unsigned& out, unsigned char layer, unsigned x, unsigned y) const
{
	if( _tiles == 0 )
		return;

	//out = cells[(y * (unsigned)_size.x + x)*4 + layer];
	out = _tiles[ GetTileIndex(layer, x, y) ];
}

//only layers from 0 to 3 are valid
void TileMap::GetTileID(unsigned& out, unsigned char layer, uvec2 map_pos) const
{
	GetTileID(out, layer, map_pos.x, map_pos.y);
}

unsigned TileMap::GetTileIndex(unsigned layer, unsigned x, unsigned y) const
{
	if( (layer > 3) || (x >= _size.x) || (y >= _size.y) )
		return 0;

	return ((y * (unsigned)_size.x + x)*4 + layer);
}

void TileMap::ReGenTileTexture()
{
    if( _tile_texture == 0 || _tiles == 0 )
        return;

    const unsigned w = _size.x;
    const unsigned h = _size.y;

    glBindTexture(GL_TEXTURE_2D, _tile_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, _tiles);
    glBindTexture( GL_TEXTURE_2D, 0 );
}

void TileMap::GenTileTexture()
{
	if( _tiles == 0 )
		return;

    if( _tile_texture == 0 )
        glGenTextures(1, &_tile_texture);

    const unsigned w = _size.x;
    const unsigned h = _size.y;

    glBindTexture(GL_TEXTURE_2D, _tile_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI, w, h, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, _tiles); //works!
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI_EXT, w, h, 0, GL_RGBA_INTEGER_EXT, GL_UNSIGNED_SHORT, cells); //also works

    glBindTexture( GL_TEXTURE_2D, 0 );
}

bool TileMap::LoadTextureMappingAtlas(const std::string& filename)
{
	if(! _tile_texture_map.Load(filename) )
		return false;

	if( _size.x < 2 || _size.y < 2 || _tiles == 0 )
		return true;

	GenTileTexture();

	return true;
}

const TextureAtlas& TileMap::GetTextureMappingAtlas() const
{
	return _tile_texture_map;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
