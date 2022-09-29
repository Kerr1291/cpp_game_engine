#include <GUIList.h>
#include <GUIManager.h>

#include <GUIWindow.h>

void GUIList::UpdateSpacing()
{
    std::map<int, std::set<GUINode*> >::iterator iter = children.begin();

    list_location = glm::clamp( list_location, 0.0f, 1.0f );
    vec2 next_pos = vec2(0, list_location * -GetSize().y);
    for(; iter != children.end(); ++iter)
    {
        std::set<GUINode*>::iterator child_iter = iter->second.begin();
        for(; child_iter != iter->second.end(); ++child_iter)
        {
            (*child_iter)->SetPos( next_pos );
            next_pos = vec2( next_pos.x, next_pos.y + (*child_iter)->GetSize().y + spacing );
        }
    }

    UpdateScrollbarParams();
}

truth GUIList::NodeIsVisible(GUINode* n)
{
    if(!GetParent())
        return 1;

    GUINode* p = GetParent()->GetParent();
    if(!p)
        return 1;

    if( n->GetScreenPos().y > (p->GetScreenPos().y + p->GetSize().y) )
        return 0;

    if( (n->GetScreenPos().y + n->GetSize().y) > (p->GetScreenPos().y + p->GetSize().y) )
        return 0;

    if( n->GetScreenPos().y < p->GetScreenPos().y )
        return 0;

    if( (n->GetScreenPos().y + n->GetSize().y) < p->GetScreenPos().y )
        return 0;

    return 1;
}


void GUIList::RenderListNodes( std::map<int, std::set<GUINode*> >& nodes )
{
    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
        {
            GUINode* n = (*node_iter);
            if( NodeIsVisible(n) )
                n->Render();
        }
    }
}

void GUIList::UpdateListNodes( std::map<int, std::set<GUINode*> >& nodes )
{
    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
        {
            GUINode* n = (*node_iter);
            if( NodeIsVisible(n) )
                n->Update();
        }
    }
}

void GUIList::UpdateScrollbarParams()
{
    if(!this->GetParent())
        return;
    GUINode* p = this->GetParent()->GetParent();
    if(!p)
        return;

    float ratio =  p->GetSize().y / GetSize().y;
    scrollbar_step = ratio * 0.1f;

    const float scrollbar_width = 10.0f;
    const float scrollbar_height = ratio * p->GetSize().y;
    vec2 scrollbar_anchor =
        p->GetScreenPos() +
        vec2( p->GetSize().x, list_location * p->GetSize().y - list_location * ratio * p->GetSize().y) - vec2(scrollbar_width,0);

    scrollbar_points[0] = vec2(0,0)                              + scrollbar_anchor;
    scrollbar_points[1] = vec2(0,              scrollbar_height) + scrollbar_anchor;
    scrollbar_points[2] = vec2(scrollbar_width,scrollbar_height) + scrollbar_anchor;
    scrollbar_points[3] = vec2(scrollbar_width,0)                + scrollbar_anchor;
}

truth GUIList::MouseOverScrollbar()
{
    vec2 mouse = Global::Mouse::ScreenPos;
    if( mouse.x < scrollbar_points[0].x )
        return 0;
    if( mouse.y < scrollbar_points[0].y )
        return 0;
    if( mouse.x > scrollbar_points[2].x )
        return 0;
    if( mouse.y > scrollbar_points[2].y )
        return 0;

    return 1;
}

