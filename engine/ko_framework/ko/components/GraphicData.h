#ifndef GRAPHICDATA_H
#define GRAPHICDATA_H

#include <globals.h>

struct GraphicData
{
        GraphicData();
        ~GraphicData();

        void CreateRenderData();

        std::vector< std::vector< vec2 > > gfxPoints;
        std::vector< truth > solid;

        std::vector<unsigned> dataSizes;
        float** renderData;
        std::vector<unsigned> vao;
        std::vector<unsigned> buffer;
};


#endif

