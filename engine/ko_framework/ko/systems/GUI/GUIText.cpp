#include <GUIText.h>


GUIText::GUIText(const std::string& name)
//:font("./Graphics/comicate.ttf") //TODO: make a font folder and store the font assets in it and make a font path
:GUINode(1, name)
,text("")
,color( vec3(1.0f,1.0f,1.0f) )
{
	//font.FaceSize(32);
}

void GUIText::Render()
{
    if( hidden )
        return;

	glColor4f( color.r, color.g, color.b, alpha );
	glRasterPos2i( screen_pos.x, screen_pos.y+13 );

	//TODO: figure out how this should work
	//font.Render(text.c_str());//,-1, FTPoint(screen_pos.x-100, screen_pos.y));

	glutBitmapString( GLUT_BITMAP_8_BY_13, (unsigned char*)text.c_str() );
}

truth GUIText::MouseOver()
{
    vec2 mouse = Global::Mouse::ScreenPos;
    if( mouse.x < screen_pos.x )
        return 0;
    if( mouse.y < screen_pos.y )
        return 0;
    if( mouse.x > screen_pos.x + text.size() * 8 )
        return 0;
    if( mouse.y > screen_pos.y + 13 )
        return 0;

    return 1;
}

void GUIText::Update()
{
}
