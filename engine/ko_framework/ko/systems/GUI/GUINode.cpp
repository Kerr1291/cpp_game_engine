#include <GUINode.h>

#include <GUIManager.h>

GUINode::~GUINode()
{
    std::cerr<<"deleting " <<this->full_name <<"\n";
    if( is_component == 0 ){
        DeleteNodes(children);
        DeleteNodes(members);
    }
}


truth GUINode::HasChildren()
{
    return ( ( children.size() > 0 ) ? 1 : 0 );
}

truth GUINode::HasMembers()
{
    return ( ( members.size() > 0 ) ? 1 : 0 );
}

GUINode* GUINode::GetNode(const std::string& name)
{
	if( is_component )
		return 0;

	GUINode* node = 0;

	node = FindNodeInMap( members, name );

	if( node )
		return node;

	node = FindNodeInMap( children, name );

	return node;
}

GUINode* GUINode::FindNode(const std::string& name)
{
	if( is_component )
		return 0;

	GUINode* node = 0;

	node = PartialMatchFindNodeInMap( members, name );

	if( node )
		return node;

	node = PartialMatchFindNodeInMap( children, name );

	return node;
}

void GUINode::SetName(const std::string& new_name)
{
	name = new_name;
	UpdateName();
}


void GUINode::SetParent(GUINode* _parent)
{
    parent = _parent;

	UpdateName();
    UpdateScreenTransform();
}


void GUINode::AttachMember(int z_order, GUINode* member)
{
	if( is_component )
		return;

    InsertNode(members, z_order, member);
    member->SetParent(this);
    member->UpdateRoot();
}


void GUINode::AttachChild(int z_order, GUINode* child)
{
	if( is_component )
		return;

    InsertNode(children, z_order, child);
    child->SetParent(this);
    child->UpdateRoot();
}

void GUINode::RemoveMember(GUINode* member)
{
	if( is_component )
		return;

    RemoveNode( members, member );
}

void GUINode::RemoveChild(GUINode* child)
{
	if( is_component )
		return;

    RemoveNode( children, child );
}

void GUINode::SetPos(const vec2& pos)
{
    local_pos = pos;
    UpdateScreenTransform();
}

void GUINode::SetPosRelative(const vec2& pos)
{
	vec2 relative_size = ( parent ? parent->GetSize() : Global::GameWindow->GetDimensions() );
	vec2 relative_pos = glm::clamp( pos , 0.0f, 1.0f ) * relative_size;
    SetPos( relative_pos );
}

void GUINode::MovePos(const vec2& pos)
{
    local_pos += pos;
	SetPos(local_pos);
}

void GUINode::MovePosRelative(const vec2& delta)
{
	vec2 relative_size = ( parent ? parent->GetSize() : Global::GameWindow->GetDimensions() );
	vec2 relative_delta = glm::clamp( delta , 0.0f, 1.0f ) * relative_size;
    MovePos( relative_delta );
}

void GUINode::SetSize(const vec2& size)
{
    local_node_size = size;
    UpdateScreenTransform();
}

void GUINode::SetSizeRelative(const vec2& size)
{
    local_node_size = glm::clamp( size , 0.0f, 1.0f ) * ( parent ? parent->GetSize() : Global::GameWindow->GetDimensions() );
    SetSize( local_node_size );
}

void GUINode::SetLocalTransform(const vec2& pos, const vec2& size)
{
    SetSize( size );
    SetPos( pos );
}

void GUINode::SetLocalTransformRelative(const vec2& relative_pos, const vec2& size )
{
    SetSizeRelative( size );
    SetPosRelative( relative_pos );
}

void GUINode::SetAlpha(float a)
{
    alpha = glm::clamp( a, 0.0f, 1.0f );
}

void GUINode::SetHidden(truth hide)
{
    hidden = hide;
}

void GUINode::DeleteThisTree()
{
    if( GetRoot() )
        External::gui->DeleteNode( GetRoot()->GetFullName() );
    else
        External::gui->DeleteNode( GetFullName() );
}

