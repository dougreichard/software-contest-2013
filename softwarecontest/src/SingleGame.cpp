#include "SingleGame.h"
#include <iostream>
#include "../include/rapidjson/writer.h"
#include "../include/rapidjson/document.h"

#include "CastlePlayer.h"
#include "TournamentLoserDropped.h"

using namespace std;
using namespace rapidjson;



SingleGame::SingleGame(GameClient* host, GameClient* one, GameClient* two, ITournament * tourny) : GameHost(host) {


	_players[0] = (one == NULL) ? (Player*)new BoomPlayer() : new SocketPlayer(one);
	_players[1] = (two == NULL) ? (Player*)new RushPlayer() : new SocketPlayer(two);
	_state = 0;

	_waiting[0] = NULL;
	_waiting[1] = NULL;
	_arrived[0] = NULL;
	_arrived[1] = NULL;

	_tournament = tourny;
	_turnCount = 0;

}
SingleGame::~SingleGame() {
	// We allocate
	delete _players[0];
	delete _players[1];
}

void SingleGame::End() {

}


// USe same call back for Start and Step
class GamePlayerResult : public PlayerResult{
	string _results;
	SingleGame* _game;
	int _state;
public:
	GamePlayerResult (SingleGame* that, int state);
	virtual void Callback(string s);
	string Results();
};

GamePlayerResult::GamePlayerResult (SingleGame* that, int state) {
	_game = that;
	_state = state;
}
void GamePlayerResult::Callback(string s) {
	_results = s;

	if (_state == SingleGame::GameStart) 
		_game->PlayerStartCallback(this);
	else if (_state == SingleGame::GameState) 
		_game->PlayerStateCallback(this);
	else 
		_game->PlayerStepCallback(this);
}
string GamePlayerResult::Results() {
	return _results;
}


void SingleGame::Start() {

	if (_waiting[0] != NULL || _waiting[1] != NULL) return;

	_waiting[0] = new GamePlayerResult(this, gameState::GameStart);
	_waiting[1] = new GamePlayerResult(this, gameState::GameStart);
	_arrived[0] = NULL;
	_arrived[1] = NULL;

	if (_tournament != NULL) {
		_turn[0].fromCard( Card::getCardFromGame(0,_tournament->gameNum(), _tournament->heatNum() ).get());
		_turn[1].fromCard(Card::getCardFromGame(1,_tournament->gameNum(), _tournament->heatNum() ).get());

	} else {
		_turn[0].fromRandom();
		_turn[0].fromCard(&_turn[0]);
		_turn[1].fromRandom();
		_turn[1].fromCard(&_turn[1]);
	}
	_turnCount = 0;
	//cout << _players[0]->getName() << " VS. " << _players[1]->getName();
	//cout << " lets get ready to crumble\n";



	
	for (int p=0;p<2;p++)  {
		::StringStream ss;	
		Writer<::StringStream> writer(ss); 
				
		writer.StartObject();
		// Send other players ID
		writer.String("id");
		writer.Int(_players[!p]->getID());

		writer.String("player");
		writer.Int(p+1);

		writer.EndObject();
		string  resp = ss.str();
		//cout << resp;

		_players[p]->startGame(resp, _waiting[p]);
	}
}


void SingleGame::Command(string s) {

	// Error out of order commanding?
	if (_waiting[0] != NULL || _waiting[1] != NULL) return;

	_waiting[0] = new GamePlayerResult(this, gameState::GameStep);
	_waiting[1] = new GamePlayerResult(this, gameState::GameStep);
	_arrived[0] = NULL;
	_arrived[1] = NULL;

	if (_tournament != NULL) {
		_turn[0].refill(0, _tournament->gameNum(),_tournament->heatNum(), _turnCount );
		_turn[1].refill(1, _tournament->gameNum(),_tournament->heatNum(), _turnCount );
		_turnCount++;
	} else {
		_turn[0].refill();
		_turn[1].refill();
	}

	//_turn[0].fromRandom();
	//_turn[0].fromCard(&_turn[0]);
	//_turn[1].fromRandom();
	//_turn[1].fromCard(&_turn[1]);

	_players[0]->getPushes(_turn[0], _waiting[0]);
	_players[1]->getPushes(_turn[1], _waiting[1]);
	
}

