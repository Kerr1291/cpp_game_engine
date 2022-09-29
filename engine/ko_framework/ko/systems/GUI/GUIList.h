#ifndef GUILIST_H
#define GUILIST_H

#include <GUINode.h>

class GUIList : public GUINode
{
    float spacing;
    float list_location;

    void UpdateSpacing();

    truth NodeIsVisible(GUINode* n);

    void RenderListNodes( std::map<int, std::set<GUINode*> >& nodes );
    void UpdateListNodes( std::map<int, std::set<GUINode*> >& nodes );

    void UpdateScrollbarParams();

    truth MouseOverScrollbar();
    void RenderScrollbar();

    truth HasScrollbar();

    vec2 scrollbar_points[4];
    truth scrolling_with_mouse;
    vec2 mouse_previous_pos;
    float scrollbar_step;

    public:

        GUIList(const std::string& name = "");

        void Render();

        truth MouseOver();
        void Update();

        void SetSpacing(float new_spacing);

        void AddItem(GUINode* item);

        vec3 scrollbar_color;
};

#endif
