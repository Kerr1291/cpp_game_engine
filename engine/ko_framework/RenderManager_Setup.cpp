#include <RenderManager.h>
#include <DataFile.h>
#include <DataFileIterator.h>

//screen space quad
const glm::vec2 k_quad[4] = { vec2(-0.5,0.5), vec2(0.5,0.5), vec2(0.5,-0.5),vec2(-0.5,-0.5) };
const glm::vec2 k_screen[8] = { vec2(-1.0f,1.0f), vec2(0.0f,1.0f), 
							    vec2(1.0f,1.0f),  vec2(1.0f,1.0f),
							    vec2(1.0f,-1.0f), vec2(1.0f,0.0f),
							    vec2(-1.0f,-1.0f),vec2(0.0f,0.0f) };
//convenient array access
const float* quad_array = &k_quad[0].x;
const float* k_screen_array = &k_screen[0].x;

///////////////////////////////////////////////////
// shaders for rendering a screen space quad
// Pretty basic so I included them here
// TODO: finalize sprite and map shaders
//       and place them here too.
const char* k_scn_v_shader[1] = { "\n\
#version 150 \n\
in vec2 position; \n\
in vec2 texuv; \n\
out vec2 _texuv; \n\
void main() \n\
{ \n\
  _texuv = texuv;\n\
  gl_Position = vec4(position,0.0,1.0); \n\
}\n\
" };

const char* k_scn_f_shader[1] = { "\n\
#version 150 \n\
in vec2 _texuv; \n\
out vec4 outColor;\n\
uniform sampler2D tex_fbo;\n\
void main()\n\
{\n\
  outColor = texture(tex_fbo,_texuv);\n\
}\n\
" };
//
///////////////////////////////////////////////////

/*
Shaders
{
	GridRendering
	{
		Vertex = "./ko/Shaders/gridrender.vert"
		Fragment = "./ko/Shaders/gridrender.frag"
	}
	SpritRendering
	{
		Vertex = "./ko/Shaders/spriterender.vert"
		Fragment = "./ko/Shaders/spriterender.frag"
	}
}
*/