void SingleGame::SendGameState(string s) {

	// Error out of order commanding?
	if (_waiting[0] != NULL || _waiting[1] != NULL) return;

	_waiting[0] = new GamePlayerResult(this, gameState::GameState);
	_waiting[1] = new GamePlayerResult(this, gameState::GameState);
	_arrived[0] = NULL;
	_arrived[1] = NULL;


	_players[0]->gameState(s, _waiting[0]);
	_players[1]->gameState(s, _waiting[1]);
	
}


void SingleGame::PlayerStepCallback(PlayerResult* arrival) {

	if (_arrived[0] == NULL) {
		_arrived[0] = arrival;
		//cout << "P1";
		return;
	} else if (_arrived[1] == NULL) {
		// Error
		_arrived[1] = arrival;
		//cout << "P2";
	}
	
	// Both have arrived
	string pushes[2];
	pushes[0] = _waiting[0]->Results().c_str();
	pushes[1] = _waiting[1]->Results().c_str();
	

	for(int i =0;i<2;i++) {
		Document doc;
		doc.Parse<0>(_waiting[i]->Results().c_str());
		if (doc.HasParseError()) {
			// You suck
		} else if (doc.HasMember("push")) {
			pushes[i] = doc["push"].GetString();
		}
	}
	
	int countL = _turn[0].push(pushes[0]);
	int countR = _turn[1].push(pushes[1]);

	// Cleanup
	for(int i=0;i<2;i++) {
		delete _waiting[i];
		_waiting[i] = NULL;
		_arrived[i] = NULL;
	}

	QueueItem left = {_turn[0].getPushedType(), countL};
	QueueItem right = {_turn[1].getPushedType(), countR};

	int winner = this->_board.playTurn(left,right);
	
	// Game State to host
	::StringStream ss;
	Writer<::StringStream> writer(ss); 
	writer.StartObject();

	_board.toJSon(writer);
	writer.EndObject();
	string  resp = ss.str();
	//cout << resp;

	// SendGame State to players
	SendGameState(resp);
}


void SingleGame::PlayerStartCallback(PlayerResult* arrival) {

	if (_arrived[0] == NULL) {
		_arrived[0] = arrival;
		return;
	} if (_arrived[1] != NULL) {
		// Error
	}
	
	// Both have arrived
	string one = _waiting[0]->Results();
	string two = _waiting[1]->Results();

	// Cleanup
	for(int i=0;i<2;i++) {
		delete _waiting[i];
		_waiting[i] = NULL;
		_arrived[i] = NULL;
	}

	/// Both players ready, lets begin
		::StringStream ss;
		Writer<::StringStream> writer(ss); 
				
		writer.StartObject();

		_board.toJSon(writer);
		writer.EndObject();
		
		string  resp = ss.str();
		//cout << resp;
	
		// SendGame State to players
		SendGameState(resp);
}


void SingleGame::PlayerStateCallback(PlayerResult* arrival) {

	if (_arrived[0] == NULL) {
		_arrived[0] = arrival;
		return;
	} if (_arrived[1] != NULL) {
		// Error
	}
	
	// Both have arrived
	string one = _waiting[0]->Results();
	string two = _waiting[1]->Results();

	// Cleanup
	for(int i=0;i<2;i++) {
		delete _waiting[i];
		_waiting[i] = NULL;
		_arrived[i] = NULL;
	}
	// Send host GameState so it knows and can send the next card
	::StringStream ss;
	Writer<::StringStream> writer(ss); 
				
	writer.StartObject();

	_board.toJSon(writer);
	writer.EndObject();
		
	string  resp = ss.str();
	//cout << resp;
	Broadcast(resp);
}



