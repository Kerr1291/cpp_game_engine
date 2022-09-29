
#include <states.h>

namespace fsm
{
	void run_fsm(state** fsm)
	{
		if( !*fsm )
			return;

		if(!(*fsm)->is_loaded())
			(*fsm)->init();

		state* next = (*fsm)->run();
		if( (*fsm) != next )
			delete *fsm;
		(*fsm) = next;
	}

	void change_state(state** fsm, state* new_state)
	{
		if( (*fsm) != new_state )
			delete *fsm;
		(*fsm) = new_state;
	}
}

state* null::run()
{
    return 0;
}



#if 0
/*
state* quit::run()
{
        std::cout<<"id "<<o->id <<" entering state "<<name()<<"\n";
        cleanup.push_back( o );
        return 0;
}



state* reduce::run()
{
        std::cout<<"id "<<o->id <<" entering state "<<name()<<"\n";
        o->count--;
        std::cout<<"reducing count to " <<o->count <<"\n";

        if( o->count %2 == 0 )
            return new divide<reduce>(o);

        if( o->count <= 0 )
            return new quit(o);

        return this;
}



template <typename T>
state* divide<T>::run()
{
        std::cout<<"id "<<o->id <<" entering state "<<name()<<"\n";
        o->count /= 2;
        std::cout<<"dividing count to " <<o->count <<"\n";
        std::cout<<"moving to stored state "<<stored_name() <<"\n";
        return stored_state(o);
}


state* start::run()
{
        std::cout<<"id "<<o->id <<" entering state "<<name()<<"\n";

        if( o->count > 0 )
        {
            if( o->count %2 == 0 )
                return new divide<start>(o);
            else
                return new reduce(o);
        }

        return new quit(o);
}
*/
#endif
