#include "Card.h"
#include "../include/rapidjson/document.h"
#include "../include/rapidjson/writer.h"
#include <assert.h>

#include <sstream>

using namespace rapidjson;

std::default_random_engine Card::_generator;
std::uniform_int_distribution<int> Card::_distribution(1,3);


Card::Card(void)
{
	fromRandom();
}

Card::Card(const string& s)
{
	fromString(s);
}

Card::~Card(void)
{
}

// File:
// [
//   {card: "vvvvv,vvvvv,vvvvvv,vvvvvv,vvvvvv};
// ]
void Card::fromString (const string& s) {
	//// R = repair
	///  X = empty
	///  P = Player
	///  A = army 
	///  S = siege
	///
	///  5 Values separated by commas
	bool hasPlayer = false;

	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			Card::cardType c = getTypeFromChar(s[row*6+col]);
			_slots[row][col] = c;
			// Col 6 is comma
			if (c== Card::player) {
				if (hasPlayer) {
					// Error card has two players
				}
				hasPlayer = true;
			}
		}
	}

	if (!hasPlayer) {
		// error no player
	}
}


Card::cardType Card::getTypeFromChar(char c) {
	// Col 6 is comma
	switch (c)
	{
	case 'R':
		return cardType::repair;
	case 'P':
		return cardType::player;
	case 'A':
		return cardType::army;
	case 'S':
		return cardType::siege;

	case 'X':
		return cardType::empty;
		break;


	};
	return cardType::empty;
}


char Card::getCharFromType(Card::cardType c) {
	switch (c)	{
		case Card::siege:  return 'S'; 
		case Card::army:  return'A'; 
		case Card::repair:  return'R'; 
		case Card::player:  return'P'; 
		case Card::empty:  return 'X'; 
		default:
			// Error Bad card
			return ' ';
	};
}


std::string Card::toString (char delim) {
	//// R = repair
	///  X = empty
	///  P = Player
	///  A = army (army)
	///  S = siege (archers)
	///
	///  5 Values separated by commas
	// 25 Chars 4 commas, 0
	char buf[30];
	buf[29]=0;
	char* pBuf = buf;

	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			*pBuf++ = getCharFromType(this->_slots[row][col]);
		}
		if (row<4) *pBuf++ =delim;
	}

	return string(buf);
}



void Card::toJSon(Writer<::StringStream>& writer) {
	writer.String("card");
	
	writer.StartArray();
	// Build card
	for(int row=0;row < 5;row++) {
		writer.StartArray();
		char s[2];
		s[1] = 0;
		for(int col=0;col<5;col++) {
			s[0] = getCharFromType(this->_slots[row][col]);
			writer.String(s);
		}
		writer.EndArray();
	}
	writer.EndArray();
}

std::string Card::toJSon () {
	// Use SaxStyle writer
	::StringStream ss;
	Writer<::StringStream> writer(ss); 
	writer.StartObject();

	toJSon(writer);
	
	writer.EndObject();
	
	
	
	return ss.str();
}

std::string PushableCard::toJSon () {
	// Use SaxStyle writer
	::StringStream ss;
	Writer<::StringStream> writer(ss); 
	writer.StartObject();

	toJSon(writer);
	
	writer.EndObject();
	
	return ss.str();
}

void PushableCard::toJSon (Writer<::StringStream>& writer) {
	writer.String("card");
	
	writer.StartArray();
	// Build card
	char s[2];
	for(int row=0;row < 5;row++) {
		writer.StartArray();
		
		s[1] = 0;
		for(int col=0;col<5;col++) {
			s[0] = getCharFromType(this->_slots[row][col]);
			writer.String(s);
		}
		writer.EndArray();
	}
	writer.EndArray();

	char dirs[5] = "UDLR";
	char pushes[5];
	char * p = pushes;
	for(int i=0;i<4;i++) {
		if (canPush(dirs[i])) 
			*p++ = dirs[i];
	}
	*p=0;
	
	writer.String("canPush");
	writer.String(pushes);

	

	s[0] = getCharFromType(this->_pushType);
	writer.String("pushType");
	writer.String(s);
}

