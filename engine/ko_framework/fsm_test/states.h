#ifndef STATES_H
#define STATES_H


//defines to make creating states cleaner
//see the 'null' state for an example

#define STATE( state_name )\
class state_name : public state \
{ \
    public:\
    std::string name() { return std::string( #state_name ); }\
    state_name(state_object* _o):state(_o){} \
    state* run();\
};

#define STORAGE_STATE( state_name )\
template <typename T> \
class state_name : public state \
{ \
    public:\
    std::string name() { return std::string( #state_name ); }\
    state_name(state_object* _o):state(_o){} \
    state* run();\
    T* stored_state(state_object* _o){return new T(_o);}; \
    std::string stored_name() { T t(o); return t.name(); }\
};

////////////////////////////////////////////////////////
// this is what the states pass around
class BaseObject;

typedef BaseObject state_object;


//base state class
class state
{
    public:
        state(state_object* _o):o(_o){}
        virtual ~state(){};
        virtual std::string name() = 0;
        virtual state* run()=0;
    protected:
        object* o;
};

//this function is used to process the states
//if 'fsm' is null after running, the state machine is completed
void run_fsm(state** fsm);

STATE( null )

#endif
