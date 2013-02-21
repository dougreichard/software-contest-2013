#pragma once

#include "Socket.h"
#include <list>
using namespace std;

class GameClient;
class GameLobby: public ServerSocket {
public:
	void leave(GameClient* client);

	GameClient* getPlayerByName(string name);
	list<GameClient*>& getClients();
protected:
	virtual ClientSocket* AllocClient();
	virtual void OnString(string s);
protected:
	list<GameClient*> _clients;
	list<GameClient*> _goneclients;

};
