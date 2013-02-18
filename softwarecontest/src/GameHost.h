#pragma once

#include <string>
#include "CastleGameBoard.h"
#include "Card.h"


using namespace std;


class GameClient;
class GameHost{
public:
	GameHost(GameClient* host);
	virtual ~GameHost();
	virtual void Start();
	virtual void End();
	virtual void Command(string s);

	void Broadcast(string s);
private:
	GameClient* _host;
};