void Card::fromRandom() {
	static std::default_random_engine generator;
	static std::uniform_int_distribution<int> distribution(1,3);
	static std::uniform_int_distribution<int> player(0,4);

	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			int dice_roll = distribution(generator);

			if(1 == dice_roll) this->_slots[row][col] = cardType::siege;
			else if(2 == dice_roll) this->_slots[row][col] = cardType::repair;
			else if(3 == dice_roll) this->_slots[row][col] = cardType::army;
		}
	}

	int playerRow = player(generator);
	int playerCol = player(generator);
	this->_slots[playerRow][playerCol] = cardType::player;
}

bool Card::isValid() {
	bool hasPlayer = false;

	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			cardType current = _slots[row][col];

			if (current != siege && 
				current != army &&
				current != repair && 
				current != player ) return false;
			if (current == player && hasPlayer) return false;
			if (current == player) hasPlayer = true;
		}
	}

	return hasPlayer;
}

PushableCard::PushableCard(const Card* rhs) {
	fromCard(rhs);
}
PushableCard::PushableCard() {
	this->_pushType = cardType::empty;
	fromRandom();
	fromCard(this);//Reset play pos
}

void PushableCard::fromCard(const Card const* rhs) {
	this->_pushType = cardType::empty;

	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			this->_slots[row][col] = rhs->_slots[row][col];
			if (cardType::player == this->_slots[row][col]) {
				this->_row = row;
				this->_col = col;
			}

		}
	}
}


int PushableCard::row() {
	return _row;
}
int PushableCard::col() {
	return _col;
}

bool PushableCard::canPush(char direction) {
	// Nothing pushed

	// If up and top row is same as push type 
	if ('U' == direction ) {
		if (_row==0) return false;
		if (cardType::empty == this->_pushType) return true;
		// find top empty row
		int row = 0;
		for (;row < this->_row; ) {
			if (this->_slots[row][this->_col] != cardType::empty )
				break;
			row++;
		}
		if (this->_slots[row][this->_col] == this->_pushType)
			return true;
		else
			return false;
	}
	// If left and 0 col is same as push type 
	if ('L' == direction) {
		// find top empty row
		if (_col==0) return false;
		if (cardType::empty == this->_pushType) return true;

		int col = 0;
		for (;col< this->_col; ) {
			if (this->_slots[this->_row][col] != cardType::empty )
				break;
			col++;
		}
		if (this->_slots[this->_row][col] == this->_pushType)
			return true;
		else
			return false;
	}
	if ('D' == direction ) {
		// find bottom not empty row
		if (_row==4) return false;
		if (cardType::empty == this->_pushType) return true;

		int row = 4;
		for (;row > this->_row; ) {
			if (this->_slots[row][this->_col] != cardType::empty )
				break;
			row--;
		}
		if (this->_slots[row][this->_col] == this->_pushType)
			return true;
		else
			return false;
	}
	// If left and 0 col is same as push type 
	if ('R' == direction) {
		if (_col==4) return false;
		if (cardType::empty == this->_pushType) return true;

		// find top empty row
		int col = 4;
		for (;col > this->_col; ) {
			if (this->_slots[this->_row][col] != cardType::empty )
				break;
			col--;
		}
		if (this->_slots[this->_row][col] == this->_pushType)
			return true;
		else
			return false;
	}

	return false;
}


