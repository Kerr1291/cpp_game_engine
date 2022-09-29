#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <globals.h>
#include <tweakbar.h>

struct GraphicData;
struct Transform;

class Sprite;

struct RenderObject
{
        RenderObject(Transform* t);
        RenderObject(Transform* t, GraphicData* g);
        ~RenderObject();

        void SetColor(unsigned i, float r, float g, float b, float a = 1.0f);

        unsigned texture;

		Sprite* sprite;

        truth visible;
        std::vector< float* > colors;
        Transform* const transform;
        GraphicData* const gfx;

        std::string name;

		void ExposeToTweakbar( TwBar* bar, const std::string& parent );
		void RemoveFromTweakbar( TwBar* bar, const std::string& parent );
};


#endif

