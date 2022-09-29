#include <Command.h>

Command::Command(truth isblocking)
        :blocking(isblocking)
        ,managed(CommandType::MANAGED)
        ,complete(0){}

Command::~Command()
{
        //if( managed == CommandType::NON_MANAGED )
        //{
        //      std::list<Command*>::iterator iter = chain.begin();
        //      for(; iter != chain.end(); ++iter)
        //              delete *iter;
        //}
}

truth Command::IsComplete()
{
        return complete;
}

void Command::Reset()
{
        complete = 0;
}

//void Command::AddCommandToChain(Command* cmd)
//{
        //cmd->managed = managed;
        //chain.push_back( cmd );
//}

