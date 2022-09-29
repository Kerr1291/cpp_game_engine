

#include <GUIManager.h>

#include <GUINode.h>
#include <SystemInterface.h>
#include <GUIButton.h>

GUIManager::GUIManager()
:_hidden(false)
,focus(0)
,input_focus(0)
{
    External::gui = this;
}

GUIManager::~GUIManager()
{
    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
            delete iter->second;
}

void GUIManager::AddNode(GUINode* root_node)
{
    if( NodeExists( root_node ) )
        return;

    //can only add root nodes
    if( root_node->parent )
        return;

    if( nodes.empty() )
        nodes[0] = root_node;
    else
        nodes[ nodes.rbegin()->first + 1 ] = root_node;

    focus = root_node;
}

GUINode* GUIManager::FindRootNode(const std::string& name)
{
    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        if( iter->second->GetName() == name )
            return iter->second;
    }

    return 0;
}

void GUIManager::DeleteNode(GUINode* node)
{
    //TODO: run a find on this list to make sure nodes are double-y added to the list
    if( node )
    {
        RemoveNode( node );
        //TODO: move this push back into a private add function that does the double add check
        delete_list.push_back( node );
    }
}

void GUIManager::DeleteNode(const std::string& name)
{
    GUINode* n = FindRootNode(name);
    if( n )
    {
        RemoveNode( n );
        delete_list.push_back( n );
    }
}

void GUIManager::RemoveNode(GUINode* node)
{
    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        if( iter->second == node )
        {
            if( focus == iter->second )
                focus = 0;

            if( input_focus == iter->second )
                input_focus = 0;

            nodes.erase( iter );
            return;
        }
    }
}

truth GUIManager::PendingGameLogic( GUINode* node )
{
    GUIButton* button_check = dynamic_cast< GUIButton* >( node );
    if( button_check )
    {
        std::cerr<<"button found\n";
        //check and see if the command is pending here, skip delete on this node if it is
        if( External::system->HasPendingCommand( button_check->command ) )
        {
            std::cerr<<"pending command found, skipping delete for now\n";
            return 1;
        }
    }

    return 0;
}


void GUIManager::Cleanup()
{
    if( delete_list.size() > 0 )
    {
        truth allow_clear = 1;
        std::list< GUINode* >::iterator delete_iter = delete_list.begin();
        for(; delete_iter != delete_list.end(); ++delete_iter)
		{
            std::cerr<<"starting delete...\n";

			if( (*delete_iter)->GetParent() ){
				(*delete_iter)->GetParent()->RemoveMember( (*delete_iter) );
				(*delete_iter)->GetParent()->RemoveChild( (*delete_iter) );
			}

            //Special behavior for buttons, we can't delete them if they
            //have a pending command to run in the queue... so just hide them until they can be deleted
            if( PendingGameLogic( *delete_iter ) )
            {
                //check and see if there is pending game logic here, skip delete on this node if it is
                std::cerr<<"pending command found, skipping delete for now\n";
                ( *delete_iter )->SetHidden(1);
                allow_clear = 0;
                continue;
            }

            std::cerr<<"deleting node\n";
            //delete and remove node
            delete *delete_iter;
            std::cerr<<"removing from list\n";
            delete_iter = delete_list.erase( delete_iter );
            std::cerr<<"finished delete\n";
	
			//this keeps the for loop from performing an increment on a bad iterator if the list is empty
			if( delete_list.empty() )
				break;
		}

        //i personally like clearing lists just to ensure the nodes are gone for good
		if( allow_clear )
            delete_list.clear();
    }
}

void GUIManager::Update()
{
    signal_flag = 0;

    if( focus )
    {
        focus->Update();
    }

    std::map<int, GUINode*>::reverse_iterator r_iter = nodes.rbegin();
    for(; r_iter != nodes.rend(); ++r_iter)
    {
        if( Global::Mouse::GetState( Button::LEFT_DOWN ) && r_iter->second->MouseOver() && focus != r_iter->second )
        {
            if( focus && focus->MouseOver() )
                return;

            focus = r_iter->second;
            MoveFocusToFront();
            return;
        }
    }

    if( focus && Global::Mouse::GetState( Button::LEFT_DOWN ) )
    {
        std::map<int, GUINode*>::reverse_iterator r_iter = nodes.rbegin();
        for(; r_iter != nodes.rend(); ++r_iter)
        {
            if( r_iter->second->MouseOver() )
                return;
        }
        focus = 0;
        input_focus = 0;
    }
}

void GUIManager::Render()
{
	if( _hidden )
		return;

    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        if( focus != iter->second )
            iter->second->Render();
    }

    if( focus )
        focus->Render();
}

truth GUIManager::HasFocus(GUINode* root_node)
{
    if( root_node == focus )
        return 1;
    return 0;
}

GUINode* GUIManager::GetFocus()
{
    return focus;
}

void GUIManager::SetInputFocus(GUINode* node)
{
    input_focus = node;
}

GUINode* GUIManager::GetInputFocus()
{
    return input_focus;
}

truth GUIManager::HasInputFocus(GUINode* node)
{
    if( input_focus == node )
        return 1;
    return 0;
}

void GUIManager::SetSignalFlag()
{
    signal_flag = 1;
}

truth GUIManager::GetSignalFlag()
{
    return signal_flag;
}

truth GUIManager::MouseOverGUI()
{
    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
		if( iter->second->IsHidden() )
			continue;

         if( iter->second->MouseOver() )
			 return 1;
    }
	return 0;
}

void GUIManager::SetGUIHidden(bool hidden)
{
	_hidden = hidden;
}

truth GUIManager::NodeExists(GUINode* node)
{
    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        if( iter->second == node )
            return 1;
    }
    return 0;
}

void GUIManager::MoveFocusToFront()
{
    int front_z_order = nodes.rbegin()->first;
    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        if( focus == iter->second )
        {
            nodes.erase( iter );
            break;
        }
    }

    nodes[ ++front_z_order ] = focus;
    PackBack();
}

void GUIManager::PackBack()
{
    while( nodes.find( 0 ) == nodes.end() && nodes.begin()->first > 0 )
        ShiftNodes(-1);
}

void GUIManager::ShiftNodes(int direction)
{
    std::map<int, GUINode*> shifted;

    std::map<int, GUINode*>::iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        shifted[ iter->first + direction ] = iter->second;
    }

    nodes.clear();
    nodes = shifted;
}













