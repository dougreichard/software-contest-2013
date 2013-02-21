#include "GameLobby.h"
#include "GameClient.h"


ClientSocket* GameLobby::AllocClient() {
	GameClient* client = new GameClient(this);
	_clients.push_back(client);
	return client;
}
// Create Game
// Client Creates a Single Game
// Create a Card Game
// Run Tournament
void GameLobby::OnString(string s) {
	
}

list<GameClient*>& GameLobby::getClients() {
	return this->_clients;
}

void GameLobby::leave(GameClient* client) {
	_clients.remove(client);
	//_goneclients.remove(client); // in case duplicate
	_goneclients.push_back(client);

}

GameClient* GameLobby::getPlayerByName(string name) {
	if (name=="") return NULL;

	for(list<GameClient*>::iterator i = _clients.begin(); 
		i != _clients.end();
		i++) {
			if ((*i)->getName() == name) 
				return *i;
	}
	return NULL;
}