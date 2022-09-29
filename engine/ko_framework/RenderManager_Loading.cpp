#include <RenderManager.h>

#include <fstream>

#include <stdio.h>

#include <stb_image.c> //for loading images as textures



//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//private data
//////////////////////////////////////////////////////////////////////////////////////

bool CreateShader(const std::string& filename, unsigned handle);
bool LoadShaderFile(const std::string& filename, char** out_shaderStrings);
bool CompileShader(unsigned shaderHandle, char* shader_data);
bool LinkShaders(unsigned program, unsigned vertex, unsigned fragment);

void RenderManager::Shader::CreateVertexShader(const std::string& filename)
{
	_vert = glCreateShader(GL_VERTEX_SHADER);
	
    if(!CreateShader(filename, _vert))
		_has_error = true;
}

void RenderManager::Shader::CreateFragmentShader(const std::string& filename)
{
	_frag = glCreateShader(GL_FRAGMENT_SHADER);
	
    if(!CreateShader(filename, _frag))
		_has_error = true;
}

void RenderManager::Shader::CreateProgram()
{
	_program = glCreateProgram();
	
    if(!LinkShaders(_program, _vert, _frag))
		_has_error = true;
}



bool CreateShader(const std::string& filename, unsigned handle)
{
        char* shader_data = 0;
        if( !LoadShaderFile(filename, &shader_data) )
                return false;

        if(! CompileShader(handle, shader_data) )
        {
            std::cerr<<"Failed to compile shader file: "<<filename <<"\n";
            return false;
        }

        delete[] shader_data;
        return true;
}


bool LoadShaderFile(const std::string& filename, char** out_shaderStrings)
{
        if(*out_shaderStrings)
        {
                std::cerr<<"Error: output destination must be null.\n";
                return false;
        }

		std::string file_data;
        FILE* fp = 0;

        fp = fopen( filename.c_str(), "r" );

        if( !fp )
        {
                std::cerr<<"Error: shader file "<<filename <<" not found.\n";
                return false;
        }

        while( !feof(fp) )
        {
                char c = fgetc(fp);
                file_data.push_back(c);
        }

        const unsigned data_size = file_data.size();
        *out_shaderStrings = new char[ data_size ];
        memset( *out_shaderStrings, 0, data_size );
        memcpy( *out_shaderStrings, file_data.c_str(), sizeof(char)*data_size );
        (*out_shaderStrings)[data_size-1] = '\0';

        return true;

#if 0//old way, don't use this. doesn't work on all computers
        std::fstream file;
        file.open( filename.c_str(), std::ios::in );
        if(!file.is_open())
        {
                std::cerr<<"Error: shader file "<<filename <<" not found.\n";
                return 0;
        }

        std::string file_data;
        while( file.good() )
        {
                char c = file.get();
                file_data.push_back(c);
        }

        const unsigned data_size = file_data.size();
        *out_shaderStrings = new char[ data_size ];
        std::string::iterator iter = file_data.begin();
        for(unsigned i = 0; i < data_size && iter != file_data.end(); ++i)
        {
                (*out_shaderStrings)[i] = *iter;
                ++iter;
        }
        return 1;
#endif
}

bool CompileShader(unsigned shaderHandle, char* shader_data)
{
        glShaderSource(shaderHandle, 1, const_cast<const GLchar**>(&shader_data), 0);
        glCompileShader(shaderHandle);
        int status = 0;
        int logLength = 1;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
        if( logLength > 1 ) // 0 log length = no errors on ATI, 1 = no errors on NVIDIA
        {
                std::cerr<<"\nCompile Error:\n";
                char error_log[1024];
                glGetShaderInfoLog(shaderHandle, 1024, 0, error_log );
                std::cerr<<error_log<<"\n";

                if(status != GL_TRUE)
                {
                        std::cerr<<"Compile Failed.\n";
                        delete[] shader_data;
                        return false;
                }
        }
        return true;


#if 0//old way, don't use this. doesn't work on all computers
        glShaderSource(shaderHandle, 1, const_cast<const GLchar**>(&shader_data), 0);
        glCompileShader(shaderHandle);

        int logLength = 0;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
        if( logLength > 0 )
        {
                char error_log[1024];
                glGetShaderInfoLog(shaderHandle, 1024, 0, error_log );
                std::cerr<<"\n"<<error_log;
                delete[] shader_data;
                return 0;
        }
        return 1;
#endif
}

