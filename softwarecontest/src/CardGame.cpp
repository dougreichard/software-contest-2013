#include "CardGame.h"
#include "../include/rapidjson/writer.h"
#include <iostream>

using namespace std;
using namespace rapidjson;



CardGame::CardGame(GameClient* host) : GameHost(host) {
}
	
void CardGame::End() {

}


void CardGame::Start() {
		// _card.reset(new PushableCard(new Card()));
		_card.fromRandom();
		_card.fromCard(&_card);
		string  resp = _card.toJSon();
		cout << resp;
		cout << _card.toString();
		Broadcast(resp);
		
	}

void CardGame::Command(string s) {
	
	int count = 0;
	if (s=="refill") 
		_card.refill();
	else count = _card.pushOnce(s[0]);

	::StringStream ss;
	Writer<::StringStream> writer(ss); 
	writer.StartObject();
	writer.String("count");
	writer.Int(count);

	_card.toJSon(writer);
	writer.EndObject();

	string  resp = ss.str();
	Broadcast(resp);
}

