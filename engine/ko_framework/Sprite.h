#ifndef SPRITE_H
#define SPRITE_H

#include <globals.h>
#include <tweakbar.h>

struct Transform;


struct SpriteAnimation
{
	std::string name;
	unsigned num_frames;
	uvec2 start_pos;
	uvec2 frame_size;
	unsigned h_space;
	float speed;
	truth looping;
	truth pingpong;

	int current_frame;
};


class Sprite
{
	friend class RenderManager;

	public:

        Sprite(Transform* t);
        ~Sprite();

		void Update(float dt);
		void LoadSprite(const std::string& filename);
		void ReloadAnimationFile();

		//setters
		void SetAnimation(const std::string& animation_name, truth reset = 1);
		void SetAnimation(unsigned anim_index, truth reset = 1);
		void SetAnimationDirection(int new_direction);
		void SetAnimationRate(float new_rate);

		//getters
		truth HasAnimation(const std::string& animation_name) const;
		void GetAnimation(const std::string& animation_name, SpriteAnimation& out_anim) const;
		float GetAnimationRate() const;
		unsigned GetActiveAnimationIndex() const;
		std::string GetActiveAnimationName() const;
		unsigned GetNumAnimations() const;
		truth CurrentAnimationComplete() const;

		//public members
		truth paused;
		truth flipped;
        truth visible;
		
		void ExposeToTweakbar( TwBar* bar, const std::string& parent );
		void RemoveFromTweakbar( TwBar* bar, const std::string& parent ) const;

	private:

		void InitMembers();

        std::string file;
        unsigned atlas_texture;
		unsigned active_animation;
		int direction;
		float time;
		float rate;
		truth current_animation_complete;

		unsigned num_animations;
		std::map<std::string, unsigned> animation_map;
		std::vector< SpriteAnimation > animations;

        Transform* const transform;

};


#endif
