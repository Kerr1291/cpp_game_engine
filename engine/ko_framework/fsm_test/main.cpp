
#include "states.h"



void run_fsm(state** fsm)
{
    if( !*fsm )
        return;

    state* next = (*fsm)->run();
    if( (*fsm) != next )
        delete *fsm;
    (*fsm) = next;
}

int main()
{
    object* a = new object(1);
    object* b = new object(2);
    object* c = new object(3);

    state* fsm_a = new start(a);
    state* fsm_b = new start(b);
    state* fsm_c = new start(c);

    while( fsm_a || fsm_b || fsm_c )
    {
        run_fsm( &fsm_a );
        run_fsm( &fsm_b );
        run_fsm( &fsm_c );

        for( unsigned i = 0; i < cleanup.size(); ++i )
        {
            if( a == cleanup[i] )
                a = 0;
            if( b == cleanup[i] )
                b = 0;
            if( c == cleanup[i] )
                c = 0;
            delete cleanup[i];
        }

        if( cleanup.size() > 0 )
            cleanup.clear();
    }

    std::cout<<"all states complete\n";

    return 0;
}
