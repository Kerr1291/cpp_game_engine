#include <GraphicData.h>
#include <RenderManager.h>
GraphicData::GraphicData()
:renderData(0)
{
}

GraphicData::~GraphicData()
{
        if(renderData)
        {
                for(unsigned i = 0; i < gfxPoints.size(); ++i)
                        delete[] *renderData;
                delete[] renderData;
        }
}
       
void GraphicData::CreateRenderData()
{
        if( renderData )
        {
                std::cerr<<"Error: Cannot generate render data a 2nd time.\n";
                return;
        }


        solid = std::vector<truth>( gfxPoints.size(), 0 );

        renderData = new float*[ gfxPoints.size() ];

        for( unsigned i = 0; i < gfxPoints.size(); ++i )
        {
                unsigned numFloats = gfxPoints[i].size()*2;
                dataSizes.push_back( numFloats );
                renderData[ i ] = new float[ numFloats ];
                for( unsigned j = 0; j < gfxPoints[i].size(); j++ )
                {
                        renderData[i][ 2*j ] = gfxPoints[i][j].x;
                        renderData[i][ (2*j)+1 ] = gfxPoints[i][j].y;
                }
        }
       
        //make shader rendering data

        unsigned numShapes = gfxPoints.size();
        vao = std::vector<unsigned>( numShapes, 0 );
        buffer = std::vector<unsigned>( numShapes, 0 );

        const unsigned vert = glGetAttribLocation(External::renderer->basicProgram, "vert");
       
        glGenVertexArrays(numShapes, &vao[0]);
        glGenBuffers(numShapes, &buffer[0]);

        for(unsigned i = 0; i < numShapes; ++i)
        {
                const unsigned numVerts = dataSizes[i];

                glBindVertexArray(vao[i]);

                glEnableVertexAttribArray( vert );

                glBindBuffer(GL_ARRAY_BUFFER, buffer[i]);

                glVertexAttribPointer(vert, 2, GL_FLOAT, GL_FALSE, 0, 0);

                glBufferData(GL_ARRAY_BUFFER,
                                 sizeof(float)*numVerts,
                                 renderData[i],
                                 GL_STATIC_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
       
}

