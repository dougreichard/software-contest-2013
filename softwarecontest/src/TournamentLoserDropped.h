#include "GameLobby.h"
#include "Socket.h"
#include <map>
#include <vector>

class Record {
	
public:
	Record ();

	int wins;	// Heats not games
	int loses;
	int elminated;
	int castleWins;
	int totalWins;
	int totalLoses;

	int totalCastleWins;
	int totalGameWins;
	int totalGameLoses;
	GameClient* client;
};

struct Heat {
	GameClient* clients[2];
	int wins[2];
	int loses[2];

	int castleWins[2];
};


class ITournament {
public:
	~ITournament (){}
	virtual int gameNum()=0;
	virtual int heatNum()=0;
	virtual void Play()=0;
};

// This is a client socket, but its a true game client not 
class TournamentLoserDrop : public ClientSocket, public ITournament  {
public:
	TournamentLoserDrop(GameLobby* lobby, GameClient* host, int heats);
	void Play();
protected:
	// Overrides
	void OnString(string s);
	virtual void OnConnect();

	// new
	// Tournament continues a number of rounds until a winner
	virtual void OnStartTournament();
	virtual void OnEndTournament();

	// A round runs through a number of heats all players playing each other
	virtual void OnStartRound();
	virtual void OnEndRound();

	// A Heat runs through a number of games with the same players
	virtual void OnStartHeat();
	virtual void OnEndHeat();

	// Game a single game
	virtual void OnStartGame();
	virtual void OnEndGame();


	virtual int gameNum();
	virtual int heatNum();



protected:
	int _round;
	int _heatCount;

	list<GameClient*> players;
	list<GameClient*> allPlayers;
	vector<Heat> _heats;

	map<int, Record> _records;
	GameLobby* _lobby;
	GameClient* _host;

	// heat data
	int _currentGameNum;
	int _currentHeat;
};

