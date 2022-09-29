#include <Physics.h>


////////////////////////////////////////////////////////////////////////////////
// Helpers for the collision map optimize function

truth b2Vec2IsEqual( b2Vec2 a, b2Vec2 b )
{
    if( glm::abs( a.x - b.x ) <= Global::TINY
     && glm::abs( a.y - b.y ) <= Global::TINY )
        return 1;

    return 0;
}

/*
//not used?
truth b2Vec2AreAxisAlignedCollinear( b2Vec2 a, b2Vec2 b, b2Vec2 c )
{
    if( glm::abs( a.x - b.x ) <= Global::TINY
     && glm::abs( b.x - c.x ) <= Global::TINY )
        return 1;

    if( glm::abs( a.y - b.y ) <= Global::TINY
     && glm::abs( b.y - c.y ) <= Global::TINY )
        return 1;

	return 0;
}
*/

void GetVertexList( std::vector< b2vec2 >& out_verts, b2Body* b )
{
	const b2Fixture* fixture = b->GetFixtureList();

	if( dynamic_cast<const b2PolygonShape*>( fixture[0].GetShape() ) )
	{
		const b2PolygonShape* shape = dynamic_cast<const b2PolygonShape*>( fixture[0].GetShape() );

        for( unsigned i = 0; i < shape->GetVertexCount(); ++i )
        {
			out_verts.push_back( shape->GetVertex(i) );
		}
	}
	else if( dynamic_cast<const b2EdgeShape*>( fixture[0].GetShape() ) )
	{
		for( b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext() )
		{
			const b2EdgeShape* shape = dynamic_cast<const b2EdgeShape*>( f->GetShape() );
			if( !shape ) //something weird... just move on for now
				continue;

			out_verts.push_back( shape->m_vertex1 );
			out_verts.push_back( shape->m_vertex2 );
		}
	}
}

bool ShareVerts(b2Body* a, b2Body* b)
{
	if( !a || !b )
		return false;

	std::vector< b2vec2> verts_a;
	std::vector< b2vec2> verts_b;
	GetVertexList(verts_a, a);
	GetVertexList(verts_b, b);

	unsigned num_shared = 0;

	for( unsigned i = 0; i < verts_a.size(); ++i )
	{
		for( unsigned j = 0; j < verts_b.size(); ++j )
		{
			if( b2Vec2IsEqual( verts_a[i], verts_b[j] ) )
				++num_shared;

			if( num_shared >= 2 )
				return true;
		}
	}

	return false;
}

void DoComparison(std::set<unsigned>& out_redundant_set, std::vector< b2vec2 >& verts, unsigned i, unsigned j)
{
	bool should_add = false;

	if( b2Vec2IsEqual( verts[i], verts[j] ) )
	{
		if( b2Vec2IsEqual( verts[i+1], verts[j+1] ) )
		{
			should_add = true;
		}
	}

	if( b2Vec2IsEqual( verts[i], verts[j+1] ) )
	{
		if( b2Vec2IsEqual( verts[i+1], verts[j] ) )
		{
			should_add = true;
		}
	}

	if( should_add )
	{
		out_redundant_set.insert( i );
		out_redundant_set.insert( i+1 );
		out_redundant_set.insert( j );
		out_redundant_set.insert( j+1 );
	}
}

void RemoveSameEdges(std::vector< b2vec2 >& out_verts, unsigned a_size)
{
	if( out_verts.size() % 2 )
		return;

	std::set<unsigned> redundant_set;
	for( unsigned i = 0; i < a_size-1; i+=2 )
	{
		for( unsigned j = a_size; j < out_verts.size()-1; j+=2 )
		{
			DoComparison(redundant_set, out_verts, i, j);
		}
	}

	std::set<unsigned>::reverse_iterator r_iter = redundant_set.rbegin();
	for( ; r_iter != redundant_set.rend(); ++r_iter )
	{
		out_verts.erase( out_verts.begin()+(*r_iter) );
	}
}

void CreateBoundry(b2Body* boundry, std::vector< b2vec2 >& verts)
{
    for(unsigned j = 0; j < verts.size()-1; j+=2 )
    {
        b2EdgeShape shape;
        shape.Set(verts[j], verts[j+1]);
        boundry->CreateFixture(&shape, 0.0f);
    }
}

bool PhysicsManager::IsEdgeShape(b2Body* b)
{
	const b2Fixture* fixture = b->GetFixtureList();

	if( dynamic_cast<const b2EdgeShape*>( fixture[0].GetShape() ) )
		return true;

	return false;
}

bool PhysicsManager::IsPolygonShape(b2Body* b)
{
	const b2Fixture* fixture = b->GetFixtureList();

	if( !fixture )
		return false;

	if( dynamic_cast<const b2PolygonShape*>( fixture[0].GetShape() ) )
		return true;

	return false;
}

void ConvertToEdgeShapeVerts(std::vector< b2vec2 >& out_verts, std::vector< b2vec2 >& in_verts)
{
    for(unsigned j = 1; j < in_verts.size(); ++j )
    {
        out_verts.push_back(in_verts[j-1]);
		out_verts.push_back(in_verts[j]);
    }
	{//close the loop
        out_verts.push_back(in_verts[in_verts.size()-1]);
		out_verts.push_back(in_verts[0]);
	}
}


// end helpers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




b2Body* PhysicsManager::CombineBodies(b2Body* a, b2Body* b)
{
	if( a->GetType() != b2_staticBody || b->GetType() != b2_staticBody )
		return 0;

	if( IsPolygonShape(a) == 0 && IsEdgeShape(a) == 0 )
		return 0;

	if( IsPolygonShape(b) == 0 && IsEdgeShape(b) == 0 )
		return 0;

    b2Body* body_a = a;
    b2Body* body_b = b;

	b2BodyDef bd;
    b2Body* new_body = world->CreateBody(&bd);

	std::vector< b2vec2 > verts;

	std::vector< b2vec2 > verts_a;
	std::vector< b2vec2 > verts_b;
	GetVertexList(verts_a, body_a);
	GetVertexList(verts_b, body_b);

	//first grab all the vertices into one set
	if( IsEdgeShape( body_a ) )
		verts = verts_a;
	else
		ConvertToEdgeShapeVerts( verts, verts_a );

	unsigned a_size = verts.size();

	if( IsEdgeShape( body_b ) )
		for( unsigned i = 0; i < verts_b.size(); ++i )  verts.push_back( verts_b[i] );
	else
		ConvertToEdgeShapeVerts( verts, verts_b );

	//remove edges that are the same
	RemoveSameEdges( verts, a_size );

	//remove the points that share a line
	//RemoveAxisAlignedCollinearPoints( verts );

	//use what remains to make the boundry
	CreateBoundry(new_body, verts);

    world->DestroyBody(body_a);
    world->DestroyBody(body_b);

	return new_body;
}