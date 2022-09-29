#ifndef CREATEAREA_H
#define CREATEAREA_H

#include <Command.h>

//Creates a fresh new area with the given name and size
class CreateArea : public Command
{
	public:
		CreateArea(const std::string& name, unsigned grid_size_x, unsigned grid_size_y);
		virtual ~CreateArea();
		virtual truth Execute();

	private:

        std::string area_name;
        unsigned size_x;
        unsigned size_y;
};

#endif
