#include <RenderManager.h>


#include <memory.h>
#include <fstream>

#include <GraphicData.h>
#include <Transform.h>
#include <RenderObject.h>
#include <Camera.h>
#include <AreaLoader.h>
#include <GameInterface.h>

#include <stdio.h>

//Helper function, should really move to camera class
inline truth Deprecated_IsNearCamera(const vec2& pos, const Camera& c)
{
		const vec2& c_ext = c.GetCameraExtents();

		//float l = (-c_ext.x);
        float r = ( c_ext.x);
        float t = ( c_ext.y);
        //float b = (-c_ext.y);
        if( pos.x < -r )
                return false;
        if( pos.x > r )
                return false;
        if( pos.y < -t )
                return false;
        if( pos.y > t )
                return false;
        return true;
}



void RenderManager::Render(std::vector<RenderObject*>& objects)
{
        glUseProgram( basicProgram );

        unsigned scale = glGetUniformLocation(basicProgram, "scale");
        unsigned rotation = glGetUniformLocation(basicProgram, "rotation");
        unsigned pos = glGetUniformLocation(basicProgram, "pos");
        unsigned color = glGetUniformLocation(basicProgram, "object_color");
        unsigned proj = glGetUniformLocation(basicProgram, "projectionMatrix");
        unsigned glow = glGetUniformLocation(basicProgram, "hasGlow");
        int enable_glow = 1;

        //float projMat[16];
        //glGetFloatv(GL_PROJECTION_MATRIX, projMat);
        Camera* c = External::gi->GetCamera();
        const float* projMat = c->GetProjectionMatrix();

        glUniformMatrix4fv(proj, 1, 0, projMat);

        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_COLOR, GL_ONE);

        for(unsigned i = 0; i < objects.size(); ++i)
        {
                if( objects[i]->visible == 0 )
                        continue;

                if( Deprecated_IsNearCamera( objects[i]->transform->loc, *c ) == 0 )
                        continue;

                GraphicData* gfx = objects[i]->gfx;
                unsigned numShapes = gfx->gfxPoints.size();

				glUniform2f(scale, objects[i]->transform->scale.x, objects[i]->transform->scale.y);
                glUniformMatrix2fv(rotation, 1, 0, objects[i]->transform->rot_array());
                glUniform2f(pos, objects[i]->transform->loc.x, objects[i]->transform->loc.y);
                glUniform1i(glow, enable_glow);

                for(unsigned j = 0; j < numShapes; ++j)
                {
                        unsigned numVerts = gfx->dataSizes[j];

                        glBindVertexArray(gfx->vao[j]);
                        //std::cerr<<j<<": drawing: "<<numVerts/2 <<"\n";

                        if( objects[i]->gfx->solid[j] )
                        {
                                float fancolor[4] = {
                                                  0.5f * objects[i]->colors[j][0],
                                                  0.5f * objects[i]->colors[j][1],
                                                  0.5f * objects[i]->colors[j][2], 0.5f };
                                glUniform4fv(color, 1, fancolor);
                                glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts/2);
                        }

                        glUniform4fv(color, 1, objects[i]->colors[j]);
                        glDrawArrays(GL_LINE_LOOP, 0, numVerts/2);
                }
        }

        glDisable(GL_BLEND);

        glUseProgram( 0 );
}



