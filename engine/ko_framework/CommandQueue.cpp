#include <CommandQueue.h>

#include <Command.h>

CommandQueue::CommandQueue()
{}

CommandQueue::~CommandQueue()
{
	while( !commands.empty() )
	{
		delete commands.back();
		commands.pop_back();
	}
}


void CommandQueue::Process()
{
    if(commands.empty())
        return;

    std::list<Command*>::iterator iter = commands.begin();
    while( iter != commands.end() )
    {
        if( (*iter)->IsComplete() )
        {
            if( (*iter)->managed == CommandType::MANAGED )
				delete *iter;
            else
				(*iter)->Reset();
            iter = commands.erase( iter );
        }
        else
        {
            (*iter)->Execute();
            if((*iter)->blocking)
				break;
            ++iter;
        }
    }
}

void CommandQueue::Add( Command* command )
{
	commands.push_back( command );
}

bool CommandQueue::Find( Command* command )
{
    if(commands.empty())
        return false;

    std::list<Command*>::iterator iter = commands.begin();
    for(; iter != commands.end(); ++iter )
    {
        if( (*iter) == command )
            return true;
    }

    return false;
}

void CommandQueue::Cleanup()
{
}