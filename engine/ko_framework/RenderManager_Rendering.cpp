#include <RenderManager.h>
#include <Light.h>
#include <GridInfo.h>
#include <Sprite.h>
#include <Parser.h>
#include <Transform.h>
#include <RenderObject.h>
#include <Camera.h>
#include <GridInfo.h>
#include <BaseGame.h>
#include <Area.h>


//Helper function, called very frequently.
//placed here for maximum speed
inline truth IsNearCamera(const vec2& pos, const Camera& c)
{
	const vec2& c_ext = c.GetCameraExtents();

    const float r = glm::abs( c_ext.x - c.GetRefX() );
    const float t = glm::abs( c_ext.y - c.GetRefY() );
    const float p_x = glm::abs( pos.x - c.GetRefX() );
    const float p_y = glm::abs( pos.y - c.GetRefY() );

    if( p_x - r > 0 )
        return 0;
    if( p_y - t > 0 )
        return 0;

    return 1;
}



void RenderManager::SetEnabled(bool state)
{
	_enabled = state;
}

//Main render function, calls other rendering steps in necessary order
void RenderManager::RenderMain(BaseGame* game)
{
	if( !_enabled )
		return;

	Camera* camera = game->GetCurrentCamera();
	if( !camera )
		return;

	PreRender();

	GridInfo* tiles = 0;
	Area* area = game->GetCurrentArea();
	if( area )
		tiles = area->GetGrid();
	
	if( tiles )
		RenderGrid( *camera, *tiles, 0 );

	RenderSprites( *camera, renderObjects );

    if( tiles )
		RenderGrid( *camera, *tiles, 1 );

	RenderPostProcessing( *camera );
}