void RenderManager::RenderObjects(std::vector<RenderObject*>& objects)
{

        Camera* c = External::gi->GetCamera();
        for(unsigned i = 0; i < objects.size(); ++i)
        {
                if( objects[i]->visible == 0 )
                        continue;

                if( Deprecated_IsNearCamera( objects[i]->transform->loc, *c ) == 0 )
                        continue;

                for( unsigned k = 0; k < objects[i]->gfx->gfxPoints.size(); ++k)
                {
                        glColor4fv( objects[i]->colors[k] );
                        std::vector<vec2>& points = objects[i]->gfx->gfxPoints[k];



                glBegin(GL_LINES);

                for( unsigned j = 0; j < (-1 + points.size()); ++j )
                {
                        vec2 renderpoint1 = vec2( objects[i]->transform->scale.x * points[j].x, objects[i]->transform->scale.y * points[j].y ); //scale
                        renderpoint1 = objects[i]->transform->rot * renderpoint1; //rotate
                        renderpoint1 += objects[i]->transform->loc; //translate


                        vec2 renderpoint2 = vec2( objects[i]->transform->scale.x * points[j+1].x, objects[i]->transform->scale.y * points[j+1].y ); //scale
                        renderpoint2 = objects[i]->transform->rot * renderpoint2; //rotate
                        renderpoint2 += objects[i]->transform->loc; //translate

                        glVertex2f( renderpoint1.x, renderpoint1.y ); //draw point
                        glVertex2f( renderpoint2.x, renderpoint2.y ); //draw point
                }

                unsigned lastPoint = points.size()-1;
                vec2 renderpoint1 = vec2( objects[i]->transform->scale.x * points[lastPoint].x, objects[i]->transform->scale.y * points[lastPoint].y ); //scale
                renderpoint1 = objects[i]->transform->rot * renderpoint1; //rotate
                renderpoint1 += objects[i]->transform->loc; //translate


                vec2 renderpoint2 = vec2( objects[i]->transform->scale.x * points[0].x, objects[i]->transform->scale.y * points[0].y ); //scale
                renderpoint2 = objects[i]->transform->rot * renderpoint2; //rotate
                renderpoint2 += objects[i]->transform->loc; //translate

                glVertex2f( renderpoint1.x, renderpoint1.y ); //draw point
                glVertex2f( renderpoint2.x, renderpoint2.y ); //draw point
                glEnd();

                        if( objects[i]->gfx->solid[k] )
                        {
                                glEnable(GL_BLEND);
                                //glBlendFunc (GL_SRC_COLOR, GL_ONE);
                                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                                glColor4f(0.5f * objects[i]->colors[k][0],
                                                  0.5f * objects[i]->colors[k][1],
                                                  0.5f * objects[i]->colors[k][2], 0.5f);

                                glEnable( GL_TEXTURE_2D );
                                glBindTexture( GL_TEXTURE_2D, objects[i]->texture );

                                glBegin(GL_TRIANGLE_FAN);
                                for( unsigned j = 0; j < points.size(); ++j )
                                {
                                        vec2 renderpoint = vec2( objects[i]->transform->scale.x * points[j].x, objects[i]->transform->scale.y * points[j].y ); //scale
                                        renderpoint = objects[i]->transform->rot * renderpoint; //rotate
                                        renderpoint += objects[i]->transform->loc; //translate

                                        if( j == 0 )      glTexCoord2f(0.0f,0.0f);
                                        else if( j == 2 ) glTexCoord2f(1.0f,1.0f);
                                        else if( j == 3 ) glTexCoord2f(0.0f,1.0f);
                                        else if( j == 1 ) glTexCoord2f(1.0f,0.0f);

                                        glVertex2f( renderpoint.x, renderpoint.y );
                                }
                                glEnd();
                                glDisable(GL_BLEND);
                                glBindTexture( GL_TEXTURE_2D, 0 );
                                continue;
                        }
                }
        }
}

