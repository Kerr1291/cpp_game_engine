#include <RenderManager.h>

#include <GameObject.h>

#include <Light.h>


//Helper function for printing opengl errors
void PrintGLError(const std::string& label)
{
	std::cerr<<"checking for errors at: "<<label <<" ";
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR) {
            errString = gluErrorString(errCode);
       fprintf (stderr, "OpenGL Error: %s\n", errString);
    }
    else
       std::cerr<<" ...no errors found\n";
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//RenderManager Class//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


RenderManager::RenderManager()
:_enabled(true)
,_ambient_light(vec3(1,1,1))
,_screen_texture(0)
,_screen_fbo(0)
,_glow_texture(0)
,_light_fbo(0)
,_k_tilemap_shader("TileMapRendering")
,_k_sprite_shader("SpriteRendering")
{
	CreateFBOs();
	CreateScreenTextures();

	//TODO: Grab this file from the config.ini
    CreateShaders("./ko/Shaders/Shaders.ini");
}

RenderManager::~RenderManager()
{
	//TODO: free all the resources!
}

void RenderManager::Init()
{
	//default render layers that are always expected.
	//these are necessary for the map drawing to work correctly.
	//if they don't exist the map will not be drawn
	_render_layers[-1] = new PoolManager<Renderable>();
	_render_layers[0] = new PoolManager<Renderable>();
	_render_layers[1] = new PoolManager<Renderable>();
	_render_layers[2] = new PoolManager<Renderable>();
  
  _tile_rdata.GenData(_shaders[_k_tilemap_shader].ProgramID(), GL_LINEAR, GL_RGBA);
  _sprite_rdata.GenData(_shaders[_k_sprite_shader].ProgramID(), GL_LINEAR, GL_RGBA);
}

void RenderManager::EnableGameRendering(bool state)
{
	_enabled = state;
}

void RenderManager::AddLight(const Light& light)
{
	_lights.push_back(light);
}

void RenderManager::SetAmbientLight(const vec3& ambient_light)
{
	_ambient_light = ambient_light;
}

void RenderManager::SetAmbientLight(float r, float g, float b)
{
	SetAmbientLight( vec3(r,g,b) );
}

vec3 RenderManager::GetAmbientLight() const
{
	return _ambient_light;
}

void RenderManager::MakeRenderable(GameObject* owner)
{
	if(!owner)
		return;

	const int render_layer = owner->_render_layer;
	
	std::map< int,PoolManager<Renderable>* >::iterator insert_layer;

	if( _render_layers.count( render_layer ) == 0 )
	{
		_render_layers[render_layer] = new PoolManager<Renderable>();
	}

	insert_layer = _render_layers.find( render_layer );

	_render_layers[render_layer]->MakeComponent( owner );
}