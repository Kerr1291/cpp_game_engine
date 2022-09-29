#ifndef __RENDERABLE_INTERFACE_H
#define __RENDERABLE_INTERFACE_H

#include <string>

class Renderable;

class RenderableInterface
{
	friend class Renderable;

		Renderable* _parent;

	public:

		RenderableInterface();
		~RenderableInterface();
		
		void SetRenderable(Renderable* parent);

		void SetAnimation(const std::string& animation_name, bool reset = true);
		void SetAnimationDirection(int new_direction);
		void SetAnimationRate(float new_rate);
		
		bool HasAnimation(const std::string& animation_name) const;
		float GetAnimationRate() const;
		std::string GetActiveAnimationName() const;
		bool CurrentAnimationComplete() const;
};

#endif