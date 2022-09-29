#include <GUIGraphic.h>

#include <RenderManager.h>

GUIGraphic::GUIGraphic(const std::string& name)
//:font("./Graphics/comicate.ttf") //TODO: make a font folder and store the font assets in it and make a font path
:GUINode(1, name)
,texture(0)
,color( vec3(1.0f,1.0f,1.0f) )
{
}

void GUIGraphic::Render()
{
    if( hidden )
        return;

	if( texture == 0 )
		return;

	glEnable(GL_TEXTURE_2D);
	glColor4f( color.r, color.g, color.b, alpha );
	glBindTexture(GL_TEXTURE_2D, texture);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin( GL_QUADS );
        glVertex2f( screen_pos.x, screen_pos.y );									glTexCoord2f(0,1);
        glVertex2f( screen_pos.x, screen_pos.y + GetSize().y );					glTexCoord2f(1,1);
        glVertex2f( screen_pos.x + GetSize().x, screen_pos.y + GetSize().y ); glTexCoord2f(1,0);
        glVertex2f( screen_pos.x + GetSize().x, screen_pos.y );					glTexCoord2f(0,0);
    glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

truth GUIGraphic::MouseOver()
{
    vec2 mouse = Global::Mouse::ScreenPos;
    if( mouse.x < screen_pos.x )
        return 0;
    if( mouse.y < screen_pos.y )
        return 0;
    if( mouse.x > screen_pos.x + GetSize().x )
        return 0;
    if( mouse.y > screen_pos.y + GetSize().y )
        return 0;

    return 1;
}

void GUIGraphic::LoadTexture(const std::string& filename)
{
	if( filename.empty() )
	{
		texture = 0;
		return;
	}

	texture = External::renderer->LoadTexture( Path::GraphicAssets + filename, 1 );
}

void GUIGraphic::Update()
{
}