GraphicData* RenderManager::LoadGfx(std::string filename)
{
        if( HasGfx( filename ) )
                return graphics[ loadedGraphics[ filename ] ];

        std::fstream fs(filename.c_str(), std::ios::in);

        if( !fs.is_open() )
        {
                std::cerr<<"FILE NOT FOUND: "<<filename <<"\n";
                return 0;
        }

        GraphicData* data = new GraphicData();
        unsigned numSets = 0;
        unsigned readSets = 0;
        unsigned totalPoints = 0;
        unsigned readPoints = 0;
        fs >> numSets;

        while( fs.good() )
        {
                std::string currentSetName;
                unsigned numPoints = 0;
                std::vector< vec2 > setPoints;

                fs >> currentSetName;
                fs >> numPoints;
                totalPoints+=numPoints;

                for(unsigned i = 0; i < numPoints; ++i)
                {
                        vec2 point;
                        fs >> point.x;
                        fs >> point.y;
                        setPoints.push_back( point );
                        readPoints++;
                }
                readSets++;
                data->gfxPoints.push_back( setPoints );
        }

        if( readSets != numSets )
        {
                std::cerr<<"Error gfx Load failed: numSets != amount of point sets read from file!\n";
                delete data;
                return 0;
        }

        if( readPoints != totalPoints )
        {
                std::cerr<<"Error gfx Load failed: totalPoints != amount of points read from file!\n";
                delete data;
                return 0;
        }

        data->CreateRenderData();

        if( !AddGfx( data, filename ) )
        {
                std::cerr<<"Error: failed to add graphic to system!\n";
                delete data;
                return 0;
        }

        return data;
}

GraphicData* RenderManager::LoadGfxFromBMP(const std::string& filename)
{
    if( HasGfx( filename ) )
                return graphics[ loadedGraphics[ filename ] ];

    //Make sure the extension is .bmp
        std::string::const_reverse_iterator ext_getter = filename.rbegin();
        std::string ext;
        for(unsigned i = 0; i < 3; ++i, ++ext_getter)
                ext.push_back( *ext_getter );

        if( ext != "pmb" )
        {
                std::cerr<<"Error: "<<filename <<" not a bitmap file.\n";
                return 0;
        }

    //load in the data
    std::list<ColorData> data;
    LoadColorDataForGfx(filename, data);

    if(data.empty())
        {
                std::cerr<<"Error: No colors found in the bitmap file.\n";
                return 0;
        }

    const unsigned H = 100;
    const unsigned W = 100;
    std::map<unsigned, std::list<ColorData> > sorted;

    std::list<ColorData>::iterator iter;
    iter = data.begin();
    for(; iter != data.end(); ++iter)
    {
        //get the index, sort them based on the index
        unsigned index = iter->g;
        sorted[ index ].push_back( *iter );
    }

    GraphicData* gfxData = new GraphicData();

    std::map<unsigned, std::list<ColorData> >::iterator iter2;
    iter2 = sorted.begin();
    for(; iter2 != sorted.end(); ++iter2)
    {
                if( iter2->second.size() <= 1 )
                {
                        std::cerr<<"Error: cannot generate set of points with 1 or fewer points.\n";
                        delete gfxData;
                        return 0;
                }

                std::map<unsigned, vec2> sorted_points;

                //generate sorted set of points
                iter = iter2->second.begin();
                for(; iter != iter2->second.end(); ++iter)
                        sorted_points[ iter->r ] = iter->loc;

                //put the sorted points into a vector
                std::vector< vec2 > setPoints;
                std::map<unsigned, vec2>::iterator set_maker;
                set_maker = sorted_points.begin();
                for(; set_maker != sorted_points.end(); ++set_maker)
                        setPoints.push_back( set_maker->second );

                //put the vector in the new graphic object
                gfxData->gfxPoints.push_back( setPoints );
    }

        gfxData->CreateRenderData();

    if( !AddGfx( gfxData, filename ) )
    {
        std::cerr<<"Error: failed to add graphic to system!\n";
        delete gfxData;
        return 0;
    }

        return gfxData;
}



RenderObject* RenderManager::CreateRenderObject(Transform* t, GraphicData* g)
{
        RenderObject* r = new RenderObject(t,g);
        // renderObjects.push_back( r );
        return r;
}

