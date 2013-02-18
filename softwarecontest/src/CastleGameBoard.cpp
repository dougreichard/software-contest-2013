#include "CastleGameBoard.h"
#include <sstream>
#include <iomanip>

QueueItem NullItem = {Card::empty, 0};

CastleGameBoard::CastleGameBoard(void)
{
	Reset();
}


CastleGameBoard::~CastleGameBoard(void)
{
}

QueueItem popQueue(QueueItem* q, int size) {
	QueueItem ret = q[size-1];
	for(int i=size-1; i > 0; i--)
		q[i] = q[i-1];
	q[0].kind = Card::empty;
	q[0].size = 0;
	return ret;
}

int CastleGameBoard::playTurn(QueueItem& leftPlayer, QueueItem& rightPlayer) {
	int ret = updateTurn(leftPlayer, rightPlayer);
	if (ret != 0) _winner = ret;
	return ret;
}

int CastleGameBoard::updateTurn(QueueItem& leftPlayer, QueueItem& rightPlayer) {
	
	// Process sieges/siege
	// Get bombs at end of flight
	QueueItem bombHit[2];
	bombHit[0] = popQueue(_bombs[0],3);
	bombHit[1] = popQueue(_bombs[1],3);

	// Put the work Items into the field of play
	QueueItem work[2];
	work[0] = popQueue(_workQueue[0],3);
	work[1] = popQueue(_workQueue[1],3);



	// Reset Lose count
	_lostInScirmishes[0]=0;
	_lostInScirmishes[1]=0;
	
	//////////////////////////////////////////////////////////////////
	// Process armies facing each other
	// Process scirmishes
	for(int left=0, right=5; left <6;left++, right--) {
		if (_road[0][left].kind == Card::army && _road[1][right].kind == Card::army) {
			// Equla Size
			if (_road[0][left].size == _road[1][right].size) {
				_lostInScirmishes[0]+= _road[1][right].size;
				_lostInScirmishes[1]+= _road[1][right].size;

				_road[0][left] = NullItem;
				_road[1][right] = NullItem;
			}  else if (_road[0][left].size > _road[1][right].size) {
				// Left wins
				_lostInScirmishes[0]+= _road[1][right].size;
				_lostInScirmishes[1]+= _road[1][right].size;

				_road[0][left].size -= _road[1][right].size;
				_road[1][right] = NullItem;
			} else  {
				// right wins
				_lostInScirmishes[0]+= _road[0][left].size;
				_lostInScirmishes[1]+= _road[0][left].size;

				 _road[1][right].size -= _road[0][left].size;
				_road[0][left] = NullItem;
				
			}
		}
	}

	///////////////////////////////////////////////
	// Move armies along in Queue
	QueueItem armyHit[2];
	armyHit[0] = popQueue(_road[0],7);
	armyHit[1] = popQueue(_road[1],7);

	// Fight at the gate
	if (work[0].kind == Card::army && armyHit[1].kind==Card::army) {
		if (work[0].size == armyHit[1].size) {
			_lostInScirmishes[0] += armyHit[1].size;
			_lostInScirmishes[1] += armyHit[1].size;
			armyHit[1] = NullItem;
			work[0] = NullItem;
		}
		else if (work[0].size > armyHit[1].size) {
			_lostInScirmishes[0] += armyHit[1].size;
			_lostInScirmishes[1] += armyHit[1].size;
			
			work[0].size -= armyHit[1].size;
			armyHit[1] = NullItem;
		} else  {
			_lostInScirmishes[0] += work[0].size;
			_lostInScirmishes[1] += work[0].size;
		
			armyHit[1].size -= work[0].size;
			work[0] = NullItem;
		}

	}
	if (work[1].kind == Card::army && armyHit[0].kind==Card::army) {
		if (work[1].size == armyHit[0].size) {
			_lostInScirmishes[0] += armyHit[0].size;
			_lostInScirmishes[1] += armyHit[0].size;
			armyHit[0] = NullItem;
			work[1] = NullItem;
		}
		else if (work[1].size > armyHit[0].size) {
			_lostInScirmishes[0] += armyHit[0].size;
			_lostInScirmishes[1] += armyHit[0].size;
			
			work[1].size -= armyHit[0].size;
			armyHit[0] = NullItem;
		} else  {
			_lostInScirmishes[0] += work[1].size;
			_lostInScirmishes[1] += work[1].size;
		
			armyHit[0].size -= work[1].size;
			work[1] = NullItem;
		}
	}

	/////////////////////////////////////////
	// Process Work Queue
	for(int i=0;i<2;i++) {
		if (work[i].kind == Card::army)
			_road[i][0] = work[i];
		else if (work[i].kind == Card::siege)
			_bombs[i][0] = work[i];
	}
	
	// Process side by side-scirmishes
	for(int left=0, right=6; left <7;left++, right--) {
		if (_road[0][left].kind == Card::army && _road[1][right].kind == Card::army) {
			// Equla Size
			if (_road[0][left].size == _road[1][right].size) {
				_lostInScirmishes[0]+= _road[1][right].size;
				_lostInScirmishes[1]+= _road[1][right].size;

				_road[0][left] = NullItem;
				_road[1][right] = NullItem;
			}  else if (_road[0][left].size > _road[1][right].size) {
				// Left wins
				_lostInScirmishes[0]+= _road[1][right].size;
				_lostInScirmishes[1]+= _road[1][right].size;

				_road[0][left].size -= _road[1][right].size;
				_road[1][right] = NullItem;
			} else  {
				// right wins
				_lostInScirmishes[0]+= _road[0][left].size;
				_lostInScirmishes[1]+= _road[0][left].size;

				 _road[1][right].size -= _road[0][left].size;
				_road[0][left] = NullItem;
				
			}
		}
	}
	//////////////////////////////////////////////////////////////
	// Add recruit to surving armies, but not work queue added
	for(int i=1; i <7;i++) {
		if (_road[0][i].kind == Card::army) _road[0][i].size++;
		if (_road[1][i].kind == Card::army) _road[1][i].size++;
	}
	

	// Add new work Items
	_workQueue[0][0] = leftPlayer;	
	_workQueue[1][0] = rightPlayer;

	
	//////////////////////////////////////////////////////
	// Process bombs and army hits
	for (int i=0;i<2;i++) {
		_castle[!i] -= armyHit[i].size;
		_castle[!i] -= bombHit[i].size;
	}
	
	bool tied = false;
	// Is there a destruction win
	if (_castle[0] <= 0) {
		if (_castle[0] > _castle[1]) return 1; // Playwer one destoryed, but so is two
		if (_castle[0] < _castle[1]) return 2; // Player two actuall won
		else if (_castle[0] == _castle[1]) {   // Both built and tied
			tied = true;
		}
	} else if (_castle[1] <= 0) return 1;  // Only player two destoried


	if (!tied) {
		// Add Repairs at the end
		for(int i=0;i<2;i++) {
			if (work[i].kind == Card::repair)
				_castle[i] += work[i].size;
		}

	
		// Check for build win
		if (_castle[0] >= 100) {
			if (_castle[0] > _castle[1]) return 1; // Make sure it beat player two
			if (_castle[0] < _castle[1]) return 2; // Player two actuall won
			else if (_castle[0] == _castle[1]) {   // Both built and tied
				tied = true;
			}
		} else if (_castle[1] >= 100) return 2;
	}

	if (tied) {
		// Calc army sizes see if someone wins
		int armySize[2] = {0,0};
		int bombSize[2] = {0,0};

		for (int i=0;i<2;i++) {
			for(int r=0; r<MAX_SIZE;r++) {
				if (r < ROAD_SIZE) armySize[i] += _road[i][r].size;
				if (r < BOMB_SIZE) bombSize[i] += _bombs[i][r].size;

				// Add  in work items
				if (r < WORK_SIZE) {
					if (_workQueue[i][r].kind == Card::army) armySize[i] += _workQueue[i][r].size;
					if (_workQueue[i][r].kind == Card::siege) bombSize[i] += _workQueue[i][r].size;
				}
			}
		}

		// Final tie breakers
		if (armySize[0] > armySize[1]) return 1;
		else if (armySize[0] < armySize[1]) return 2;
		else if (bombSize[0] > bombSize[1]) return 1;
		else if (bombSize[0] < bombSize[1]) return 2;
		else return 3;
	}

	// Keep playing
	return 0;
}


