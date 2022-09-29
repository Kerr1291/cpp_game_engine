#ifndef GUINODE_H
#define GUINODE_H

#include <globals.h>

class GUINode
{
    friend class GUIManager;

    public:

		virtual void Render(){}
        virtual truth MouseOver(){return 0;}
        virtual void Update(){}

    protected:

        GUINode* parent;

        std::map<int, std::set<GUINode*> > members; //these are drawn after children, before parent
        std::map<int, std::set<GUINode*> > children; //children are drawn before members

        vec2 local_pos;
        vec2 local_node_size;
        vec2 screen_pos;
        vec2 total_node_size;

        truth hidden;
        float alpha;
		const truth is_component;

	private:

		std::string name;
		std::string full_name;

        GUINode* root;

    public:

		GUINode(truth component, std::string node_name = "")
        :parent(0)
        ,local_pos(vec2(0,0))
        ,local_node_size(vec2(1.0f,1.0f))
        ,screen_pos(vec2(0,0))
        ,total_node_size(vec2(1.0f,1.0f))
        ,hidden(0)
        ,alpha(1.0f)
		,is_component(component)
		,name(node_name)
		,full_name(name)
		,root(0)
        {}

		virtual ~GUINode();

        truth HasChildren();
        truth HasMembers();

		GUINode* GetNode(const std::string& name);
		GUINode* FindNode(const std::string& name);
		void SetName(const std::string& new_name);
		std::string GetName() { return name; }
		std::string GetFullName() { return full_name; }
		GUINode* GetRoot() { return root; }
		truth IsComponent() { return is_component; }
		GUINode* GetParent() { return parent; }
		truth IsHidden() { return hidden; }

        void SetParent(GUINode* _parent);
        void AttachMember(int z_order, GUINode* member);
        void AttachChild(int z_order, GUINode* child);

        void RemoveMember(GUINode* member);
        void RemoveChild(GUINode* child);

        void SetPos(const vec2& pos);
        void SetPosRelative(const vec2& relative_pos);
		void MovePos(const vec2& delta);
		void MovePosRelative(const vec2& delta);
        void SetSize( const vec2& size );
        void SetSizeRelative(const vec2& size);
        void SetLocalTransform(const vec2& pos, const vec2& size);
        void SetLocalTransformRelative(const vec2& absolute_position, const vec2& size );

        void SetAlpha(float a);
        void SetHidden(truth hide);

		vec2 GetScreenPos() { return screen_pos; }
		vec2 GetSize() { return total_node_size; }

		void DeleteThisTree();

    protected:

        truth RootHasFocus();
        truth HasInputFocus();
        void SetInputFocus(GUINode* node);

        truth IsMouseOverNodes(std::map<int, std::set<GUINode*> >& nodes);
        void RenderNodes(std::map<int, std::set<GUINode*> >& nodes);
		void UpdateNodes(std::map<int, std::set<GUINode*> >& nodes);

    private:

        void UpdateRoot();
        void UpdateNodeRoots(std::map<int, std::set<GUINode*> >& nodes);

		void UpdateName();
        void InsertNode(std::map<int, std::set<GUINode*> >& nodes, int z, GUINode* n);

        void RemoveNode(std::map<int, std::set<GUINode*> >& nodes, GUINode* n);
        void DeleteNode(std::map<int, std::set<GUINode*> >& nodes, GUINode* n);
        void DeleteNodes(std::map<int, std::set<GUINode*> >& nodes);

        void UpdateNodeTransforms(std::map<int, std::set<GUINode*> >& nodes);
		void UpdateNodeNames(std::map<int, std::set<GUINode*> >& nodes);

		GUINode* FindNodeInMap(std::map<int, std::set<GUINode*> >& nodes, const std::string& name);
		GUINode* PartialMatchFindNodeInMap(std::map<int, std::set<GUINode*> >& nodes, const std::string& name);

    protected:

        void UpdateScreenTransform();
};


#endif