void RenderManager::TestTexture(unsigned texture)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1,1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, texture );
        glBegin(GL_QUADS);
        glTexCoord2f (0.0, 0.0);
        glVertex2f (-1.0, -1.0);
        glTexCoord2f (1.0, 0.0);
        glVertex2f (0, -1.0);
        glTexCoord2f (1.0, 1.0);
        glVertex2f (0, 0);
        glTexCoord2f (0.0, 1.0);
        glVertex2f (-1.0, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);


    glColor4f(0.5,0.5,0.5,1);
        glBegin(GL_LINES);
        glVertex2f (-1.0, -1.0);
        glVertex2f (0, -1.0);

        glVertex2f (0, -1.0);
        glVertex2f (0, 0);

        glVertex2f (0, 0);
        glVertex2f (-1.0, 0);

        glVertex2f (-1.0, 0);
        glVertex2f (-1.0, -1.0);
        glEnd();
    glColor4f(1,1,1,1);


    glPopMatrix();
    glDisable(GL_BLEND);
}


void RenderManager::CreateTexture(unsigned &texture)
{
        if( texture != 0 )
                glDeleteTextures(1, &texture);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, Global::GameWindow->GetW(), Global::GameWindow->GetH(), 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

        //original
        //glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, Global::Window::Width, Global::Window::Height, 0,GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 0);

        //first try
        //glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, Global::Window::Width, Global::Window::Height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
}



void RenderManager::CreateRenderbuffer(unsigned &buffer, unsigned &framebuffer, unsigned texture0, unsigned texture1)
{
        if( framebuffer != 0 )
                glDeleteFramebuffers(1, &framebuffer);
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        if( buffer != 0 )
                glDeleteRenderbuffers(1, &buffer);
        glGenRenderbuffers(1, &buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Global::GameWindow->GetW(), Global::GameWindow->GetH());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer);

        // Set "renderedTexture" as our colour attachement #0
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture0, 0);
        if( texture1 != 0 )
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture1, 0);

        GLenum DrawBuffers[4] = {0,0,0,0};
        unsigned numBuffers = 0;

        if(texture1 != 0)
        {
                DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
                DrawBuffers[1] = GL_COLOR_ATTACHMENT1;
                numBuffers = 2;
        }
        else
        {
                DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
                numBuffers = 1;
        }

        glDrawBuffers(numBuffers, DrawBuffers);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
                std::cerr<<"Framebuffer status returned: " <<glCheckFramebufferStatus(GL_FRAMEBUFFER) <<"\n";
                std::cerr<<"Error generating framebuffer!\n";
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderManager::LoadColorDataForGfx(const std::string& filename, std::list<ColorData >& out_data)
{
//SDL HAS BEEN REMOVED
#if 0
        if(filename.empty())
                        return;

        SDL_Surface* data = SDL_LoadBMP( filename.c_str() );
        if(!data)
        {
                std::cerr<<"Error loading file: "<<filename <<"\n";
                return;
        }

        std::list< ColorData > colordata;

        const unsigned pixelsize = 3;

        if( data->format->BytesPerPixel != pixelsize )
        {
                std::cerr<<"Error: must be a 24-bits-per-pixel bitmap. Load failed\n";
                SDL_FreeSurface(data);
                return;
        }

        if( !(data->w%2 || data->h%2) )
        {
                std::cerr<<"Error: bitmap height and width must be odd numbers. Load failed\n";
                SDL_FreeSurface(data);
                return;
        }

        if(SDL_MUSTLOCK(data))
        {
                SDL_LockSurface( data );
        }

        //when inserting the locations, we convert the bitmap to a coordinate grid
        //then use the center of the bitmap as the origin and scale down
        //the distance between points so all points lie in a 0 to 1 range
        for(int y = 0; y < data->h; y++)
        {
                for(int x = 0; x < data->w; x++)
                {
                        AreaLoader::pixeltype color[3]={0,0,0};
                        memcpy(color,
                        &((AreaLoader::pixeltype*)data->pixels)[(y*data->pitch) + x*pixelsize],
                        sizeof(AreaLoader::pixeltype)*pixelsize);

                        if((color[0] != 0 || color[1] != 0 || color[2] != 0) &&
                                                                          !(color[0] == 255 && color[1] == 255 && color[2] == 255) )
                        {
                                int w = data->w - 1;
                                int h = data->h - 1;
                                int midx = w / 2;
                                int midy = h / 2;
                                float in_x = static_cast<float>(x), in_y = static_cast<float>(y);
                                if(x != midx)
                                        in_x = (x < midx ? -(midx - in_x) : in_x - midx );
                                if(y != midy)
                                        in_y = (y < midy ? midy - in_y : -(in_y - midy) );
                                in_x /= midx;
                                in_y /= midy;
                                colordata.push_back(ColorData(color[2],color[1],color[0], vec2(in_x,in_y)));
                        }
                }
        }

        if( SDL_MUSTLOCK( data ) )
        {
                SDL_UnlockSurface( data );
        }

        SDL_FreeSurface(data);
        out_data = colordata;
#endif
}