void RenderManager::PreRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _screen_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, _light_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderManager::RenderPostProcessing(const Camera& camera)
{
//TODO:::: CLEAN THIS UP TO USE THE NEW SHADER MAP
#if 0

	glBindFramebuffer(GL_FRAMEBUFFER, _light_fbo);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, glowTexture, 0);
	if( _lights.size() > 0 )
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);

		{ //light pass
			glUseProgram( postProcess_lightPass_Program );
			unsigned gt_ID = glGetUniformLocation(postProcess_lightPass_Program, "glowTexture");

			unsigned lightPos_ID = glGetUniformLocation(postProcess_lightPass_Program, "lightPos");
			unsigned lightColor_ID = glGetUniformLocation(postProcess_lightPass_Program, "lightColor");
			unsigned lightRadius_ID = glGetUniformLocation(postProcess_lightPass_Program, "lightRadius");
			unsigned lightIntensity_ID = glGetUniformLocation(postProcess_lightPass_Program, "lightIntensity");
			unsigned lightZoom_ID = glGetUniformLocation(postProcess_lightPass_Program, "lightZoom");
			unsigned lightDirection_ID = glGetUniformLocation(postProcess_lightPass_Program, "lightDirection");

			unsigned screenHeight_ID = glGetUniformLocation(postProcess_lightPass_Program, "screenHeight");
			unsigned screenWidth_ID = glGetUniformLocation(postProcess_lightPass_Program, "screenWidth");

			unsigned proj = glGetUniformLocation(postProcess_lightPass_Program, "projectionMatrix");

			unsigned pass_ID = glGetUniformLocation(postProcess_lightPass_Program, "pass");

			glUniform1i(gt_ID, 0);
			glUniform1f( screenHeight_ID, (float)Global::GameWindow->GetH() );
			glUniform1f( screenWidth_ID, (float)Global::GameWindow->GetW() );

			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, glowTexture );

			const float* __restrict__ projMat = camera.GetProjectionMatrix();
			glUniformMatrix4fv(proj, 1, 0, projMat);

			glUniform1f( pass_ID, 1.0 );
			glUniform1f( lightZoom_ID, camera.GetWindowZoom() ); //window zoom, needed to properly render lights at different zoom distances

			for( unsigned i = 0; i < lights.size(); ++i)
			{
			    const Light* __restrict__ currentLight = lights[i];

				if( IsNearCamera( currentLight->position, camera ) == 0 )
					continue;

				glUniform2fv( lightPos_ID, 1, glm::value_ptr( currentLight->position ) );
				glUniform4fv( lightColor_ID, 1, glm::value_ptr( currentLight->color ) );
				glUniform1f( lightRadius_ID, currentLight->radius );// * 1.0f / glm::max( c->GetWindowZoom(), 0.001f ) );
				glUniform1f( lightIntensity_ID, currentLight->intensity );
				glUniform2fv( lightDirection_ID, 1, glm::value_ptr( currentLight->direction ) );
				DrawScreenQuad();

				glUniform1f( pass_ID, 0 );
			}

			glUseProgram( 0 );

			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
		glDisable(GL_BLEND);
	}

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE,GL_ONE);
	glDisable(GL_BLEND);


	//prep for blur
	//glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, glowTexture, 0);

	{ //blur pass 0
		glUseProgram( postProcess_blurPass_Program );
		//unsigned st_ID = glGetUniformLocation(postProcess_blurPass_Program, "screenTexture");
		unsigned gt_ID = glGetUniformLocation(postProcess_blurPass_Program, "glowTexture");
		unsigned pass = glGetUniformLocation(postProcess_blurPass_Program, "pass");
		unsigned sampleRadius_ID = glGetUniformLocation(postProcess_blurPass_Program, "sample_radius");

		//const Camera* __restrict__ c = External::gi->GetCamera();

		//glUniform1i(st_ID, 0);
		glUniform1i(gt_ID, 0);
		glUniform1f(pass, 0);
		glUniform1f(sampleRadius_ID, 1.0f );// 5.0f / glm::max( c->GetWindowZoom(), 0.001f ) ); //TODO: fix. need this to make glow better

		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, glowTexture );

		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture( GL_TEXTURE_2D, glowTexture );

		DrawScreenQuad();
		//glUseProgram( 0 );

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture( GL_TEXTURE_2D, 0 );
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture( GL_TEXTURE_2D, 0 );
	}

	{ //blur pass 1
		//glUseProgram( postProcess_blurPass_Program );
		//unsigned st_ID = glGetUniformLocation(postProcess_blurPass_Program, "screenTexture");
		//unsigned gt_ID = glGetUniformLocation(postProcess_blurPass_Program, "glowTexture");
		unsigned pass = glGetUniformLocation(postProcess_blurPass_Program, "pass");

		//glUniform1i(st_ID, 0);
		//glUniform1i(gt_ID, 1);
		glUniform1f(pass, 1.0f);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture( GL_TEXTURE_2D, screenTexture );

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture( GL_TEXTURE_2D, glowTexture );

		DrawScreenQuad();
		glUseProgram( 0 );

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	{ //final pass
		glUseProgram( postProcess_finalPass_Program );
		unsigned st_ID = glGetUniformLocation(postProcess_finalPass_Program, "screenTexture");
		unsigned gt_ID = glGetUniformLocation(postProcess_finalPass_Program, "glowTexture");
		unsigned amb_ID = glGetUniformLocation(postProcess_finalPass_Program, "ambient_light");
		glUniform1i(st_ID, 0);
		glUniform1i(gt_ID, 1);
		glUniform3fv(amb_ID, 1, glm::value_ptr( ambient_light ) );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, screenTexture );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture( GL_TEXTURE_2D, glowTexture );

		DrawScreenQuad();
		glUseProgram( 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	 // TestTexture( glowTexture );
#endif //END -- TODO: CLEANUP TO USE NEW SHADER MAP
}


void RenderManager::RenderSprites(const Camera& camera, std::vector<RenderObject*>& objects)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram( spriteProgram );

    unsigned proj = glGetUniformLocation(spriteProgram, "projectionMatrix");
    const float* __restrict__ projMat = camera.GetProjectionMatrix();
    glUniformMatrix4fv(proj, 1, 0, projMat);


    unsigned spritePos = glGetUniformLocation(spriteProgram, "spritePosition");
    unsigned spriteRotation = glGetUniformLocation(spriteProgram, "spriteRotation");
    unsigned spriteScale = glGetUniformLocation(spriteProgram, "spriteScale");

    unsigned animationFrame = glGetUniformLocation(spriteProgram, "animationFrame");
    unsigned animationSize = glGetUniformLocation(spriteProgram, "animationSize");
	unsigned animationStartPos = glGetUniformLocation(spriteProgram, "animationStartPos");
	unsigned animationSpace = glGetUniformLocation(spriteProgram, "animationSpace");

	unsigned flippedSprite = glGetUniformLocation(spriteProgram, "flipped");

    unsigned spriteTexture = glGetUniformLocation(spriteProgram, "atlasTexture");

    unsigned glowPass_ID = glGetUniformLocation(spriteProgram, "glowPass");


    for(unsigned i = 0; i < objects.size(); ++i)
    {
        const RenderObject* __restrict__ renderObject = objects[i];
		const Sprite* __restrict__ s = renderObject->sprite;

		if( !s )
			continue;

		if( renderObject->visible == 0 )
			continue;

        if( IsNearCamera( s->transform->loc, camera ) == 0 )
            continue;

		const SpriteAnimation& a = s->animations[ s->active_animation ];

		//pass in transform data
        glUniform2f(spritePos, renderObject->transform->loc.x, renderObject->transform->loc.y);
        glUniformMatrix2fv(spriteRotation, 1, 0, renderObject->transform->rot_array());
        glUniform2f(spriteScale, renderObject->transform->scale.x, renderObject->transform->scale.y);

		//pass in animation data
		glUniform1f(animationFrame, a.current_frame);
		glUniform2f(animationSize, a.frame_size.x, a.frame_size.y);
		glUniform2f(animationStartPos, a.start_pos.x, a.start_pos.y);
		glUniform1f(animationSpace, a.h_space);

		//is the sprite fliipped?
		glUniform1f(flippedSprite, s->flipped);

		//pass in the texture
		glUniform1i(spriteTexture, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s->atlas_texture);

		glEnable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
		glUniform1f(glowPass_ID, 0);

		//TODO: replace with a vbo or vba?
		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 1.0);
		glVertex2f (-0.5, -0.5);

		glTexCoord2f (1.0, 1.0);
		glVertex2f (0.5, -0.5);

		glTexCoord2f (1.0, 0.0);
		glVertex2f (0.5, 0.5);

		glTexCoord2f (0.0, 0.0);
		glVertex2f (-0.5, 0.5);
		glEnd();

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glDisable(GL_BLEND);
		glUniform1f(glowPass_ID, 1.0f);

		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 1.0);
		glVertex2f (-0.5, -0.5);

		glTexCoord2f (1.0, 1.0);
		glVertex2f (0.5, -0.5);

		glTexCoord2f (1.0, 0.0);
		glVertex2f (0.5, 0.5);

		glTexCoord2f (0.0, 0.0);
		glVertex2f (-0.5, 0.5);
		glEnd();

	}

    glUseProgram( 0 );

    glPopMatrix();
    glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


