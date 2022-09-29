#ifndef AREA_PLANET_A_H
#define AREA_PLANET_A_H

#include <Area.h>

class AreaPlanetA : public Area
{
        public:
                AreaPlanetA();
                virtual ~AreaPlanetA();
                virtual std::string GetName();

                virtual void Init();
                virtual void UpdateArea();
                virtual void Cleanup();
                virtual void CustomDraw();

        private:
};


#endif