truth GUINode::RootHasFocus()
{
    return External::gui->HasFocus( GetRoot() );
}

truth GUINode::HasInputFocus()
{
    return External::gui->HasInputFocus( this );
}

void GUINode::SetInputFocus(GUINode* node)
{
    External::gui->SetInputFocus( node );
}

truth GUINode::IsMouseOverNodes(std::map<int, std::set<GUINode*> >& nodes)
{
	if( is_component )
		return 0;
	
	if( IsHidden() )
		return 0;

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
        {
            if( (*node_iter)->MouseOver() )
                return 1;
        }
    }
    return 0;
}

void GUINode::RenderNodes(std::map<int, std::set<GUINode*> >& nodes)
{
	if( is_component )
		return;

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
        {
            (*node_iter)->Render();
        }
    }
}


void GUINode::UpdateNodes(std::map<int, std::set<GUINode*> >& nodes)
{
	if( is_component )
		return;
	
	float x_diff = (screen_pos.x + total_node_size.x) - Global::GameWindow->GetW();
	float y_diff = (screen_pos.y + total_node_size.y) - Global::GameWindow->GetH();
	if( x_diff > 0 )
		MovePos( vec2(-x_diff,0) );
	if( y_diff > 0 )
		MovePos( vec2(0,-y_diff) );
	if( screen_pos.x < 0 )
		MovePos( vec2(-screen_pos.x,0) );
	if( screen_pos.y < 0 )
		MovePos( vec2(0,-screen_pos.y) );

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
        {
            (*node_iter)->Update();
        }
    }
}

void GUINode::UpdateRoot()
{
    if( parent->GetRoot() )
        root = parent->GetRoot();
    else
        root = parent;

    UpdateNodeRoots( members );
    UpdateNodeRoots( children );
}

void GUINode::UpdateNodeRoots(std::map<int, std::set<GUINode*> >& nodes)
{
    if( is_component )
        return;

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
            (*node_iter)->UpdateRoot();
    }
}

void GUINode::UpdateName()
{
	if( parent && name.size() > 0 )
	{
		full_name = parent->GetFullName() + "/" + name;
		UpdateNodeNames( members );
		UpdateNodeNames( children );
	}
	else
	{
		full_name = name;
	}
}

void GUINode::InsertNode(std::map<int, std::set<GUINode*> >& nodes, int z, GUINode* n)
{
	if( is_component )
		return;

    std::map<int, std::set<GUINode*> >::iterator finder = nodes.find( z );

    if( finder != nodes.end() )
    {
        std::set<GUINode*>::iterator node_finder = finder->second.find( n );
        if( node_finder != finder->second.end() )
            return;
        finder->second.insert( n );
    }
    else
    {
        std::set<GUINode*> node_set;
        node_set.insert( n );
        nodes[z] = node_set;
    }
}


void GUINode::RemoveNode(std::map<int, std::set<GUINode*> >& nodes, GUINode* n)
{
	if( is_component )
		return;

	if( nodes.size() > 0 )
	{
		std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
		for(; iter != nodes.end(); ++iter)
		{
			std::set<GUINode*>::iterator node_finder = iter->second.find( n );
			if( node_finder != iter->second.end() ) {
				iter->second.erase( node_finder );
				if( iter->second.empty() )
					nodes.erase( iter );
				return;
			}
		}
	}
}

void GUINode::DeleteNode(std::map<int, std::set<GUINode*> >& nodes, GUINode* n)
{
	if( is_component )
		return;

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_finder = iter->second.find( n );
        if( node_finder != iter->second.end() ) {

            if( External::gui->PendingGameLogic( *node_finder ) )
            {
                //check and see if there is pending game logic here, skip delete on this node if it is
                std::cerr<<"pending command found, skipping delete for now\n";
                ( *node_finder )->SetHidden(1);
                External::gui->DeleteNode( *node_finder );
            }
            else
            {
                delete *node_finder;
            }

            iter->second.erase( node_finder );
			if( iter->second.empty() )
				nodes.erase( iter );
            return;
        }
    }
}

