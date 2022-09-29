#include <RenderingDataTypes.h>
#include <RenderManager.h>
#include <globals.h>

//screen space quad
const glm::vec2 k_quad[4] = { vec2(-0.5,0.5), vec2(0.5,0.5), vec2(0.5,-0.5),vec2(-0.5,-0.5) };
const glm::vec2 k_screen[8] = { vec2(-1.0f,1.0f), vec2(0.0f,1.0f), 
							    vec2(1.0f,1.0f),  vec2(1.0f,1.0f),
							    vec2(1.0f,-1.0f), vec2(1.0f,0.0f),
							    vec2(-1.0f,-1.0f),vec2(0.0f,0.0f) };
//convenient array access
const float* quad_array = &k_quad[0].x;
const float* k_screen_array = &k_screen[0].x;


RenderingData::RenderingData()
:_target_texture(0),_shader(0),_vao(0),_vbo(0),_fbo(0)
{}

void RenderingData::UnloadData()
{
		glDeleteTextures(1, &_target_texture);
    //TODO: unload the rest
}

void RenderingData::CreateTexture()
{
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  
	if( _target_texture != 0 )
		glDeleteTextures(1, &_target_texture);

  glGenTextures(1,&_target_texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_target_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, _GL_colortype, Global::GameWindow->GetW(), Global::GameWindow->GetH(), 0, _GL_colortype, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _GL_filtertype);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _GL_filtertype);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_target_texture,0);
  
  glBindTexture(GL_TEXTURE_2D,0);
  glBindFramebuffer( GL_FRAMEBUFFER,0);
}

void RenderingData::GenData(unsigned shader, int GL_filtertype, int GL_colortype)
{
  _shader = shader;
  _GL_filtertype = GL_filtertype;
  _GL_colortype = GL_colortype;
  
  glGenBuffers(1,&_vbo);
  glBindBuffer(GL_ARRAY_BUFFER,_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, k_quad_array, GL_STATIC_DRAW);
  
  GenVAO();
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glGenFramebuffers(1,&_fbo);
  
  CreateTexture();
  
}
void RenderingData::GenVAO()
{
  glUseProgram(shader);          
  glGenVertexArrays(1,&_vao);
  glBindVertexArray(_vao);
  AssignVAOAttribPointers();
  glBindVertexArray(0);
  glUseProgram(0);
}
void RenderingData::Render()
{
  glDrawArrays( GL_QUADS,0,4 );
}
void RenderingData::Bind()
{
  glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
  glBindVertexArray( _vao );
  glUseProgram( _program );
}

void TileRenderingData::SetLayer(int layer)
{
  _layer = layer;
}
  
void TileRenderingData::Bind()
{
  RenderingData::Bind();
  
	const Camera& camera = *External::game->GetCurrentCamera();
	const Area& area = External::game->GetCurrentArea();
	const TileMap& map = area.GetTileMap();
	const TextureAtlas& atlas = map.GetTextureMappingAtlas();
	
  unsigned glu_map_size  = glGetUniformLocation(_program, "gridSize");
  unsigned glu_map_layer = glGetUniformLocation(_program, "pass");
  unsigned proj          = glGetUniformLocation(_program, "projectionMatrix");

  const float* projMat = camera.GetProjectionMatrix();
  glUniformMatrix4fv(proj, 1, 0, projMat);
  
  glUniform2f(glu_map_size, map.GetMapSize().x, map.GetMapSize().y);
  glUniform1i(glu_map_layer, _layer);
  
 const int active_tex[] = {
  GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4,
  GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9  };
  
  unsigned glu_tex0 = glGetUniformLocation(_program, "gridTexture");
  unsigned glu_tex1 = glGetUniformLocation(_program, "atlasTexture");
  unsigned glu_tex2 = glGetUniformLocation(_program, "atlasMapTexture");
  
  glUniform1i(glu_tex0, 0);
  glUniform1i(glu_tex0, 1);
  glUniform1i(glu_tex0, 2);

  glActiveTexture(active_tex[0]);
  glBindTexture(GL_TEXTURE_2D, map._tile_texture);
  glActiveTexture(active_tex[1]);
  glBindTexture(GL_TEXTURE_2D, atlas._atlas_texture);
  glActiveTexture(active_tex[2]);
  glBindTexture(GL_TEXTURE_1D, atlas._packed_data_texture);
}

void TileRenderingData::AssignVAOAttribPointers()
{
  unsigned pos_attrib = glGetAttribLocation(_program,"position");
  glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
  glEnableVertexAttribArray(pos_attrib);     
}





void SpriteRenderingData::Bind()
{
  RenderingData::Bind();
}
          
void SpriteRenderingData::AssignVAOAttribPointers()
{
  unsigned pos_attrib = glGetAttribLocation(_program,"position");
  glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
  glEnableVertexAttribArray(pos_attrib);     
}
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
 