void GUIList::RenderScrollbar()
{
    float color_scale = 1.0f;
    if( MouseOverScrollbar() )
    {
        color_scale = 1.5f;
    }

    glColor4f( color_scale * scrollbar_color.r, color_scale * scrollbar_color.g, color_scale * scrollbar_color.b, alpha );

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f( scrollbar_points[0].x, scrollbar_points[0].y );
        glVertex2f( scrollbar_points[1].x, scrollbar_points[1].y );
        glVertex2f( scrollbar_points[2].x, scrollbar_points[2].y );
        glVertex2f( scrollbar_points[3].x, scrollbar_points[3].y );
    glEnd();

    glColor4f( 1.0f, 1.0f, 1.0f, alpha );

    glBegin(GL_LINE_LOOP);
        glVertex2f( scrollbar_points[0].x, scrollbar_points[0].y );
        glVertex2f( scrollbar_points[1].x, scrollbar_points[1].y );
        glVertex2f( scrollbar_points[2].x, scrollbar_points[2].y );
        glVertex2f( scrollbar_points[3].x, scrollbar_points[3].y );
    glEnd();
    glDisable(GL_BLEND);
}

truth GUIList::HasScrollbar()
{
    if(!this->GetParent())
        return 0;
    GUINode* p = this->GetParent()->GetParent();
    if(!p)
        return 0;

    float ratio =  p->GetSize().y / GetSize().y;
    if( ratio >= 1.0f )
        return 0;

    return 1;
}

GUIList::GUIList(const std::string& name)
:GUINode(0, name)
,spacing(2.0f)
,list_location(0.0f)
,scrollbar_color( vec3( 0.7f, 0.7f, 0.7f ) )
,scrollbar_step(1.0f)
{
    UpdateScrollbarParams();
}

void GUIList::Render()
{
    if( hidden )
        return;

    UpdateSpacing();

    RenderListNodes( members );
    RenderListNodes( children );

    if( HasScrollbar() )
        RenderScrollbar();
}


truth GUIList::MouseOver()
{
    if( IsMouseOverNodes( children ) )
        return 1;

    if( IsMouseOverNodes( members ) )
        return 1;

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

void GUIList::Update()
{
    if( MouseOver() && RootHasFocus() && glm::abs( Global::Mouse::WheelDelta ) > 0 )
    {
        list_location += Global::Mouse::WheelDelta * scrollbar_step;
        list_location = glm::clamp( list_location, 0.0f, 1.0f );
    }

    if( HasScrollbar() )
    {
        if( MouseOverScrollbar() && Global::Mouse::GetState( Button::LEFT_DOWN ) )
        {
            scrolling_with_mouse = 1;
            mouse_previous_pos = Global::Mouse::ScreenPos;
        }
        if( scrolling_with_mouse && Global::Mouse::GetState( Button::LEFT_HELD ) )
        {
            vec2 current_pos = Global::Mouse::ScreenPos;
            vec2 delta = current_pos - mouse_previous_pos;
            mouse_previous_pos = current_pos;

            float direction = 0;
            if( glm::abs(delta.y) > Global::TINY )
                direction = (delta.y < 0 ? -1 : 1);

            list_location += direction * scrollbar_step;
            list_location = glm::clamp( list_location, 0.0f, 1.0f );
        }
        if( scrolling_with_mouse && Global::Mouse::GetState( Button::LEFT_UP ) )
        {
            scrolling_with_mouse = 0;
            return;
        }
    }

	UpdateListNodes( children );
	UpdateListNodes( members );
}

void GUIList::SetSpacing(float new_spacing)
{
    unsigned num_children = 0;
    std::map<int, std::set<GUINode*> >::iterator iter = children.begin();
    for(; iter != children.end(); ++iter)
    {
        num_children += iter->second.size();
    }

    vec2 size = GetSize();
    size.y -= spacing * num_children;
    if( GetParent() )
        GetParent()->SetSize( size );
    SetSize( size );

    spacing = new_spacing;
    size.y += spacing * num_children;

    if( GetParent() )
        GetParent()->SetSize( size );
    SetSize( size );
}

void GUIList::AddItem(GUINode* item)
{
    if( FindNode( item->GetName() ) )
        return;

    vec2 size = GetSize();
    vec2 item_size = item->GetSize();

    if(size.x < item_size.x)
        size.x = item_size.x;
    size.y += item_size.y + spacing;

    if( GetParent() )
        GetParent()->SetSize( size );
    SetSize( size );

    int z_order = children.size();
    AttachChild(z_order, item);
}
