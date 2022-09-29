#include <Sprite.h>
#include <RenderManager.h>
#include <Parser.h>

Sprite::Sprite(Transform* t)
:transform(t)
{
	InitMembers();
}

Sprite::~Sprite()
{
}

void Sprite::Update(float dt)
{
	if( animations[ active_animation ].num_frames < 2 )
    {
        current_animation_complete = 1;
		return;
    }

	if( paused )
		return;

	SpriteAnimation& a = animations[ active_animation ];

	time += dt * a.speed * rate;

	if( time >= 1.0f )
	{
		int previous_frame = a.current_frame;
		a.current_frame += direction;

		if( a.current_frame >= static_cast<int>( a.num_frames ) )
		{
			if( a.looping )
			{
				if( a.pingpong )
				{
					a.current_frame = previous_frame-1;
					direction = -1;
				}
				else
					a.current_frame = 0;
			}
			else
			{
				a.current_frame = previous_frame;
                current_animation_complete = 1;
			}
		}

		if( a.current_frame < 0 )
		{
			if( a.looping )
			{
				if( a.pingpong )
				{
					a.current_frame = previous_frame+1;
					direction = 1;
				}
				else
					a.current_frame = a.num_frames-1;
			}
			else
			{
				a.current_frame = previous_frame;
                current_animation_complete = 1;
			}
		}

		time = 0;
	}
}

void Sprite::LoadSprite(const std::string& filename)
{
	INIFile atlas_map( filename );

	if( !atlas_map.IsGood() )
	{
		std::cerr<<"Failed to open sprite: " <<filename <<"\n";
		return;
	}

	//////////////////////////////
	//Reset the sprite
	InitMembers();
	//
	//////////////////////////////

	file = filename;

	std::string atlas_file;
    atlas_map.GetValue("numAnimations", num_animations);
    atlas_map.GetValue("spriteFile", atlas_file);

	atlas_texture = External::renderer->LoadTexture( atlas_file );

	if( atlas_texture == 0 )
	{
		std::cerr<<"Failed to open sprite atlas: " <<atlas_file <<"\n";
		return;
	}

    for( unsigned i = 0; i < num_animations; ++i )
    {
		SpriteAnimation animation;

        int tpos[2] = {0,0}; //TODO: add short and unsigned short overloads to the POD template types in the parser
        int tsize[2] = {0,0};
		int looping = 0;
		int pingpong = 0;

        std::stringstream name_key;
        std::stringstream pos_key;
        std::stringstream size_key;
		std::stringstream frames_key;
		std::stringstream space_key;
		std::stringstream speed_key;
		std::stringstream looping_key;
		std::stringstream pingpong_key;

        name_key << i << "_name";
        atlas_map.GetValue( name_key.str(), animation.name );

		if( animation_map.find( animation.name ) != animation_map.end() )
		{
			std::cerr<<"Warning: Animation "<<i <<" with name "<<animation.name <<" has already been loaded. Skipping the loading of this animation.\n";
			continue;
		}

        pos_key		 <<animation.name << "_pos";
        size_key	 <<animation.name << "_size";
        frames_key	 <<animation.name << "_frames";
        space_key	 <<animation.name << "_spacing";
        speed_key	 <<animation.name << "_speed";
        looping_key  <<animation.name << "_looping";
        pingpong_key <<animation.name << "_pingpong";

		if( atlas_map.HasValue( pos_key.str() ) )		atlas_map.GetValue( pos_key.str(),      tpos );				   else memcpy( tpos, glm::value_ptr( uvec2(0,0) ), sizeof(unsigned)*2 );
        if( atlas_map.HasValue( size_key.str() ) )		atlas_map.GetValue( size_key.str(),     tsize );			   else memcpy( tsize, glm::value_ptr( uvec2(1,1) ), sizeof(unsigned)*2 );
		if( atlas_map.HasValue( frames_key.str() ) )	atlas_map.GetValue( frames_key.str(),   animation.num_frames); else animation.num_frames = 1;
		if( atlas_map.HasValue( space_key.str() ) )		atlas_map.GetValue( space_key.str(),    animation.h_space );   else animation.h_space = 0;
		if( atlas_map.HasValue( speed_key.str() ) )		atlas_map.GetValue( speed_key.str(),    animation.speed );	   else animation.speed = 1.0f;
		if( atlas_map.HasValue( looping_key.str() ) )	atlas_map.GetValue( looping_key.str(),  looping );			   else looping = 0;
		if( atlas_map.HasValue( pingpong_key.str() ) )	atlas_map.GetValue( pingpong_key.str(), pingpong );			   else pingpong = 0;

		animation.looping = looping;
		animation.pingpong = pingpong;

		animation.start_pos = uvec2( tpos[0], tpos[1] );
		animation.frame_size = uvec2( tsize[0], tsize[1] );
		animation.current_frame = 0;


		animation_map[ animation.name ] = animations.size();
		animations.push_back( animation );
	}
}

