#ifndef GUITEXT_H
#define GUITEXT_H

#include <GUINode.h>

class GUIText : public GUINode
{
	//FTGLPixmapFont font;
    public:

        GUIText(const std::string& name = "");

        void Render();
        truth MouseOver();
        void Update();

        std::string text;
        vec3 color;
};

#endif