int PushableCard::pushOnce(char direction)
{
	if (!canPush(direction)) return 0;

	int count=0;
	if ('U' == direction) {
		// Assumes slot cannot be empty if there is no push type
		if (this->_pushType == cardType::empty)
			this->_pushType = this->_slots[0][this->_col];

		int empty = 0;
		int index = 0;
		cardType newCol[5] = {cardType::empty,cardType::empty,cardType::empty,cardType::empty,cardType::empty}; 
		// copy tail
		for(int row=4;row>this->_row;row--)
			newCol[row] = _slots[row][this->_col];

		// Get eligble push - fill them empty
		for(int row=0;row<this->_row; row++) {
			if (index == 0 && this->_slots[row][this->_col] == this->_pushType) {
				count++;
				this->_slots[row][this->_col] = cardType::empty;
			} 
			else if (this->_slots[row][this->_col] != cardType::empty) {
				newCol[index] = _slots[row][_col];
				index++;
			} else empty++;
		}
		newCol[_row-count-empty] = cardType::player;
		_row = _row-count-empty;

		// Now just copy the new column in
		for(int row=0;row<5; row++) {
			_slots[row][_col] = newCol[row];
		}
		// empty should now have the ammount play needs to move down
	}
	else if ('D' == direction) {
		// Assumes slot cannot be empty if there is no push type
		if (this->_pushType == cardType::empty)
			this->_pushType = this->_slots[4][this->_col];

		int empty = 0;
		int index = 4;
		cardType newCol[5] = {cardType::empty,cardType::empty,cardType::empty,cardType::empty,cardType::empty}; 
		// copy head up to player
		for(int row=0;row<this->_row;row++)
			newCol[row] = _slots[row][this->_col];

		// Get eligble push - fill them empty
		for(int row=4;row>this->_row; row--) {
			if ((index==4) && this->_slots[row][this->_col] == this->_pushType) {
				count++;
				this->_slots[row][this->_col] = cardType::empty;
			} 
			else if (this->_slots[row][this->_col] != cardType::empty) {
				newCol[index] = _slots[row][_col];
				index--;
			} else empty++;
		}
		newCol[_row+count+empty] = cardType::player;
		_row = _row+count+empty;
		// Now just copy the new column in
		for(int row=0;row<5; row++) {
			_slots[row][_col] = newCol[row];
		}
		// empty should now have the ammount play needs to move down

	}
	else if ('L' == direction) {
		// Assumes slot cannot be empty if there is no push type
		if (this->_pushType == cardType::empty)
			this->_pushType = this->_slots[_row][0];

		int empty = 0;
		int index = 0;
		cardType newRow[5] = {cardType::empty,cardType::empty,cardType::empty,cardType::empty,cardType::empty}; 
		// copy tail
		for(int col=4;col>this->_col;col--)
			newRow[col] = _slots[_row][col];

		// Get eligble push - fill them empty
		for(int col=0;col<this->_col; col++) {
			if (index == 0 && this->_slots[_row][col] == this->_pushType) {
				count++;
				this->_slots[_row][col] = cardType::empty;
			} 
			else if (this->_slots[_row][col] != cardType::empty) {
				newRow[index] = _slots[_row][col];
				index++;
			} else empty++;
		}
		newRow[_col-count-empty] = cardType::player;
		_col = _col-count-empty;

		// Now just copy the new column in
		for(int col=0;col<5; col++) {
			_slots[_row][col] = newRow[col];
		}
		// empty should now have the ammount play needs
	}
	else if ('R' == direction) {
		// Assumes slot cannot be empty if there is no push type
		if (this->_pushType == cardType::empty)
			this->_pushType = this->_slots[_row][4];

		int empty = 0;
		int index = 4;
		cardType newRow[5] = {cardType::empty,cardType::empty,cardType::empty,cardType::empty,cardType::empty}; 
		// copy tail
		for(int col=0;col<this->_col;col++)
			newRow[col] = _slots[_row][col];

		// Get eligble push - fill them empty
		for(int col=4;col>this->_col; col--) {
			if (index == 4 && this->_slots[_row][col] == this->_pushType) {
				count++;
				this->_slots[_row][col] = cardType::empty;
			} 
			else if (this->_slots[_row][col] != cardType::empty) {
				newRow[index] = _slots[_row][col];
				index--;
			} else empty++;
		}
		newRow[_col+count+empty] = cardType::player;
		_col = _col+count+empty;

		// Now just copy the new column in
		for(int col=0;col<5; col++) {
			_slots[_row][col] = newRow[col];
		}
		// empty should now have the ammount play needs
	}
	return count;
}

