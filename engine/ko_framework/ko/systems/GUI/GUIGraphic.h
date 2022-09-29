#ifndef GUIGRAPHIC_H
#define GUIGRAPHIC_H

#include <GUINode.h>

class GUIGraphic : public GUINode
{
	unsigned texture;
    public:

        GUIGraphic(const std::string& name = "");

        void Render();
        truth MouseOver();
        void Update();

		void LoadTexture(const std::string& filename);
        vec3 color;
};

#endif
