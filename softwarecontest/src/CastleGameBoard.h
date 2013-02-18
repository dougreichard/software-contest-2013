#pragma once

#include "Card.h"
struct QueueItem
{
	Card::cardType kind;
	int size;
};




class CastleGameBoard
{
public:
	CastleGameBoard(void);
	~CastleGameBoard(void);

	void Reset();
	// 0 no winner, 1 = left player, 2 = right player, 3 == tied
	int playTurn(QueueItem& leftPlayer, QueueItem& rightPlayer);

	static const int ROAD_SIZE=7;
	static const int WORK_SIZE=3;
	static const int BOMB_SIZE=3;
	static const int MAX_SIZE=7;   // Use this in loops

	string toString();
	void toJSon(rapidjson::Writer<::StringStream>& writer);

protected:
	int updateTurn(QueueItem& leftPlayer, QueueItem& rightPlayer);

private:
	// work queue
	QueueItem _workQueue[2][WORK_SIZE];
	// Castle
	int _castle[2];
	int _winner;
	// Road 
	QueueItem _road[2][ROAD_SIZE]; // Maybe vary this size
	QueueItem _bombs[2][BOMB_SIZE];
	// Armies lost
	int _lostInScirmishes[2];

};

