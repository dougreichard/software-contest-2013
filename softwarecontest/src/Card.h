#pragma once

#include <string>
#include <sstream>
#include <random>
#include "../include/rapidjson/writer.h"
using namespace std;

class StringStream {
public:
	void Put(char c) { ss << c; }

	// Not implemented
	char* PutBegin() { return 0; }
	size_t PutEnd(char*) { return 0; }
	string str() {return ss.str();}

private:
	ostringstream ss;
};


class Card
{
public:
	Card(void);
	Card(const string& s);
	virtual ~Card(void);

	enum cardType {empty =0, army=1, siege=2, repair=3, player=7};

	cardType _slots[5][5];

	void fromString (const string& s);
	void fromRandom();
	
	string toString(char delim = ',');
	string toJSon();
	void toJSon(rapidjson::Writer<::StringStream>& w);
	bool isValid();

	static char getCharFromType(Card::cardType c);
	static cardType getTypeFromChar(char c);

	static std::default_random_engine _generator;
	static std::uniform_int_distribution<int> _distribution;
};


class PushableCard : public Card
{
	cardType _pushType;
	// where is the player
	int _row;
	int _col;
	
public:
	int row();
	int col();

	PushableCard(const Card* card);
	PushableCard();

	void fromCard(const Card* rhs);
	void fromString (const string& s, Card::cardType t = Card::empty);
	string toJSon();
	void toJSon(rapidjson::Writer<::StringStream>& w);
	

	bool canPush(char direction);
	int pushOnce(char direction);

	int push(const string& s);
	cardType getPushedType();

	void refill();
	
};
