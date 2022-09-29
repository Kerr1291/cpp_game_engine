///////////////////////////////
///////////////////////////////

//basic rundown of how things work

// -- In TextureAtlas -- 
// create atlas texture
// create atlas map
// read map data
// pack map data into a texture
//
// -- In TileMap -- 
// create TileMap(s) 					 
// create TileMap textures. will pack 4 'tile maps' into 1 texture 
//
// -- In RenderManager -- 
// render using map texture + atlas + (packed) atlas map texture
// index is read from map texture			 
// index is used to index into the 1D (packed) atlas map texture (array)
// rgba value at that location is used to look up texture in atlas
// rg = position in atlas
// ba = size of texture
//////////////////////////////////////
#include <TextureAtlas.h>
#include <globals.h>
#include <RenderManager.h>
#include <DataFile.h>
#include <DataFileIterator.h>
#include <GameSystemInterface.h>

TextureAtlas::TextureAtlas()
:_atlas_texture(0)
,_packed_data_texture(0)
,_atlas_texture_file("")
,_mapping_file("")
,_num_textures(0)
{
}

TextureAtlas::TextureAtlas(const TextureAtlas& other)
:_atlas_texture(0)
,_packed_data_texture(0)
,_atlas_texture_file("")
,_mapping_file(other._mapping_file)
,_num_textures(0)
{
	Load(_mapping_file);
}

TextureAtlas::TextureAtlas& operator =(const TextureAtlas& other)
{
	_atlas_texture = 0;
	_packed_data_texture = 0;
	_atlas_texture_file = "";
	_mapping_file = other._mapping_file;
	_num_textures = 0;
	Load(_mapping_file);
}


TextureAtlas::~TextureAtlas()
{
  //TODO: notify render manager that _atlas_texture is not being used anymore so it can unload it if nothing is using it
  External::system->GetRenderManager().UnloadTexture(_atlas_texture_file);
  glDeleteTextures(1, &_packed_data_texture); 
}

unsigned TextureAtlas::GetAtlasTexture() const
{
  return _atlas_texture;
}

unsigned TextureAtlas::GetNumTextures() const
{
  return _num_textures;
}

void TextureAtlas::GLBindAtlasTexture() const
{
    glBindTexture(GL_TEXTURE_2D, _atlas_texture);
}

void TextureAtlas::GLBindMapTexture() const
{
    glBindTexture(GL_TEXTURE_2D, _packed_data_texture);
}

bool TextureAtlas::Load(const std::string& atlas_map_file)
{
    //a mapping file is used to define what texture an atlas uses
    //and what uv coords map to each texture contained within the atlas image
    DataFile atlas_map;
    if( !atlas_map.Load(atlas_map_file) )
    {
	    std::cerr<<"Failed to load atlas mapping file "<<atlas_map_file <<"\n";
	    return false;
    }

    
    SerializedObject atlas;
    if( !atlas_map.GetFileObject("Atlas", atlas) )
    {
	    std::cerr<<"Atlas mapping file "<<atlas_map_file <<" does not contain an 'Atlas' object.\n";
	    return false;
    }
    
    if( !atlas.HasValue("AtlasFile") )
    {
	    std::cerr<<"Atlas mapping file "<<atlas_map_file <<" does not specify an 'AtlasFile' texture.\n";
	    return false;
    }

        
    std::string atlas_texture_filename;
    SERIALIZE_MEMBER(atlas, "AtlasFile", atlas_texture_filename)
    
	unsigned load_result = External::system->GetRenderManager().LoadTexture(atlas_texture_filename);
    if( load_result == 0 )
    {
	    std::cerr<<"Atlas mapping file "<<atlas_map_file <<" failed to load atlas texture "<<atlas_texture_filename <<"\n";
	    return false;
    }
    
    if( _atlas_texture )
    {
      External::system->GetRenderManager().UnloadTexture(_atlas_texture_file);
    }
	_atlas_texture_file = atlas_texture_filename;
    
    _mapping_file = atlas_map_file;
    _atlas_texture = load_result;

    //at this point the atlas file exists and the texture file exists, so create the atlas
    if( LoadMappingData() )
      PackMappingData();
    else
      return false;
    
    return true;
}

bool TextureAtlas::LoadMappingData()
{
    //we already verified that the file exists and contains atlas data
    DataFile atlas_map;
    atlas_map.Load(_mapping_file);

    SerializedObject atlas;
    atlas_map.GetFileObject("Atlas", atlas);
    
    SerializedObject textures;
	if( !atlas_map.GetFileObject("Textures",textures) )
    {
      std::cerr<<"No object 'Textures' found inside atlas mapping file "<<_mapping_file <<"\n";
      return false;
    }
  
    _texture_positions.clear();
    _texture_sizes.clear();
    _texture_names.clear();
    
    _num_textures = 0;
    
    //Read in all the texture objects listed inside textures.
    //Serialization order is important.
    //The first object serialized will be mapped to index 0.
    //The second to index 1, and so on. The game map is filled
    //with these index values. ~65k textures allowed per atlas.
    //Suggested naming convention to preserve ordering: 00000_<name>
    ObjectIterator<SerializedObject> iter(textures);
    for(; iter.Good(); iter.Next() )
    {
      unsigned pos[2] = {0,0};
      unsigned size[2] = {1,1};
      SerializedObject& texture_data = iter.Get();
      
      SERIALIZE_MEMBER(texture_data, "Position", pos)
      SERIALIZE_MEMBER(texture_data, "Size", size)
      
      _texture_positions.push_back( pos[0] );
      _texture_positions.push_back( pos[1] );
      
      _texture_sizes.push_back( size[0] );
      _texture_sizes.push_back( size[1] );
      
      //store the texture name mostly for editor use
      _texture_names.push_back( texture_data.GetName() );
      
      ++_num_textures;
    }
    
    return true;
}


void TextureAtlas::PackMappingData()
{
    if( _packed_data_texture > 0 )
      glDeleteTextures(1, &_packed_data_texture); 
    
    _packed_data_texture = 0;
  
    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &_packed_data_texture);
    glBindTexture(GL_TEXTURE_1D, _packed_data_texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    unsigned short* __restrict__ packed_data = new unsigned short[ 4 * _num_textures ];

    for(unsigned j = 0; j < _num_textures; ++j)
    {
        packed_data[(j * 4)    ] = _texture_positions[j * 2];
        packed_data[(j * 4) + 1] = _texture_positions[j * 2 + 1];

        packed_data[(j * 4) + 2] = _texture_sizes[j * 2];
        packed_data[(j * 4) + 3] = _texture_sizes[j * 2 + 1];
    }

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16UI, _num_textures, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, packed_data);
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_TEXTURE_1D);
    
    delete packed_data;
}