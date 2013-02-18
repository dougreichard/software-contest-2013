#pragma once

#include "GameHost.h"

class Player;
class PlayerResult;
class GamePlayerResult;
class SingleGame : public GameHost {
public:
	SingleGame(GameClient* host, GameClient* one, GameClient* two);
	virtual ~SingleGame();
	virtual void Start();
	virtual void End();
	virtual void Command(string s);
	virtual void PlayerStartCallback(PlayerResult* );
	virtual void PlayerStepCallback(PlayerResult* );
	virtual void SendGameState(string s);
	virtual void PlayerStateCallback(PlayerResult* );

	enum gameState {none, GameStart, GameStep, GameState };
private:
	PushableCard _turn[2];
	Player* _players[2];
	CastleGameBoard _board;
	GamePlayerResult* _waiting[2];
	PlayerResult* _arrived[2];

	int _state;
};
