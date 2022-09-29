#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <globals.h>
#include <PoolManager.h>
#include <TextureAtlas.h>
#include <Light.h>
#include <Renderable.h>
#include <RenderingDataTypes.h>

class Transform;
class GridInfo;
class Camera;
class BaseGame;

void PrintGLError(const std::string& label);

class RenderManager
{
		friend class RenderManagerInterface;
		friend class MainWindowInterface;
		friend class GameSystemInterface;
    friend class SystemInterface;
    friend class TextureAtlas;
		friend class Editor;

		typedef std::map<int, PoolManager<Renderable>* >::iterator layer_iter_t;

		//////////////////////////////////////////////////////////////////////
    //RenderManager.cpp
  public:
    RenderManager();
    ~RenderManager();

    void Init();
    void EnableGameRendering(bool state);

    void AddLight(const Light& light);
    void SetAmbientLight(const vec3& ambient_light);
    void SetAmbientLight(float r, float g, float b);
    vec3 GetAmbientLight() const;

    void MakeRenderable(GameObject* owner);

		//////////////////////////////////////////////////////////////////////
    //RenderManager_Setup.cpp
  private:
    bool CreateShaders(const std::string& shader_ini_file);
    void CreateScreenTextures();
    void CreateScreenSizedTexture(unsigned& out_texture, unsigned fbo, unsigned bind_index);
    void CreateFBOs();
    void CreateVBOs();
    void GenScreenData();

		//////////////////////////////////////////////////////////////////////
    //RenderManager_Rendering.cpp
		public:
				//Render functions
				void SetEnabled(bool state);
				void RenderMain(BaseGame* game);
				void PreRender();
				void RenderPostProcessing(const Camera& camera);
				//void RenderSprites(const Camera& camera, std::vector<RenderObject*>& objects);
                void RenderGrid(const Camera& camera, GridInfo& grid, unsigned pass);  //send in 0 or 1 only
				void ShowAtlasTextures(vec2 position, vec2 size, std::vector<unsigned> textures, TextureAtlas* atlas, unsigned selected_index);
				void DrawBox(vec2 bottom_left, vec2 top_right, vec4 color);
		private:
				void DrawScreenQuad();
                void TestTexture(unsigned texture);

				
		//////////////////////////////////////////////////////////////////////
				//RenderManager_RenderMain.cpp
		public:
				//New Rendering Functions
				void RenderMain();
		private:
				void ClearPostProcessBuffers();
				void RenderBackLayers();
				void RenderFrontLayers(layer_iter_t start_iter);
				bool RenderLayer(int layer);
				void RenderTileMap(int layer);
				void RenderLayer(PoolManager<Renderable>* layer);
				void RenderPostProcessing();


		//////////////////////////////////////////////////////////////////////
				//RenderManager_Loading.cpp
		public:
                TextureAtlas* LoadAtlas(const std::string& atlas_name);
                unsigned LoadTexture(const std::string& filename, bool store = true);
				void UnloadTexture(const std::string& filename);
		private:
                unsigned LoadImage(const std::string& filename);
                unsigned LoadBMP(const std::string& filename);

		//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////
				//Variables
        private:

				//Disable to stop all in-game rendering
				//editor, GUI system menus, and debug drawing will still work
				bool _enabled;

				///////////////////////////////////////
				// RenderLayers
				std::map<int, PoolManager<Renderable>* > _render_layers;
				std::vector<Light>					     _lights;
				vec3									 _ambient_light;
				//
				///////////////////////////////////////
				

				///////////////////////////////////////
				// Loaded texture data
				template <typename T>
				struct LoadedData{
					LoadedData():_ref_count(0){}
					unsigned _ref_count;
					T _data;
				};
                std::map<std::string,LoadedData<unsigned>>	_loadedTextures;
				//
				///////////////////////////////////////

				
				///////////////////////////////////////
				// Shaders
				struct Shader{
					Shader():_program(0),_vert(0),_frag(0),_has_error(false){}
          Shader(int p, int v, int f):_program(p),_vertex(v), _frag(f),_has_error(false){}
					~Shader(){}//TODO: write a delete function
					void CreateVertexShader(const std::string& filename);
					void CreateFragmentShader(const std::string& filename);
					void CreateProgram();

					unsigned ProgramID(){return _program;}
					unsigned VertexID(){return _vert;}
					unsigned FragmentID(){return _frag;}
					bool HasError(){return _has_error;}
					
					private:
					unsigned _program;
					unsigned _vert;
					unsigned _frag;
					bool	 _has_error;
				};
				std::map<std::string, Shader> _shaders;
				//
				///////////////////////////////////////
				
        TileRenderingData   _tile_rdata;
        SpriteRenderingData _sprite_rdata;

				//post process data
				unsigned _screen_texture;
				unsigned _screen_fbo;
        unsigned _screen_vbo;
        unsigned _screen_vao;
        
        unsigned _diffuse_tex;
        unsigned _tile_vao;
        unsigned _tile_vbo;
        
				unsigned _glow_texture;
				unsigned _light_fbo;

				//default shader names to load
				//these all must exist in Shaders.ini or nothing will render
				const std::string _k_tilemap_shader;
				const std::string _k_sprite_shader;
        const std::string _k_screen_shader;
};


#endif

