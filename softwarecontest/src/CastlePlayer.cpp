#include "CastlePlayer.h"
#include "Card.h"
#include "GameClient.h"

char pickDir(PushableCard& card, bool up, bool down, bool left, bool right) {

	// for now be dumb 
	if(up) return 'U';
	if(left) return 'L';
	if(down) return 'D';
	if(right) return 'R';
}

string getPushes(Card& card, Card::cardType first, Card::cardType second, Card::cardType third) {
	PushableCard p(&card);

	int row = p.row();
	int col = p.col();
	int count=0;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	char s[30];

	int a[] = {first, second, third};
	int desired=Card::empty;

	// Find any play 
	for (int i=0;i<3;i++) {
		desired = a[i];

		if (card._slots[0][col] == desired) up = true;
		if (card._slots[4][col] == desired) down = true;
		if (card._slots[row][0] == desired) left = true;
		if (card._slots[row][4] == desired) right = true;

		if (up || down || left || right) break;
	}
		
	if (desired==Card::empty)  assert(false);;

	// Try as long as there are pushes
	do {
		
		up = (card._slots[0][col] == desired);
		down = (card._slots[4][col] == desired);
		left = (card._slots[row][0] == desired);
		right = (card._slots[row][4] == desired);

		if (!up && !down && !left && !right) break;

		s[count] = pickDir(p, up,down,left,right);
		s[count+1] = 0;
		if (p.pushOnce(s[count]) == 0) {
			s[count] = 0;
			break;
		}
		row = p.row();
		col = p.col();
	
		

	} while (count++ < 25);

	string ret(s);
	return ret;
}




// Favors repair, Siege then Army - but fairly dumb no look ahead
void BoomPlayer::getPushes(Card& card, PlayerResult* cb) {

	string s = ::getPushes(card, Card::repair, Card::army, Card::siege);
	cb->Callback(s);

}
void BoomPlayer::startGame(string s,PlayerResult* cb) {
	cb->Callback("ready");
}
void BoomPlayer::gameState(string s,PlayerResult* cb) {
	cb->Callback("ready");
}


// Favors Army, Then Siege then Repair - but fairly dumb no look ahead
void RushPlayer::getPushes(Card& card, PlayerResult* cb) {
	string s = ::getPushes(card, Card::army, Card::siege, Card::repair);
	cb->Callback(s);

}
void RushPlayer::startGame(string s, PlayerResult* cb) {
	cb->Callback("ready");
}

void RushPlayer::gameState(string s, PlayerResult* cb) {
	cb->Callback("ready");
}



SocketPlayer::SocketPlayer(GameClient* s) {
	_socket = s;
}


// Um this has to be Async
// So Call back of read needs to do something
void SocketPlayer::getPushes(Card& card, PlayerResult* cb) {
	_socket->ResquestCallback(cb);

	string s = card.toJSon();

	_socket->BeginWrite(s);
}
void SocketPlayer::startGame(string s, PlayerResult* cb) {
	// Should just say 'ready'
	_socket->ResquestCallback(cb);
	_socket->BeginWrite(s);
}

void SocketPlayer::gameState(string s, PlayerResult* cb) {
	// Should just say 'ready'
	_socket->ResquestCallback(cb);
	_socket->BeginWrite(s);
}


string BoomPlayer::getName() {
	return "BOOM";
}

string RushPlayer::getName() {
	return "Rush";
}

string SocketPlayer::getName() {
	return _socket->getName();
}

int BoomPlayer::getID() {
	return (int)this;;
}

int RushPlayer::getID() {
	return (int)this;
}

int SocketPlayer::getID() {
	return _socket->getID();
}