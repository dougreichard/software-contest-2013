#include "TournamentLoserDropped.h"
#include "SingleGame.h"
#include "GameClient.h"
#include <iostream>
#include <iomanip>

#include "../include/rapidjson/document.h"
#include "../include/rapidjson/writer.h"
using namespace rapidjson;

Record::Record () {
	elminated = 0;
	wins = 0;
	loses= 0;
	castleWins = 0;
	totalWins = 0;
	totalLoses = 0;
	totalGameWins = 0;
	totalGameLoses = 0;
	totalCastleWins = 0;
	client = NULL;;

}

/////////////////////////////////////////////////////////
/// Starts the tournament by connecting to the lobby
/// Assumes all players are in the lobby
void TournamentLoserDrop::Play() {
	// I'm connecting to the lobby via sockets
	// in the same process on the same loop. Craziness. but not when you're non-blocking
	this->BeginConnect(_lobby->getSocket()->loop, "127.0.0.1", 8124);
}

////////////////////////////////////////////////////////
// Once connected 
void TournamentLoserDrop::OnConnect() {
	// Instead of passing the lobby I could have a qerest to list the players in GameClient
	// but I won't for now
	OnStartTournament();
}

TournamentLoserDrop::TournamentLoserDrop(GameLobby* lobby, GameClient* host, int heats) {
	_heatCount = heats;
	_lobby = lobby;
	_round = 0;
	_host = host;

	while (!(_heatCount %2)) {
		_heatCount ++;
		while(_heatCount%3 ) {
			cout << _heatCount << "moving to multiple of 3 and odd" << "\n";
			_heatCount ++;
		}
	}

}
////////////////////////////////////////////////////////////////////////////
/// 
void TournamentLoserDrop::OnStartTournament() {
	// Build a list of clients
	// Clear records
	// cout << "Tournament Start";
	_round = 0;

	for(list<GameClient*>::iterator i =  _lobby->getClients().begin(); 
		i != _lobby->getClients().end();
		i++) {
			if((*i)->getName() != "" ) {
				players.push_back((*i));
				allPlayers.push_back((*i));
			}
	}
	
	
	OnStartRound();
}

// A round runs through a number of heats all players playing each other
void TournamentLoserDrop::OnStartRound() {
	//cout << "Start of round " << _round << "\n";;
	_heats.clear();
	

	/// clear records
	for(list<GameClient*>::iterator remaining = players.begin(); 
		remaining != players.end();
		remaining++) {

		_records[(*remaining)->getID()].wins=0;
		_records[(*remaining)->getID()].loses=0;
		_records[(*remaining)->getID()].castleWins=0;
		_records[(*remaining)->getID()].client = *remaining;
	}
	/// Build a list of games for this round
	for(list<GameClient*>::iterator outer = players.begin(); 
		outer != players.end();
		outer++) {

		list<GameClient*>::iterator inner = outer;
		inner++;
		for(; 
			inner != players.end();
			inner++) {

				if (outer == inner) continue;

				Heat heat;
				heat.clients[0] = *outer;
				heat.clients[1] = *inner;
				heat.wins[0] = heat.wins[1] = 0;
				heat.loses[0] = heat.loses[1] = 0;
				heat.castleWins[0] = heat.castleWins[1] = 0;
				
				_heats.push_back(heat);


		}
	}

	Card::fillCardDeck(_heats.size(), this->_heatCount);
	Card::fillTileDeck(_heats.size(), this->_heatCount);

	_currentHeat = 0;	
	OnStartHeat();

}


// A Heat runs through a number of games with the same players
void TournamentLoserDrop::OnStartHeat() {
	//cout << "Start of heat" << _currentHeat << "\n";;
	stringstream msg;
	msg << '\n' << "Round: " << _round << "heat: " << _currentHeat << " " <<
		_heats[_currentHeat].clients[0]->getName() << " vs " <<
		_heats[_currentHeat].clients[1]->getName() << "\n";
	// cout << msg.str();
	// cout << '\n';
	//_host->BeginWrite(msg.str());

	_currentGameNum = 0;
	OnStartGame();
}

class MyStringStream {
public:
	void Put(char c) { ss << c; }

	// Not implemented
	char* PutBegin() { return 0; }
	size_t PutEnd(char*) { return 0; }
	string str() {return ss.str();}

private:
	ostringstream ss;
};

// Game a single game
void TournamentLoserDrop::OnStartGame() {
	// cout << "Start of Game" << _currentGameNum << "\n";;

	string one = _heats[_currentHeat].clients[0]->getName();
	string two = _heats[_currentHeat].clients[1]->getName();
	_host->BeginWrite(".");

	
	
	// Send start game Message
	// For Writing JSOn
	MyStringStream ss;
	Writer<MyStringStream> writer(ss);

	// Parse Json
	writer.StartObject();
	writer.String("host");
	writer.String("single");

	writer.String("playerone");
	writer.String(one.c_str());

	writer.String("playertwo");
	writer.String(two.c_str());
	writer.EndObject();

	BeginWrite(ss.str());
	BeginRead();
}