void GUINode::DeleteNodes(std::map<int, std::set<GUINode*> >& nodes)
{
	if( is_component )
		return;

	if( nodes.size() > 0 )
	{
		std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
		for(; iter != nodes.end(); ++iter)
		{
			std::set<GUINode*>::iterator node_iter = iter->second.begin();
			for(; node_iter != iter->second.end(); ++node_iter)
			{
                if( External::gui->PendingGameLogic( *node_iter ) )
                {
                    //check and see if there is pending game logic here, skip delete on this node if it is
                    std::cerr<<"pending command found, skipping delete for now\n";
                    ( *node_iter )->SetHidden(1);
                    External::gui->DeleteNode( *node_iter );
                    continue;
                }
                else
                {
                    delete *node_iter;
                }
			}
		}
        nodes.clear();
	}
}

void GUINode::UpdateNodeTransforms(std::map<int, std::set<GUINode*> >& nodes)
{
	if( is_component )
		return;

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
            (*node_iter)->UpdateScreenTransform();
    }
}

void GUINode::UpdateNodeNames(std::map<int, std::set<GUINode*> >& nodes)
{
	if( is_component )
		return;

    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
            (*node_iter)->UpdateName();
    }
}

GUINode* GUINode::FindNodeInMap(std::map<int, std::set<GUINode*> >& nodes, const std::string& name)
{
    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
		{
		    //std::cerr<<"full name: " <<(*node_iter)->GetFullName() <<" == " <<name <<"\n";
			if( (*node_iter)->GetFullName() == name )
				return (*node_iter);

			GUINode* recursive_result = (*node_iter)->GetNode( name );
			if( recursive_result )
				return recursive_result;
		}
    }
	return 0;
}

GUINode* GUINode::PartialMatchFindNodeInMap(std::map<int, std::set<GUINode*> >& nodes, const std::string& name)
{
    std::map<int, std::set<GUINode*> >::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        std::set<GUINode*>::iterator node_iter = iter->second.begin();
        for(; node_iter != iter->second.end(); ++node_iter)
		{
		    //std::cerr<<"full name: " <<(*node_iter)->GetFullName() <<" contains? " <<name <<"\n";
            if ((*node_iter)->GetFullName().find(name) != std::string::npos)
                return (*node_iter);

			GUINode* recursive_result = (*node_iter)->FindNode( name );
			if( recursive_result )
				return recursive_result;
		}
    }
	return 0;
}

void GUINode::UpdateScreenTransform()
{
    GUINode* p = parent;
    if( !p )
    {
        screen_pos = local_pos;// * vec2( Global::GameWindow->GetW(), Global::GameWindow->GetH() );
        total_node_size = local_node_size;// * vec2( Global::GameWindow->GetW(), Global::GameWindow->GetH() );
    }
	else
	{
		screen_pos = p->screen_pos + local_pos;// * p->total_node_size;
		total_node_size = local_node_size;// * p->total_node_size;

		#if 0

		vec2 p_max = p->screen_pos + p->total_node_size;
		vec2 l_max = screen_pos + total_node_size;
		vec2 diff = l_max - p_max;

		if( diff.x > 0 )
			total_node_size.x -= diff.x;
		if( diff.y > 0 )
			total_node_size.y -= diff.y;

		vec2 min_diff = p->screen_pos - screen_pos;
		if( min_diff.x > 0 )
		{
			screen_pos.x += min_diff.x;
			total_node_size.x -= min_diff.x;
		}
		if( min_diff.y > 0 )
		{
			screen_pos.y += min_diff.y;
			total_node_size.y -= min_diff.y;
		}

		#endif
	}

	if( is_component )
		return;

    UpdateNodeTransforms(members);
    UpdateNodeTransforms(children);
}
