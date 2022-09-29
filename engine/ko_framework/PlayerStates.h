#ifndef PLAYERSTATES_H
#define PLAYERSTATES_H

#include <globals.h>
#include <states.h>

/////////////////////////////////
// the player's state machine

STATE( player_start )
STATE( player_idle )
STATE( player_walk )


STATE_CUSTOM_BEGIN( player_jump )
STATE_CUSTOM_END

STATE_CUSTOM_BEGIN( player_attack )
STATE_CUSTOM_END

#endif