string CastleGameBoard::toString() {
	ostringstream ss;

	ss << "\r\n";
	ss << "\r\n";
	for (int p=0;p<2;p++) {
		ss << "              ]]      [[   ";
		for(int b=0;b<BOMB_SIZE;b++) 
			ss <<'(' << setw(2) << _bombs[p][b].size <<")";
		if (p==0)
			ss << " >> ";
		else
			ss << " << ";
		ss << "   ]]      [[";
		ss << "\r\n";
	}

	ss << "               ] " << setw(3) << _castle[0] <<  "  [                        ] " << setw(3) << _castle[1] << "  ["; 
	ss << "\r\n";

	for (int i=0;i<WORK_SIZE;i++) {
		if (_workQueue[0][i].kind == Card::siege) ss<<'S';
		if (_workQueue[0][i].kind == Card::army) ss<<'A';
		if (_workQueue[0][i].kind == Card::repair) ss<<'R';

		if (_workQueue[0][i].kind == Card::empty) ss << "___";
		else 
			ss << setw(2) << _workQueue[0][i].size;
		ss << " ";
	}

	ss <<	"   ]      [  ";

	for(int r=0;r<ROAD_SIZE;r++) {
		
		if (_road[0][r].kind != Card::empty) ss << setw(2) << _road[0][r].size << '>';
		if (_road[1][(ROAD_SIZE-1)-r].kind != Card::empty) ss << '<' << setw(2) << _road[1][(ROAD_SIZE-1)-r].size;

		if (_road[0][r].kind == Card::empty && _road[1][(ROAD_SIZE-1)-r].kind == Card::empty) ss << "___";

	}

	ss << " ]      [  ";

	for (int i=WORK_SIZE-1; i>=0;i--) {
		if (_workQueue[1][i].kind == Card::siege) ss<<'S';
		if (_workQueue[1][i].kind == Card::army) ss<<'A';
		if (_workQueue[1][i].kind == Card::repair) ss<<'R';

		if (_workQueue[1][i].kind == Card::empty) ss << "___";
		else 
			ss << setw(2) << _workQueue[1][i].size;
		ss << " ";
	}
	ss << "\r\n";
	ss << "\r\n";
	ss << "\r\n";
	
	return ss.str();
}


