#ifndef _TILEMAP_H
#define _TILEMAP_H

#include <globals.h>

#include <TextureAtlas.h>

class TileMap
{
    friend class RenderManager;
  	friend class Editor;
    
  public:
    TileMap();
    TileMap(const TileMap& other);
    TileMap& operator =(const TileMap& other);
    ~TileMap();

		void UnloadMap();

		bool IsInMap(float world_x, float world_y) const;
		bool IsInMap(const vec2& world_pos) const;

		void WorldToMap(uvec2& out_vec, float world_x, float world_y) const;
		void WorldToMap(uvec2& out_vec, const vec2& world_pos) const;

		uvec2 GetMapSize() const;

    void UploadTileMapData(unsigned short* tiles, unsigned w, unsigned h);
		void UpdateTileID(unsigned short data, unsigned char layer, unsigned x, unsigned y);
		void UpdateTileID(unsigned short data, unsigned char layer, uvec2 map_pos);
		void UpdateTileID_NoRefresh(unsigned short data, unsigned char layer, unsigned x, unsigned y);
		void UpdateTileID_NoRefresh(unsigned short data, unsigned char layer, uvec2 map_pos);
		void RefreshMapTexture();

		//only layers from 0 to 3 are valid
		void GetTileID(unsigned& out, unsigned char layer, unsigned x, unsigned y) const;
		void GetTileID(unsigned& out, unsigned char layer, uvec2 map_pos) const;

		bool LoadTextureMappingAtlas(const std::string& filename);
		const TextureAtlas& GetTextureMappingAtlas() const;

  private:

		//used internally only to allow safe access inside the _cells array
		unsigned GetTileIndex(unsigned layer, unsigned x, unsigned y) const;

		//generates the texture that draws the tile map
		void ReGenTileTexture();
    void GenTileTexture();

    uvec2 _size;
    unsigned _tile_texture;
    TextureAtlas _tile_texture_map;
    unsigned short* _tiles;
};


#endif
