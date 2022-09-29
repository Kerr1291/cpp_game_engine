#include <RenderObject.h>
#include <memory.h>

#include <GraphicData.h>

#include <Sprite.h>

#include <RenderManager.h>

RenderObject::RenderObject(Transform* t)
:visible(1)
,transform(t)
,gfx(0)
,name("")
{
        if( !t )
                throw 0;

        texture = 0;
		sprite = 0;
}


RenderObject::RenderObject(Transform* t, GraphicData* g)
:visible(1)
,transform(t)
,gfx(g)
,name("")
{
        if( !t )
                throw 0;

        if( !g )
                throw 0;

        colors = std::vector<float*>( gfx->gfxPoints.size(), 0 );
        for( unsigned i = 0; i < colors.size(); ++i )
        {
                colors[i] = new float[4];
                memset( colors[i], 0, sizeof(float)*4 );
                colors[i][0] = 1; colors[i][3] = 1;
        }
        //memset( color, 0, sizeof( float )*4 );
        //color[3] = 1.0f;

        texture = 0;
		sprite = 0;
}

RenderObject::~RenderObject()
{
        for( unsigned i = 0; i < colors.size(); ++i )
        {
                delete[] colors[i];
        }
}

void RenderObject::SetColor(unsigned i, float r, float g, float b, float a)
{
        float color[4];
        color[0] = r; color[1] = g; color[2] = b; color[3] = a;
        memcpy( colors[i], color, sizeof(float)*4 );
}

void RenderObject::ExposeToTweakbar( TwBar* bar, const std::string& parent )
{
	if( sprite )
		sprite->ExposeToTweakbar(bar, parent);
}

void RenderObject::RemoveFromTweakbar( TwBar* bar, const std::string& parent )
{
	if( sprite )
		sprite->RemoveFromTweakbar(bar, parent);
}
