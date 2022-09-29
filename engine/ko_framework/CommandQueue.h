#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <list>

class Command;

class CommandQueue
{
	public:

		CommandQueue();
		~CommandQueue();

		void Process();
        void Add( Command* command );
		bool Find( Command* command );
        void Cleanup();

    private:

		std::list<Command*> commands;

};


#endif