bool RenderManager::CreateShaders(const std::string& shader_ini_file)
{
	DataFile shader_file;
	shader_file.Load(shader_ini_file);

	if( !shader_file.HasObject("Shaders") )
	{
		std::cerr<<"Shader file "<<shader_ini_file <<" does not contain a 'Shaders' object and so is not a valid shader ini file.\n";
		return false;
	}

	SerializedObject shaders;
	shader_file.GetFileObject("Shaders", shaders);

	bool all_success = true;

	ObjectIterator<SerializedObject> iter(shaders);
	for(; iter.Good(); iter.Next())
	{
		SerializedObject& current_shader_data = iter.Get();
		const std::string& name = current_shader_data.GetName();

		if( _shaders.find( name ) != _shaders.end() )
		{
			std::cerr<<"Shader "<<name <<" already exists in "<<shader_ini_file <<". Shaders skipped.\n";
			continue;
		}

		Shader new_shader;
		if( current_shader_data.HasValue("Vertex") )
		{
			std::string vertex_filename = "";
			current_shader_data.GetValue("Vertex",vertex_filename);
			new_shader.CreateVertexShader(vertex_filename);
		}
		
		if( current_shader_data.HasValue("Fragment") )
		{
			std::string fragment_filename = "";
			current_shader_data.GetValue("Fragment",fragment_filename);
			new_shader.CreateVertexShader(fragment_filename);
		}
		
		if( new_shader.HasError() || new_shader.FragmentID() == 0 )
		{
			std::cerr<<"Error compiling shader program '"<<name <<"'\n";
			all_success = false;
			continue;
		}
			
		new_shader.CreateProgram();
		
		if( new_shader.HasError() )
		{
			std::cerr<<"Error linking shader program '"<<name <<"'\n";
			all_success = false;
			continue;
		}
		_shaders[ name ] = new_shader;
	}

	return all_success;
	/*
	//TODO: move all of this into a shader ini file

		_grid.CreateVertexShader("./ko/Shaders/gridrender.vert");
		_grid.CreateFragmentShader("./ko/Shaders/gridrender.vert");
		_grid.CreateProgram();
		
		_grid.CreateVertexShader("./ko/Shaders/gridrender.vert");
		_grid.CreateFragmentShader("./ko/Shaders/gridrender.vert");
		_grid.CreateProgram();

		_sprite;
		_atlas_gui;
		_post_process_light_pass;
		_post_process_blur_pass;
		_post_process_final_pass;


        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        basicProgram = glCreateProgram();

        gridVertexShader = glCreateShader(GL_VERTEX_SHADER);
        gridFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        gridProgram = glCreateProgram();

        spriteVertexShader = glCreateShader(GL_VERTEX_SHADER);
        spriteFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        spriteProgram = glCreateProgram();

        atlasGUIVertexShader = glCreateShader(GL_VERTEX_SHADER);
        atlasGUIFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        atlasGUIProgram = glCreateProgram();
	
        postProcess_lightPass_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        postProcess_lightPass_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        postProcess_lightPass_Program = glCreateProgram();

        postProcess_blurPass_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        postProcess_blurPass_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        postProcess_blurPass_Program = glCreateProgram();

        postProcess_finalPass_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        postProcess_finalPass_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        postProcess_finalPass_Program = glCreateProgram();
		
		// create basic shaders to render simple geometry

        if(!CreateShader("./ko/Shaders/basic.vert", vertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/basic.frag", fragmentShader))
                return 0;

        if(!LinkShaders(basicProgram, vertexShader, fragmentShader))
                return 0;

		//create the shader to render the tile maps

        if(!CreateShader("./ko/Shaders/gridrender.vert", gridVertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/gridrender.frag", gridFragmentShader))
                return 0;

        if(!LinkShaders(gridProgram, gridVertexShader, gridFragmentShader))
                return 0;

		//create the sprite shader

        if(!CreateShader("./ko/Shaders/spriterender.vert", spriteVertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/spriterender.frag", spriteFragmentShader))
                return 0;

        if(!LinkShaders(spriteProgram, spriteVertexShader, spriteFragmentShader))
                return 0;

		//create the shader used to render sprites from an atlas on the screen

        if(!CreateShader("./ko/Shaders/atlasgui.vert", atlasGUIVertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/atlasgui.frag", atlasGUIFragmentShader))
                return 0;

        if(!LinkShaders(atlasGUIProgram, atlasGUIVertexShader, atlasGUIFragmentShader))
                return 0;
		
		//create the shaders used in post processing
		
			//light shader
        if(!CreateShader("./ko/Shaders/postProcess_lightPass.vert", postProcess_lightPass_VertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/postProcess_lightPass.frag", postProcess_lightPass_FragmentShader))
                return 0;

        if(!LinkShaders(postProcess_lightPass_Program, postProcess_lightPass_VertexShader, postProcess_lightPass_FragmentShader))
                return 0;

			//blur shader
        if(!CreateShader("./ko/Shaders/postProcess_blurPass.vert", postProcess_blurPass_VertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/postProcess_blurPass.frag", postProcess_blurPass_FragmentShader))
                return 0;

        if(!LinkShaders(postProcess_blurPass_Program, postProcess_blurPass_VertexShader, postProcess_blurPass_FragmentShader))
                return 0;

			//final pass/combine shader
        if(!CreateShader("./ko/Shaders/postProcess_finalPass.vert", postProcess_finalPass_VertexShader))
                return 0;

        if(!CreateShader("./ko/Shaders/postProcess_finalPass.frag", postProcess_finalPass_FragmentShader))
                return 0;

        if(!LinkShaders(postProcess_finalPass_Program, postProcess_finalPass_VertexShader, postProcess_finalPass_FragmentShader))
                return 0;

        return 1;
		*/
}