bool LinkShaders(unsigned program, unsigned vertex, unsigned fragment)
{
        if( vertex != 0 )
                glAttachShader( program, vertex );
        glAttachShader( program, fragment );

        glLinkProgram( program );

        int status = 0;
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if( logLength > 1 )
        {
                std::cerr<<"\nLinker Error:\n";
                char error_log[1024];
                glGetProgramInfoLog(program, 1024, 0, error_log );
                std::cerr<<error_log<<"\n";
                if(status != GL_TRUE)
                {
                        std::cerr<<"Link Failed.\n";
                        return false;
                }
        }
        return true;

#if 0//old way, don't use this. doesn't work on all computers
        if( vertex != 0 )
                glAttachShader( program, vertex );
        glAttachShader( program, fragment );

        glLinkProgram( program );

        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if( logLength > 0 )
        {
                char error_log[1024];
                glGetProgramInfoLog(program, 1024, 0, error_log );
                std::cerr<<"\n"<<error_log;
                return 0;
        }
        return 1;
#endif
}

unsigned RenderManager::LoadBMP(const std::string& filename)
{
//SDL HAS BEEN REMOVED
#if 0
    SDL_Surface* data = SDL_LoadBMP( filename.c_str() );
    if(!data)
    {
            std::cerr<<"Error loading texture file: "<<filename <<"\n";
            return 0;
    }

    const unsigned pixelsize = 3;
    if( data->format->BytesPerPixel != pixelsize )
    {
            std::cerr<<"Error: must be a 24-bits-per-pixel bitmap. Load failed\n";
            SDL_FreeSurface(data);
            return 0;
    }

    if( SDL_MUSTLOCK( data ) )
    {
            SDL_LockSurface( data );
    }

    unsigned texture = 0;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, data->w, data->h, 0, GL_BGR, GL_UNSIGNED_BYTE, data->pixels );

    if( SDL_MUSTLOCK( data ) )
    {
            SDL_UnlockSurface( data );
    }

    SDL_FreeSurface(data);
    return texture;
#endif
    return 0;
}


unsigned RenderManager::LoadImage(const std::string& filename)
{
    int w,h,components;
    unsigned char *data = stbi_load(filename.c_str(), &w, &h, &components, 0);

    GLenum format = GL_RGBA;
	if( Global::GetExtension(filename) == "jpg" )
        format = GL_RGB;

    unsigned texture = 0;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, components, w, h, 0, format, GL_UNSIGNED_BYTE, data );

    stbi_image_free(data);

    return texture;
}

TextureAtlas* RenderManager::LoadAtlas(const std::string& atlas_name)
{
	TextureAtlas* new_atlas = new TextureAtlas();
	if( !new_atlas->Load(atlas_name) )
	{
		std::cerr<<"Error loading texture atlas mapping file "<<atlas_name <<"\n";
		delete new_atlas;
		return 0;
	}

	return new_atlas;
}

unsigned RenderManager::LoadTexture(const std::string& filename, bool store)
{
    if( _loadedTextures.find( filename ) != _loadedTextures.end() )
	{
		_loadedTextures[ filename ]._ref_count++;
        return _loadedTextures[ filename ]._data;//already loaded
	}
    
    unsigned texture = 0;

    //if( GetExtension(filename) == "bmp" ) //TODO: fix loading bmps now the SDL is removed
    //    texture = LoadBMP(filename);
    //else
        texture = LoadImage(filename);

    if(texture == 0)
    {
		std::cerr << "LoadImage failed! Aborting texture creation with filename: "<<filename <<"\n";
        glBindTexture( GL_TEXTURE_2D, 0 );
        return 0;
    }

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    if(store)
	{
		LoadedData<unsigned> tex;
		tex._data = texture;
		tex._ref_count = 1;
        _loadedTextures[ filename ] = tex;
	}

    glBindTexture( GL_TEXTURE_2D, 0 );

    return texture;
}

void RenderManager::UnloadTexture(const std::string& filename)
{
    if( _loadedTextures.find( filename ) != _loadedTextures.end() )
	{
		if( _loadedTextures[ filename ]._ref_count > 0 )
			_loadedTextures[ filename ]._ref_count--;
		
		if( _loadedTextures[ filename ]._ref_count == 0 )
		{
			glDeleteTextures(1, &_loadedTextures[ filename ]._data); 
			_loadedTextures.erase( _loadedTextures.find( filename ) );
		}
	}
}



