#include "GameClient.h"
#include "GameHost.h"

#include "../include/rapidjson/document.h"
using namespace rapidjson;

#include "CardGame.h"
#include "SingleGame.h"
#include "GameLobby.h"
#include "CastlePlayer.h"
#include <iostream>
using namespace std;


GameClient::GameClient(GameLobby* lobby) : ClientSocket() 	{
	_game = NULL;
	_lobby = lobby;
	_callback = NULL;
}

void GameClient::ResquestCallback(PlayerResult* cb) {
	_callback = cb;
}


// States
void GameClient::OnString(string s) {
	
	// Allow others to hook in 
	if (_callback != NULL) {
		_callback->Callback(s);
	}

	Document doc;
	doc.Parse<0>(s.c_str());
	if (doc.HasParseError()) {
		// You suck
	}
	if (doc.HasMember("host")) {
		string gameType = doc["host"].GetString();
		if (gameType == "card") {
			if (_game != NULL) {
				// EndGame
				delete _game;
			}
			_game = new CardGame(this);
			_game->Start();
		}
		else if (gameType == "single") {
			if (_game != NULL) {
				// EndGame
				delete _game;
			}
			GameClient* one = one = NULL;
			GameClient* two = NULL;

			if (one == NULL && doc.HasMember("playerone")) {
				one = _lobby->getPlayerByName(doc["playerone"].GetString());
			}
			if (doc.HasMember("playertwo")) {
				two = _lobby->getPlayerByName(doc["playertwo"].GetString());
			}


			_game = new SingleGame(this, one, two);
			_game->Start();

		}
	} 
	// Card Game 
	else if (doc.HasMember("command")) {
		_game->Command(doc["command"].GetString());
	// Single and tournament player
	} else if (doc.HasMember("name")) {
		_name = doc["name"].GetString();
		cout << _name << " has joined the lobby\n";
	}

		
}

string GameClient::getName() {
	return _name;
}

void GameClient::OnDisconnect() {
	// Leave Lobby
	_lobby->leave(this);
}