int PushableCard::push(const string& commands) {
	int totalCount = 0;

	for(int i=0;i < commands.length(); i++) {
		int count = pushOnce(commands[i]);
		if (count == 0) return 0; // bad commands
		totalCount += count*(i+1);
	}
	return totalCount;
}

void PushableCard::fromString (const string& s,Card::cardType t) {
	Card card(s);
	fromCard(&card);// Best wayt to reset everything
	_pushType = t;
}

Card::cardType PushableCard::getPushedType() {
	return this->_pushType;
}

void PushableCard::refill() {
	

	this->_pushType = Card::empty;
	
	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			if (_slots[row][col] == Card::empty) {
				int dice_roll = _distribution(_generator);

				if(1 == dice_roll) this->_slots[row][col] = cardType::siege;
				else if(2 == dice_roll) this->_slots[row][col] = cardType::repair;
				else if(3 == dice_roll) this->_slots[row][col] = cardType::army;
			}
			
		}
	}
}


void PushableCard::refill(int player, int game, int heat, int turn) {
	this->_pushType = Card::empty;

	if ((game%3) == 1) {
		player = !player;
	} else if ((game%3) == 2) {
		player = 0;
	} 
	game -= game%3;

	int tileNum = game*heat*15*turn;
	// Player two gets the back of the deck
	if (player) tileNum = tileDeck.size() - 1 - tileNum;

	while (tileNum >= tileDeck.size()) tileNum -= tileDeck.size();
	if (tileNum < 0) tileNum += tileDeck.size();


	for (int row=0;row<5;row++) {
		for (int col=0;col<5;col++) {
			if (_slots[row][col] == Card::empty) {
				_slots[row][col] = tileDeck[tileNum];
				if (player) tileNum--;
				else tileNum++;

				if (tileNum >= tileDeck.size()) tileNum -= tileDeck.size();
				if (tileNum < 0) tileNum += tileDeck.size();
			}
		}
	}
}


// Looks like there is a multiplier
// count * push turn so push 2 turn 1 = 2, push 2 turn 2 = 4, etc.

// Also looks like you do not slide on empties, you have to push




void Card::fillCardDeck(int heatCount, int gameCount){
	cardDeck.clear();
	if (heatCount==0) heatCount = 1;
	if (gameCount==0) gameCount = 1;
	// Generate 2 cards for every 3 games
	// for every heat, generat too many cards
	for (int c=0;c < heatCount* (gameCount*2); c++) {
		shared_ptr<Card> ca(new Card());
			assert(ca->isValid());
		cardDeck.push_back(ca);
	}
}
void Card::fillTileDeck(int heatCount, int gameCount) {
	tileDeck.clear();
	if (heatCount==0) heatCount = 1;
	if (gameCount==0) gameCount = 1;

	for (int c=0;c < heatCount*gameCount*25; c++) {
		int dice_roll = _distribution(_generator);

		if(1 == dice_roll) tileDeck.push_back(cardType::siege);
		else if(2 == dice_roll) tileDeck.push_back(cardType::repair);
		else if(3 == dice_roll) tileDeck.push_back(cardType::army);
		
	}
}


shared_ptr<Card> Card::getCardFromGame(int player, int game, int heat) {
	int cardNum = (game/2)*heat;
	cardNum %= cardDeck.size();
	if (game%3 == 0) {
		// Player one gets the first card
		//  Player two get the second
		// Note: Players are swapped elsewhere
		return cardDeck[cardNum+player];
	} if (game%3 == 1) {
		// Player one gets the first card
		//  Player two get the second
		// Note: Players are swapped elsewhere
		return cardDeck[cardNum+ (!player)];
	} else {
		// Both get same card
		return cardDeck[cardNum];
	}

	return shared_ptr<Card>(NULL);
}

vector<shared_ptr<Card> > Card::cardDeck;
vector<Card::cardType> Card::tileDeck;