void CastleGameBoard::Reset()
{
	_winner = 0;
	for (int p=0;p<2;p++) {
		_castle[p] = 50;
		_lostInScirmishes[p] = 0;

		for (int i=0;i<7;i++) {
			_road[p][i] = NullItem;
			if (i>2) continue;
			_bombs[p][i] = NullItem;
			_workQueue[p][i] = NullItem;
			
		}
	}
}



void writeQueue(rapidjson::Writer<::StringStream>& writer, char* name, QueueItem* queue, int count) {
	char s[4];
	writer.String(name);
	writer.StartArray();
	for(int q=0;q<2;q++) {
		writer.StartArray();
		for(int i=0;i<count;i++) {
			writer.StartObject();
			writer.String("kind");
			s[0] = Card::getCharFromType(queue[q*count+i].kind);
			s[1]=0;
			writer.String(s);

			writer.String("impact");
			writer.Int(queue[q*count+i].size);
			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndArray();
}

void CastleGameBoard::toJSon(rapidjson::Writer<::StringStream>& writer) {

	writeQueue(writer, "workQueue", _workQueue[0], WORK_SIZE);
	writeQueue(writer, "siegeQueue", _bombs[0], BOMB_SIZE);
	writeQueue(writer, "road", _road[0], ROAD_SIZE);

	writer.String("castles");
	writer.StartArray();
	writer.Int(_castle[0]);
	writer.Int(_castle[1]);
	writer.EndArray();

	writer.String("winner");
	writer.Int(_winner);
	
	writer.String("lost");
	writer.StartArray();
	writer.Int(this->_lostInScirmishes[0]);
	writer.Int(this->_lostInScirmishes[1]);
	writer.EndArray();

}


	
