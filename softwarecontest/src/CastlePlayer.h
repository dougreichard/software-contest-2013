#pragma once


#include <string>
using namespace std;

class Card;


class PlayerResult {
public:
	virtual void Callback(string s)=0;
};


class Player {

public:
	virtual ~Player(){}
	virtual void gameState(string s, PlayerResult* cb)=0;  // Send the game state
	virtual void getPushes(Card& card, PlayerResult* cb)=0;
	virtual void startGame(string s, PlayerResult* cb)=0;  // Send the game state

	virtual int getID()=0;
	virtual string getName()=0;
};

class BoomPlayer : public Player {

public:
	virtual void gameState(string s, PlayerResult* cb);  // Send the game state
	virtual void getPushes(Card& card, PlayerResult* cb);
	virtual void startGame(string s, PlayerResult* cb);  // Send the game state

	virtual int getID();
	virtual string getName();
};

class RushPlayer : public Player{

public:
	virtual void gameState(string s, PlayerResult* cb);  // Send the game state
	virtual void getPushes(Card& card, PlayerResult* cb);
	virtual void startGame(string s, PlayerResult* cb);  // Send the game state

	virtual int getID();
	virtual string getName();
};

class GameClient;
class SocketPlayer : public Player{
	GameClient*_socket;
public:
	SocketPlayer(GameClient* socket);
	virtual void gameState(string s, PlayerResult* cb);  // Send the game state
	virtual void getPushes(Card& card, PlayerResult* cb);
	virtual void startGame(string s, PlayerResult* cb);  // Send the game state

	virtual int getID();
	virtual string getName();
};


