#include <RenderManager.h>

#include <GameObject.h>
#include <Camera.h>
#include <TileMap.h>
#include <BaseGame.h>
#include <Area.h>


//////////////////////////////
// helper functions here will be called often
// optimize for speed

//Helper function, called very frequently.
//placed here for maximum speed
//This version of IsNearCamera was optimized by a profiler for maximum speed
inline bool IsNearCamera(const vec2& pos, const Camera& c)
{
    const float& r = c.GetRightEdge();
    const float p_x = glm::abs( pos.x - c.GetViewCenterXRef() );

    if( p_x - r > 0 )
        return false;
        
    const float& t = c.GetTopEdge();
    const float p_y = glm::abs( pos.y - c.GetViewCenterYRef() );
    
    if( p_y - t > 0 )
        return false;

    return true;
}

inline void ClearFBO(const unsigned& fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

inline void BindMainFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// end local helpers
////////////////////////////////////////

void RenderManager::RenderMain()
{
	if( !_enabled )
		return;

	//need a camera to render the game
	Camera* camera = External::game->GetCurrentCamera();
	if( !camera )
		return;

	ClearPostProcessBuffers();
	RenderBackLayers();

	layer_iter_t front_start;
	const int k_map_layers = 4;
	int front_layer_start = -1;
	for( int i = 0; i < k_map_layers; ++i )
	{
		RenderTileMap(i);
		if( RenderLayer(i) )
			front_layer_start = i;
	}

	if( front_layer_start == -1 )
		front_start = _render_layers.begin();
	else
		front_start = _render_layers.find( front_layer_start );

	RenderPostProcessing();
}

void RenderManager::ClearPostProcessBuffers()
{
  ClearFBO( _screen_fbo );
  ClearFBO( _light_fbo );
  BindMainFBO();
}

void RenderManager::RenderBackLayers()
{
	std::map<int, PoolManager<Renderable>* >::iterator layer;
	for( layer = _render_layers.begin(); layer != _render_layers.end(); ++layer )
	{
		int current_layer =	layer->first;
		if( current_layer > -1 )
			return;

		RenderLayer( layer->second );
	}
}

void RenderManager::RenderFrontLayers(layer_iter_t start_iter)
{
	std::map<int, PoolManager<Renderable>* >::iterator layer = start_iter;
	
	if( layer != _render_layers.begin() && layer != _render_layers.end() )
		++layer;

	for(; layer != _render_layers.end(); ++layer )
	{
		int current_layer =	layer->first;
		if( current_layer < 4 )
			continue;

		RenderLayer( layer->second );
	}
}

bool RenderManager::RenderLayer(int layer)
{
	if( _render_layers.count( layer ) > 0 )
	{
		RenderLayer( _render_layers[layer] );
		return true;
	}
	return false;
}

//functions below here will never be called if called if camera is null	
//so the dereference of camera is safe

/////////////////////////////////
// Render helper functions


void RenderManager::PrepareGLRender()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
}

inline void SetBlending(const bool& enable)
{
  if( enable )
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation( GL_FUNC_ADD );
  }
  else
  {
    glDisable(GL_BLEND);
  }
}

void RenderManager::PrepareTileRender(unsigned layer)
{
  SetBlending(true);
  _tile_rdata.Bind();
  _tile_rdata.SetLayer(layer);
}

void RenderManager::PrepareSpriteRender()
{
  SetBlending(true);
  _sprite_rdata.Bind();
  //TODO: set up the necessary sprite rendering fields
}
//
/////////////////////////////////


void RenderManager::RenderTileMap(int layer)
{
  PrepareGLRender();
  PrepareTileRender(layer);
  _tile_rdata.Render();
}


void RenderManager::RenderLayer(PoolManager<Renderable>* layer)
{
	//TODO: use iterator to render objects in layer
	const Camera& camera = *External::game->GetCurrentCamera();
	
  _sprite_rdata.Render();
}

void RenderManager::RenderPostProcessing()
{
	const Camera& camera = *External::game->GetCurrentCamera();
  
  //TODO: create post processing render layers and call them here
}