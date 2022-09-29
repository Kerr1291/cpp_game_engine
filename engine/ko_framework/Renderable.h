#ifndef __RENDERABLE_H
#define __RENDERABLE_H

#include <Component.h>
#include <globals.h>

struct Animation
{
	unsigned		   _num_frames;
	int				   _current_frame;
	std::vector<uvec2> _frame_texture_pos;
	std::vector<uvec2> _frame_texture_size;

	float			   _speed;
	truth			   _looping;
	truth			   _pingpong;

	std::string		   _name;
};


class Transform;
class RenderManager;
class Renderable;
template <typename T> class ComponentPool;
class RenderableInterface;
class TextureAtlas;

BEGIN_COMPONENT( Renderable )

	friend class RenderableInterface;
	friend class RenderManager;
	friend class Editor;
	template <typename T> friend class ComponentPool;
	
		RenderableInterface* _child;
		ComponentPool<Renderable>* _owner;

	public:
        
		Transform& _transform;

	private:

        truth		_visible;
		bool		_paused;
		bool		_flipped;
        std::string _animation_file;
		std::string _atlas_texture_file;
        unsigned	_atlas_texture;
		unsigned	_active_animation;
		int			_direction;
		float		_time;
		float		_rate;
		bool		_current_animation_complete;
		unsigned	_num_animations;

		std::map<std::string, unsigned> _animation_map;
		std::vector< Animation >		_animations;

		//only a component pool may create renderables
		Renderable(Transform& transform);
		void KillThisComponent();

	public:

		Renderable(const Renderable& other);
		~Renderable();
		void operator=(const Renderable& other){}//disabled
	
	private:

		void InitMembers();

		void Update(float dt);
		
		void Serialize(SerializedObject& obj);
		
		void ReloadAnimationFile();

		void LoadSprite(const std::string& filename);

};
#endif