truth RenderManager::AddGfx( GraphicData* g, std::string filename )
{
        if( !g )
        {
                std::cerr<<"error cannot add null graphic object\n";
                return 0;
        }

        if( maxStorage == currentStorage )
        {
                AllocateGraphicSpace( maxStorage * 2 );
        }

        graphics[ currentStorage ] = g;
        loadedGraphics[ filename ] = currentStorage;

        currentStorage++;
        return 1;
}

truth RenderManager::HasGfx( std::string filename )
{
        if( loadedGraphics.find( filename ) != loadedGraphics.end() )
        {
                return 1;
        }
        return 0;
}

void RenderManager::AllocateGraphicSpace( unsigned size )
{
        if( size <= maxStorage )
        {
                std::cerr<<"ERROR: cannot allocate a smaller buffer than the current max size!\n";
                return;
        }

        if( graphics )
        {
                GraphicData** old_graphics = new GraphicData*[maxStorage];
                memcpy( old_graphics, graphics, sizeof( GraphicData* ) * maxStorage  );
                delete[] graphics;
                graphics = new GraphicData*[size];
                memset( graphics, 0, sizeof( GraphicData* ) * size );
                memcpy( graphics, old_graphics, sizeof( GraphicData* ) * maxStorage  );
                delete[] old_graphics;
        }
        else
        {
                graphics = new GraphicData*[size];
                memset( graphics, 0, sizeof( GraphicData* ) * size );
                currentStorage = 0;
        }

        maxStorage = size;
}


// Search the render manager for a RenderObject with the name given.
// If the object is found a pointer to that object is returned.
// If the object is not found then 0 is returned.
RenderObject* RenderManager::FindRenderObject(const std::string& nameToFind)
{
        if( nameToFind.empty() )
                return 0;

        for( unsigned i = 0; i < renderObjects.size(); ++i )
        {
                if( nameToFind == renderObjects[i]->name )
                        return renderObjects[i];
        }

        return 0;
}


// Remove the given render object from the system (if it exists)
// Pass in a pointer to the object to be removed.
// You may use FindRenderObject to retrieve this pointer if needed.
// 1 (or true) is returned if successful, otherwise 0 (false) is returned.
truth RenderManager::RemoveRenderObject( RenderObject* r )
{
        if(!r)
                return 0;

        for(unsigned i = 0; i < renderObjects.size(); ++i)
        {
                if( r == renderObjects[i] )
                {
                        RemoveRenderObject( i );
                        return 1;
                }
        }

		return 0;
}


truth RenderManager::RemoveRenderObject(unsigned i)
{
        if(i >= renderObjects.size())
                return 0;

        delete renderObjects[i];
        renderObjects[i] = 0;
        renderObjects.erase( renderObjects.begin() +  i );

		return 1;
}

