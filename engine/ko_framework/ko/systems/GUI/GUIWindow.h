#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <GUINode.h>

class GUIWindow : public GUINode
{
		vec2 mouse_previous_pos;
		truth dragging;

    public:

        GUIWindow(const std::string& name = "");
		virtual ~GUIWindow();

        void Render();
        truth MouseOver();
        void Update();

        unsigned edge_thickness;
        vec3 bg_color;
        vec3 edge_color;
};

#endif