// Get game state see if there is a winner
// If not, step the game
void TournamentLoserDrop::OnString(string s) {
		// For Writing JSOn
		MyStringStream ss;
		Writer<MyStringStream> writer(ss);

		// Parse Json
		Document doc;
		doc.Parse<0>(s.c_str());
		if (doc.HasParseError()) {
			// ERROR 
		}  else if (doc.HasMember("winner")) {
			// Got Game State
			//cout << "Game State" << s;
			int winner = doc["winner"].GetInt();
			if (winner == 0) {
			
				writer.StartObject();
				writer.String("command");
				writer.String("step");
				writer.EndObject();

				BeginWrite(ss.str());
			} else {
				int winnerIndex = (winner -1);

				const Value& c = doc["castles"];
				int castle = c[SizeType(winnerIndex)].GetInt();
				if (castle >= 100) 
					_heats[_currentHeat].castleWins[winnerIndex]++;

				_heats[_currentHeat].wins[winnerIndex]++;
				_heats[_currentHeat].loses[!winnerIndex]++;
/*
				cout << '\r' << " Rnd: " << _round+1 << " Heat: " << _currentHeat+1 << " game {" << _currentGameNum+1 << ")  Winner: " << winner << " "
					<<_heats[_currentHeat].clients[0]->getName()
					<<'[' << _heats[_currentHeat].wins[0] << ']'
					<< " ==== " <<_heats[_currentHeat].clients[1]->getName()
					<<'[' << _heats[_currentHeat].wins[1] << ']';
					
					*/

				// Check for caster win
				OnEndGame();
			}

		} else {
			//cout << "Game bad data??" << s;
		}
	}


void TournamentLoserDrop::OnEndGame() {
	// cout << "End of game" << _currentGameNum << "\n";

	_currentGameNum++;
		
	if (_currentGameNum >= _heatCount) OnEndHeat();
	else OnStartGame();

}

void TournamentLoserDrop::OnEndHeat() {
	
	// cout << "End of heat" << _currentHeat << "\n";;

	// Update Record cumlative totals
	for (int p=0;p<2;p++) {
		int id = _heats[_currentHeat].clients[p]->getID();
		_records[id].totalGameWins+= _heats[_currentHeat].wins[p];
		_records[id].totalCastleWins+=_heats[_currentHeat].castleWins[p];
		_records[id].castleWins =_heats[_currentHeat].castleWins[p];
		_records[id].totalGameLoses+= _heats[_currentHeat].loses[p];
	}
	// Player one wins
	if (_heats[_currentHeat].wins[0] > _heats[_currentHeat].wins[1]) {
		int id = _heats[_currentHeat].clients[0]->getID();
		_records[id].wins++;
		id = _heats[_currentHeat].clients[1]->getID();
		_records[id].loses++;
	// check for player two win
	} else if (_heats[_currentHeat].wins[0] < _heats[_currentHeat].wins[1]) {
		int id = _heats[_currentHeat].clients[1]->getID();
		_records[id].wins++;
		id = _heats[_currentHeat].clients[0]->getID();
		_records[id].loses++;
	// Tie is lose
	} else if (_heats[_currentHeat].wins[0] == _heats[_currentHeat].wins[1]) {
		int id = _heats[_currentHeat].clients[0]->getID();
		_records[id].loses++;
		id = _heats[_currentHeat].clients[1]->getID();
		_records[id].loses++;
	}

	// Move to the next match up
	_currentHeat++;
	// No more matchups
	if (_heats.size() <= _currentHeat) {
		OnEndRound();
	}
	else OnStartHeat();
}


void TournamentLoserDrop::OnEndRound() {
	vector<int> removals;
	int leastWins = 99999999;

	cout << "End of round " << _round << "\n";;

	// Find the least wins
	for( map<int, Record>::iterator i = _records.begin();
		i != _records.end();
		i++)
	{
		if (i->second.elminated) continue;
		leastWins = min(i->second.wins, leastWins);

		i->second.totalWins+= i->second.wins;
		i->second.totalLoses += i->second.loses;
	}
	stringstream msg;

	msg << setw(15) <<
			"name" << 
			setw(7) <<
			"gone" << 
			setw(7) <<
			"W" << 
			setw(7) <<
			"L" << 
			setw(7) <<
			"cW" << 
			setw(7) <<
			"TW" << 
			setw(7) <<
			"TL" << 
			setw(7) <<
			"TGW" << 
			setw(7) <<
			"TGL" << 
			setw(7) <<
			"TcW" <<
			"\r\n";

	// Gather all the players with that win total
	for( map<int, Record>::iterator i = _records.begin();
		i != _records.end();
		i++)
	{

		if (!i->second.elminated && (leastWins == i->second.wins)) {
			i->second.elminated = _round+1;
			removals.push_back(i->first);
		}


		msg << setw(15) <<
			i->second.client->getName() << 
			setw(7) <<
			i->second.elminated << 
			setw(7) <<
			i->second.wins << 
			setw(7) <<
			i->second.loses << 
			setw(7) <<
			i->second.castleWins << 
			setw(7) <<
			i->second.totalWins << 
			setw(7) <<
			i->second.totalLoses << 
			setw(7) <<
			i->second.totalGameWins << 
			setw(7) <<
			i->second.totalGameLoses << 
			setw(7) <<
			i->second.totalCastleWins <<
			"\r\n";


			i->second.wins=0;
			i->second.loses=0;
			i->second.castleWins=0;

	}

	_host->BeginWrite(msg.str());

	for (int i=0; i<removals.size(); i++) {
		int idToRemove = removals[i];
		// Now remove them
		for(list<GameClient*>::iterator remaining = players.begin(); 
			remaining != players.end();
			) {

			if ((*remaining)->getID()==idToRemove)
				remaining = players.erase(remaining);
			else remaining++;
		}
	}


	
	if (players.size() == 1) 
		cout << "Tournament End";
	else {
		this->_round++;
		OnStartRound();
	}
}

void TournamentLoserDrop::OnEndTournament() {

}



int TournamentLoserDrop::gameNum() {
	return _currentGameNum;
}
int TournamentLoserDrop::heatNum() {
	return _currentHeat;
}