#pragma once

#include "Socket.h"

class GameHost;
class GameLobby;
class PlayerResult;

class ITournament;
class GameClient : public ClientSocket {
public:
	GameClient(GameLobby* lobby);
	virtual void OnString(string s);
	virtual void OnDisconnect();
	string getName();
	void ResquestCallback(PlayerResult* );
	int getID();
protected:
	PlayerResult* _callback;
	GameHost* _game;
	GameLobby* _lobby;
	string _name;
	static ITournament* _tournament;
};

