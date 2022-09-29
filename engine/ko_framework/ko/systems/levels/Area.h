#ifndef AREA_H
#define AREA_H

#include <globals.h>
#include <TileMap.h>

class GameObject;

class Area
{
	friend class Editor;
	public:

		Area();
		//Area(const Area& other);
		//Area& operator =(const Area& other);
		~Area();
		std::string GetName() const;

		void Load(const std::string& filename);
		void Init();
		void ClearDeleteList();
		void Unload();

		const TileMap& GetTileMap() const;

        void AddObject( GameObject* obj );
        void RemoveObject( GameObject* obj );

	protected:

        void ForceRemoveObject( GameObject* obj );
		void RemoveAllObjects();

		std::string _name;
		TileMap _tile_map;
		std::set<GameObject*> _game_objects;
        std::set<GameObject*> _delete_list;
};


#endif
