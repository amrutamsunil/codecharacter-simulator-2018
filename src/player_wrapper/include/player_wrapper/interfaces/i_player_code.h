/**
 * @file i_player_code.h
 * Interface for the player AI
 */


#ifndef PLAYER_INTERFACES_I_PLAYER_CODE_H
#define PLAYER_INTERFACES_I_PLAYER_CODE_H

#include "player/player_wrapper_export.h"

namespace player {

/**
 * Player AI interface
 */
class PLAYER_WRAPPER_EXPORT IPlayerCode {
public:
	/**
	 * Player AI update function (main logic of the AI)
	 */
	virtual void Update() = 0;

	/**
	 * Destructor
	 */
	virtual ~IPlayerCode() {}
};
}

#endif