truth Sprite::HasAnimation(const std::string& animation_name) const
{
	if( animation_map.find( animation_name ) == animation_map.end() )
		return 0;
	return 1;
}

void Sprite::SetAnimation(const std::string& animation_name, truth reset)
{
	std::map<std::string, unsigned>::iterator anim = animation_map.find( animation_name );
	if( anim == animation_map.end() )
		return;

	active_animation = anim->second;

	if( reset ) {
		animations[ active_animation ].current_frame = 0;
		time = 0;
	}

	current_animation_complete = 0;
}

void Sprite::SetAnimation(unsigned anim_index, truth reset)
{
	if( anim_index < animations.size() )
		active_animation = anim_index;

	if( reset ) {
		animations[ active_animation ].current_frame = 0;
		time = 0;
	}

	current_animation_complete = 0;
}

void Sprite::SetAnimationDirection(int new_direction)
{
	if( new_direction > 0 )
		direction = 1;
	if( new_direction < 0 )
		direction = -1;
}

float Sprite::GetAnimationRate() const
{
	return rate;
}

void Sprite::SetAnimationRate(float new_rate)
{
	rate = new_rate;
}

unsigned Sprite::GetActiveAnimationIndex() const
{
	return active_animation;
}

std::string Sprite::GetActiveAnimationName() const
{
    return animations[ active_animation ].name;
}

unsigned Sprite::GetNumAnimations() const
{
	return animations.size();
}

void Sprite::GetAnimation(const std::string& animation_name, SpriteAnimation& out_anim) const
{
	std::map<std::string, unsigned>::const_iterator anim = animation_map.find( animation_name );
	if( anim == animation_map.end() )
		return;

	out_anim = animations[ anim->second ];
}

void Sprite::ReloadAnimationFile()
{
	InitMembers();
	LoadSprite( file );
}

truth Sprite::CurrentAnimationComplete() const
{
    return current_animation_complete;
}

void Sprite::InitMembers()
{
	animation_map.clear();
	animations.clear();

	atlas_texture = 0;
	active_animation = 0;
	direction = 1;
	paused = 0;
	flipped = 0;
	time = 0.0f;
	rate = 1.0f;
}


namespace TB_Sprite
{
	static std::string tbSpriteName;
	static std::string tbAnimationName;

	void TW_CALL tbLoadSprite(void* sprite)
	{
		if( tbSpriteName.empty() )
			return;

		Sprite* s = (Sprite*)sprite;
		s->LoadSprite(Path::GraphicAssets+tbSpriteName);
	}

	void TW_CALL tbSetAnimation(void* sprite)
	{
		if( tbAnimationName.empty() )
			return;

		Sprite* s = (Sprite*)sprite;
		if( s->HasAnimation(tbAnimationName) )
			s->SetAnimation(tbAnimationName);
	}

	void TW_CALL tbReloadSprite(void* sprite)
	{
		Sprite* s = (Sprite*)sprite;
		s->ReloadAnimationFile();
	}
}

void Sprite::ExposeToTweakbar( TwBar* bar, const std::string& parent )
{
	TwAddSeparator(bar, ("sprite"+parent).c_str(), (" group='"+parent+"'").c_str());

	TwAddVarRW(bar, ("SpriteName"+parent).c_str(), TW_TYPE_STDSTRING, &TB_Sprite::tbSpriteName, (" group='"+parent+"' label='New Sprite File:'").c_str() );
	TwAddButton(bar, ("LoadSprite"+parent).c_str(), TB_Sprite::tbLoadSprite, this, (" group='"+parent+"' label='Load Sprite File' ").c_str() );

	TwAddVarRO(bar, ("SpriteFileName"+parent).c_str(), TW_TYPE_STDSTRING, &file, (" group='"+parent+"' label='Current Sprite File:'").c_str() );
	TwAddButton(bar, ("ReloadSprite"+parent).c_str(), TB_Sprite::tbReloadSprite, this, (" group='"+parent+"' label='Reload Sprite File' ").c_str() );

	TwAddVarRW(bar, ("AnimationName"+parent).c_str(), TW_TYPE_STDSTRING, &TB_Sprite::tbAnimationName, (" group='"+parent+"' label='Animation Name:'").c_str() );
	TwAddButton(bar, ("SetAnimation"+parent).c_str(), TB_Sprite::tbSetAnimation, this, (" group='"+parent+"' label='Set Animation' ").c_str() );
}

void Sprite::RemoveFromTweakbar( TwBar* bar, const std::string& parent ) const
{
	TwRemoveVar(bar, ("sprite"+parent).c_str());

	TwRemoveVar(bar, ("SpriteName"+parent).c_str());
	TwRemoveVar(bar, ("LoadSprite"+parent).c_str());
	TwRemoveVar(bar, ("SpriteFileName"+parent).c_str());
	TwRemoveVar(bar, ("ReloadSprite"+parent).c_str());
	TwRemoveVar(bar, ("AnimationName"+parent).c_str());
	TwRemoveVar(bar, ("SetAnimation"+parent).c_str());
}