void RenderManager::CreateScreenTextures()
{
	//CreateScreenSizedTexture(_screen_texture,_screen_fbo,0);

	/*//TODO: make sure the replacement function above works works before removing this
	glBindFramebuffer(GL_FRAMEBUFFER, _screen_fbo);

	if( _screen_texture != 0 )
		glDeleteTextures(1, &_screen_texture);

    glGenTextures(1, &_screen_texture);
    glBindTexture(GL_TEXTURE_2D, _screen_texture);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Global::GameWindow->GetW(), Global::GameWindow->GetH(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
	
	// bind screen texture to 0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _screen_texture, 0);
	*/
  
  /*

	//////////////////////////////////////
	// glow render texture
	if( _glow_texture != 0 )
		glDeleteTextures(1, &_glow_texture);

    glGenTextures(1, &_glow_texture);
    glBindTexture(GL_TEXTURE_2D, _glow_texture);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Global::GameWindow->GetW(), Global::GameWindow->GetH(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	//glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Global::GameWindow->GetW(), Global::GameWindow->GetH(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
	
	// bind glow texture to 1?
	// glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, glowTexture, 0);

	GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
	GLenum lightDrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

	glDrawBuffers(1, lightDrawBuffers);
	//glDrawBuffers(2, DrawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, _light_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _glow_texture, 0);
	glDrawBuffers(1, lightDrawBuffers);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  */
}

void RenderManager::CreateScreenSizedTexture(unsigned& out_texture, unsigned fbo, unsigned bind_index)			
{
/*
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if( out_texture != 0 )
		glDeleteTextures(1, &out_texture);

    glGenTextures(1, &out_texture);
    glBindTexture(GL_TEXTURE_2D, out_texture);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Global::GameWindow->GetW(), Global::GameWindow->GetH(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
	
	// bind screen texture to 0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _screen_texture, bind_index);
  */
}
				
void RenderManager::CreateFBOs()
{
	//glGenFramebuffers(1, &_screen_fbo);
	//glGenFramebuffers(1, &_light_fbo);
}
void RenderManager::CreateVBOs()
{
  //GenScreenData();
}

void RenderManager::GenScreenData()
{

/*
  glGenBuffers(1,&_screen_vbo);
  glBindBuffer(GL_ARRAY_BUFFER,_screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(float)*8, k_screen_array, GL_STATIC_DRAW);
  
  unsigned v = glCreateShader(GL_VERTEX_SHADER);
  unsigned f = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(v,1,(k_scn_v_shader),NULL);
  glCompileShader(v);
  
  //{ std::cout<<"VertexLog:\n";
  //char buffer[512];
  //glGetShaderInfoLog(v, 512, NULL, buffer);
  //std::cout<<buffer <<"\n";
  //}

  glShaderSource(f,1,(k_scn_f_shader),NULL);
  glCompileShader(f);
  
  //{std::cout<<"FragmentLog:\n";
  //char buffer[512];
  //glGetShaderInfoLog(v, 512, NULL, buffer);
  //std::cout<<buffer <<"\n";
  //}
  
  unsigned scn_program = glCreateProgram();
  glAttachShader(scn_program, v);
  glAttachShader(scn_program, f);
  
  glBindFragDataLocation(scn_program, 0, "outColor");
  glLinkProgram(scn_program);
  glUseProgram(scn_program);
  
  glGenVertexArrays(1,&_screen_vao);
  glBindVertexArray(_screen_vao);
  
  //bind the 'in' vertex attributes to our vao
  //the first sizeof is the size of the whole vertex (stride)
  //the 2nd sizeof (seen in the uv_attrib call) is the starting point of the first element
  unsigned pos_attrib = glGetAttribLocation(scn_program,"position");
  glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
  glEnableVertexAttribArray(pos_attrib);
  
  unsigned uv_attrib = glGetAttribLocation(scn_program,"texuv");
  glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));
  glEnableVertexAttribArray(uv_attrib);
  
  _shaders[_k_screen_shader] = Shader(scn_program,v,f);
  
  glBindVertexArray(0);
  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  */
}


