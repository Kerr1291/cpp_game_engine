#ifndef __TEXTURE_ATLAS_H
#define __TEXTURE_ATLAS_H

/*
 * #Example atlas map file layout
 * Atlas
 * {
 * 	AtlasFile = "level_background_graphics.png"
 * 	Textures
 * 	{
 * 		00000_MyTexture
 * 		{
 * 			Position = 0 0
 * 			Size = 32 32
 * 		}
 * 		00001_OtherTexture
 * 		{
 * 			Position = 33 0
 * 			Size = 32 32
 * 		}
 * 	}
 * }
 * */

#include <string>
#include <vector>

class TextureAtlas
{
  friend class RenderManager;
  friend class TileMap;
  public:
    TextureAtlas();
    TextureAtlas(const TextureAtlas& other);
    TextureAtlas& operator =(const TextureAtlas& other);
    ~TextureAtlas();

    unsigned GetAtlasTexture() const;
    unsigned GetNumTextures() const;
    void GLBindAtlasTexture() const;
    void GLBindMapTexture() const;
    
  private:
    
    //load an atlas map file
    bool Load(const std::string& atlas_map_file);

    //helper functions
    bool LoadMappingData();
    void PackMappingData();
    
  private:
    
    unsigned _atlas_texture;
    unsigned _packed_data_texture;
    
	  std::string _atlas_texture_file;
    std::string _mapping_file;
    unsigned _num_textures;
    
    std::vector< unsigned short > _texture_positions;
    std::vector< unsigned short > _texture_sizes;
    std::vector< std::string > _texture_names;
};

#endif