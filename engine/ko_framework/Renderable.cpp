#include <Renderable.h>

#include <Transform.h>
#include <RenderManager.h>
#include <GameSystemInterface.h>
#include <RenderableInterface.h>

#include <TextureAtlas.h>
#include <GameObject.h>


Renderable::Renderable(Transform& transform)
:_child(0)
,_owner(0) //<-must be updated manually by the pool that copied us
,_transform(transform)
,_animation_file("")
,_atlas_texture_file("")
{
	InitMembers();
}

void Renderable::KillThisComponent()
{
	if( _child )
		_child->_parent = 0;

	if( _atlas_texture != 0 )
	{
		External::system->GetRenderManager().UnloadTexture( _atlas_texture_file );
		_atlas_texture = 0;
	}

	_parent = 0;
}

Renderable::Renderable(const Renderable& other)
:Component(other)
,_child(other._child)
,_owner(0)
,_transform(other._transform)
,_visible(other._visible)
,_paused(other._paused)
,_animation_file(other._animation_file)
,_atlas_texture_file(other._atlas_texture_file)
,_atlas_texture(other._atlas_texture)
,_active_animation(other._active_animation)
,_direction(other._direction)
,_time(other._time)
,_rate(other._rate)
,_current_animation_complete(other._current_animation_complete)
,_num_animations(other._num_animations)
,_animation_map(other._animation_map)
,_animations(other._animations)
{
	
	Renderable& non_const = const_cast<Renderable&>( other );
	non_const._child = 0;
	non_const.InitMembers();//this will clear out its variables

	//if there was an interface pointing to the other component,
	//we need to make the pointer point to this one.
	if( _child )
	{
		_child->_parent = this;
	}
}


Renderable::~Renderable()
{
	KillThisComponent();
}


void Renderable::InitMembers()
{
	_visible = 1;
	_paused = 0;
	_flipped = false;
	_atlas_texture_file = "";
    _atlas_texture = 0;
	_active_animation = 0;
	_direction = 1;
	_time = 0;
	_rate = 1.0f;
	_current_animation_complete = false;
	_num_animations = 0;
	_animation_map.clear();
	_animations.clear();
}


void Renderable::Update(float dt)
{
	if( _animations[ _active_animation ]._num_frames < 2 )
    {
        _current_animation_complete = true;
		return;
    }

	if( _paused )
		return;

	Animation& a = _animations[ _active_animation ];

	_time += dt * a._speed * _rate;

	if( _time >= 1.0f )
	{
		int previous_frame = a._current_frame;
		a._current_frame += _direction;

		if( a._current_frame >= static_cast<int>( a._num_frames ) )
		{
			if( a._looping )
			{
				if( a._pingpong )
				{
					a._current_frame = previous_frame-1;
					_direction = -1;
				}
				else
					a._current_frame = 0;
			}
			else
			{
				a._current_frame = previous_frame;
                _current_animation_complete = true;
			}
		}

		if( a._current_frame < 0 )
		{
			if( a._looping )
			{
				if( a._pingpong )
				{
					a._current_frame = previous_frame+1;
					_direction = 1;
				}
				else
					a._current_frame = a._num_frames-1;
			}
			else
			{
				a._current_frame = previous_frame;
                _current_animation_complete = true;
			}
		}

		_time = 0;
	}
}


void Renderable::Serialize(SerializedObject& obj)
{
	if( !obj.HasValue("AnimationFile") )
	{
		std::cerr<<"Renderables must specify an 'SpriteFile' that contains the sprite's data.\n";
		return;
	}

	SERIALIZE_MEMBER(obj, "AnimationFile", _animation_file)

	LoadSprite( _animation_file );
}


void Renderable::ReloadAnimationFile()
{
	LoadSprite( _animation_file );
}

void Renderable::LoadSprite(const std::string& filename)
{
	InitMembers();

	DataFile animation_file;
	if( !animation_file.Load(filename) )
	{
		std::cerr<<"Failed to load animation file "<<filename <<" for object "<<GetParent()->GetName() <<"\n";
		return;
	}
		
	SerializedObject animations;
	if( !animation_file.GetFileObject("Animations", animations) )
	{
		std::cerr<<"Animation file "<<filename <<" does not contain an 'Animations' object.\n";
		return;
	}
	
	if( !animations.HasValue("AtlasFile") )
	{
		std::cerr<<"Animation file "<<filename <<" does not specify an 'AtlasFile' texture.\n";
		return;
	}

	SERIALIZE_MEMBER(animations, "AtlasFile", _atlas_texture_file)

	_atlas_texture = External::system->GetRenderManager().LoadTexture(_atlas_texture_file);
	
	if( _atlas_texture == 0 )
	{
		std::cerr<<"Animation file "<<filename <<" failed to load atlas texture "<<_atlas_texture_file <<"\n";
		return;
	}

	//load each animation
	ObjectIterator<SerializedObject> iter(animations);
	for(; iter.Good(); iter.Next() )
	{
		Animation animation;
		SerializedObject& animation_data = iter.Get();

		animation._name = animation_data.GetName();
		animation._current_frame = 0;
		SERIALIZE_MEMBER(animation_data, "Speed", animation._speed)
		SERIALIZE_MEMBER(animation_data, "Looping", animation._looping)
		SERIALIZE_MEMBER(animation_data, "PingPong", animation._pingpong)

		if( !animation_data.HasObject("Frames") )
		{
			std::cerr<<"Animation "<<animation._name <<" has no 'Frames' object.\n";
			continue;
		}

		//load each frame
		SerializedObject frames;
		animation_data.GetInnerObject("Frames",frames);
		ObjectIterator<SerializedObject> frame_iter(animations);
		for(; frame_iter.Good(); frame_iter.Next() )
		{
			unsigned pos[2] = {0,0};
			unsigned size[2] = {0,0};

			SerializedObject& frame_data = frame_iter.Get();
			SERIALIZE_MEMBER(frame_data, "Pos", pos)
			SERIALIZE_MEMBER(frame_data, "Size", size)
			
			uvec2 frame_texture_pos = uvec2(pos[0],pos[1]);
			uvec2 frame_texture_size = uvec2(size[0],size[1]);

			animation._frame_texture_pos.push_back( frame_texture_pos );
			animation._frame_texture_size.push_back( frame_texture_size );
		}

		//grab one of the array sizes so we know how many frames we have
		animation._num_frames = animation._frame_texture_pos.size();

		if( animation._num_frames == 0 )
		{
			std::cerr<<"Animation "<<animation._name <<" has no frames inside 'Frames'.\n";
			continue;
		}

		_animation_map[ animation._name ] = _animations.size();
		_animations.push_back( animation );
	}

	_num_animations = _animations.size();
}

/*
//Example file structure
Animations
{
	AtlasFile = myfile.png
	Run
	{
		Speed = 1.0
		Looping = 1
		PingPong = 0

		Frames
		{
			Frame001 { 
				Pos = 0 0
				Size = 5 5 
			}
			Frame002 { 
				Pos = 6 10
				Size = 5 5 
			}
		}
	}
	Walk
	{
		Speed = 1.0
		Looping = 1
		PingPong = 0

		Frames
		{
			Frame001 { 
				Pos = 0 0
				Size = 5 5 
			}
		}
	}
}
*/