#ifndef __RENDERING_DATA_TYPES_H
#define __RENDERING_DATA_TYPES_H

class RenderingData
{
  friend class RenderManager;
  
  RenderingData();
  
  void UnloadData();
  void GenData(unsigned shader, int GL_filtertype, int GL_colortype);
  void Render();
  
  void GenVAO();
  void CreateTexture();
  
  virtual void Bind();
  virtual void AssignVAOAttribPointers() = 0;
  
  void Bind4x4Matrix(const float* mat);

  unsigned _target_texture;
  unsigned _shader;
  unsigned _vao;
  unsigned _vbo;
  unsigned _fbo;
  
  int _GL_filtertype;
  int _GL_colortype;
};

class TileRenderingData : public RenderingData
{
  friend class RenderManager;
  virtual void Bind();
  void AssignVAOAttribPointers();
  
  void SetLayer(int layer);
  int _layer;
};

class SpriteRenderingData : public RenderingData
{
  friend class RenderManager;
  virtual void Bind();
  void AssignVAOAttribPointers();
};














#endif



















