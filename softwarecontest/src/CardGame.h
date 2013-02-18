#pragma once

#include "GameHost.h"

// A game that is single player played by the Game host
// JUst to learn card Play
class CardGame : public GameHost {
	// State 
	// Game started - Card = NULL -> Card != NULL
	// Wait for push - Card != NULL <-  -> Card->
	// Game Over - 
public:
	CardGame(GameClient* host);
	virtual void Start();
	virtual void End();
	virtual void Command(string s);
private:
	//shared_ptr<PushableCard> _card;
	PushableCard _card;
};