//render specifc textures from the given atlas
void RenderManager::ShowAtlasTextures(vec2 position, vec2 size, std::vector<unsigned> textures, TextureAtlas* atlas, unsigned selected_index)
{
	if(!atlas)
		return;

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
	int w = Global::GameWindow->GetW();
	int h = Global::GameWindow->GetH();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glUseProgram( atlasGUIProgram );

    float projMat[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projMat);
    unsigned proj = glGetUniformLocation(atlasGUIProgram, "projectionMatrix");
    glUniformMatrix4fv(proj, 1, 0, projMat);

    unsigned texturePos = glGetUniformLocation(atlasGUIProgram, "texturePosition");
    unsigned textureScale = glGetUniformLocation(atlasGUIProgram, "textureScale");

    unsigned atlasTexture = glGetUniformLocation(atlasGUIProgram, "atlasTexture");
    unsigned atlasMapTexture = glGetUniformLocation(atlasGUIProgram, "atlasMapTexture");

    unsigned atlasTextureIndex = glGetUniformLocation(atlasGUIProgram, "atlasTextureIndex");
    unsigned drawIndex = glGetUniformLocation(atlasGUIProgram, "drawIndex");

	int render_index = 0;
    for(unsigned i = 0; i < textures.size(); ++i)
    {
		if( textures[i] == (unsigned short)(-1) )
			continue;

		//pass in transform data
        glUniform2f(texturePos, position.x, position.y);
        glUniform2f(textureScale, size.x, size.y);
		glUniform1i(atlasTextureIndex, (int)textures[i]);
		glUniform1i(drawIndex, render_index);

		//pass in the texture
		glUniform1i(atlasTexture, 0);
		glUniform1i(atlasMapTexture, 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas->atlas_texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, atlas->packed_data_texture);


		//TODO: replace with a vbo or vba?
		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex2f (-0.5, -0.5);

		glTexCoord2f (1.0, 0.0);
		glVertex2f (0.5, -0.5);

		glTexCoord2f (1.0, 1.0);
		glVertex2f (0.5, 0.5);

		glTexCoord2f (0.0, 1.0);
		glVertex2f (-0.5, 0.5);
		glEnd();

		render_index++;
	}

    glUseProgram( 0 );

	int found_selected_index = -1;
	render_index = 0;
    for(unsigned i = 0; i < textures.size(); ++i)
    {
		if( textures[i] == (unsigned short)(-1) )
			continue;

		//glColor4f(1,1,1,1);
		if( textures[i] == selected_index )
		{
			found_selected_index = render_index;
		}

		vec2 line_bl = position - (size * 0.5f);
		vec2 line_tr = position + (size * 0.5f);

		line_bl.x += size.x * (float)render_index;
		line_tr.x += size.x * (float)render_index;

		DrawBox(line_bl, line_tr, vec4(1,1,1,1));

		render_index++;
	}

	if( found_selected_index >= 0 )
	{
		unsigned max_textures_on_screen = (Global::GameWindow->GetW() - size.x/2) / size.x;
		if( max_textures_on_screen%2 == 0 )
			max_textures_on_screen -= 1;

		if( found_selected_index > max_textures_on_screen/2 )
			found_selected_index = max_textures_on_screen/2;

		//glColor4f(0,1,0,1);
		vec2 line_bl = position - (size * 0.5f);
		vec2 line_tr = position + (size * 0.5f);

		line_bl.x += size.x * (float)found_selected_index;
		line_tr.x += size.x * (float)found_selected_index;

		DrawBox(line_bl, line_tr, vec4(0,1,0,1));
	}

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void RenderManager::DrawBox(vec2 bottom_left, vec2 top_right, vec4 color)
{
	glColor4fv( glm::value_ptr( color ) );

	glBegin(GL_LINE_LOOP);
	glVertex2f (bottom_left.x, bottom_left.y);

	glVertex2f (bottom_left.x, top_right.y);

	glVertex2f (top_right.x, top_right.y);

	glVertex2f (top_right.x, bottom_left.y);
	glEnd();
}



void RenderManager::RenderGrid(const Camera& camera, GridInfo& grid, unsigned pass) //send in 0 or 1 only
{
	if( !grid.atlas )
		return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation( GL_FUNC_ADD );

	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    glUseProgram( gridProgram );

    unsigned pos = glGetUniformLocation(gridProgram, "gridPosition");
    unsigned gridSize = glGetUniformLocation(gridProgram, "gridSize");
    unsigned cellSize = glGetUniformLocation(gridProgram, "cellSize");
    unsigned gridTexture = glGetUniformLocation(gridProgram, "gridTexture");
    unsigned atlasTexture = glGetUniformLocation(gridProgram, "atlasTexture");
    unsigned atlasMapTexture = glGetUniformLocation(gridProgram, "atlasMapTexture");
    unsigned passUniform = glGetUniformLocation(gridProgram, "pass");

    unsigned proj = glGetUniformLocation(gridProgram, "projectionMatrix");
    const float* projMat = camera.GetProjectionMatrix();
    glUniformMatrix4fv(proj, 1, 0, projMat);

	const vec2 render_position = grid.position + grid.texture_position_offset;

    glUniform2f(pos, render_position.x, render_position.y);
    glUniform2f(gridSize, grid.grid_size.x, grid.grid_size.y);
    glUniform2f(cellSize, grid.cell_size.x, grid.cell_size.y);

    glUniform1i(passUniform, pass);

    glUniform1i(gridTexture, 0);
    glUniform1i(atlasTexture, 1);
    glUniform1i(atlasMapTexture, 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grid.cell_texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grid.atlas->atlas_texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, grid.atlas->packed_data_texture);



    //TODO: replace with a vbo or vba?
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
    glTexCoord2f (0.0, 1.0);
    glVertex2f (-0.5, -0.5);

    glTexCoord2f (1.0, 1.0);
    glVertex2f (0.5, -0.5);

    glTexCoord2f (1.0, 0.0);
    glVertex2f (0.5, 0.5);

    glTexCoord2f (0.0, 0.0);
    glVertex2f (-0.5, 0.5);
    glEnd();

    glUseProgram( 0 );

    glPopMatrix();
    glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}




///////////////////////////////
///////////////////////////////

void RenderManager::DrawScreenQuad()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    //glEnable(GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1,1);
    //glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f (0.0, 0.0);
    glVertex2f (-1.0, -1.0);
    glTexCoord2f (1.0, 0.0);
    glVertex2f (1.0f, -1.0);
    glTexCoord2f (1.0, 1.0);
    glVertex2f (1.0f, 1.0f);
    glTexCoord2f (0.0, 1.0);
    glVertex2f (-1.0, 1.0f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

