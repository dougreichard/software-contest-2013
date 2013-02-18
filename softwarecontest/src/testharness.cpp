#include <iostream>
#include <uv.h>
#include <string>
#include <cassert>



using namespace std;

#include "socket.h"

#include "card.h"
#include "CastleGameBoard.h"

#include "../include/rapidjson/document.h"
using namespace rapidjson;

int64_t counter = 0;

void doTest() {
	string testFrom("RRSAS,SSRAR,AASPR,RSASA,RSAAA");
	Card card(testFrom);
	assert(card.isValid());

	string testTo = card.toString();

	assert(testTo == testFrom);

	for (int randoms=0;randoms<5000;randoms++) {
		Card random;
		assert(random.isValid());
	}
	// Quick visual check
	for (int randoms=0;randoms<5;randoms++) {
		Card random;
		assert(random.isValid());
		cout << random.toString() << '\n';
	}


	string fill[]= {
		"SSRAA",
		"SSAAR",
		"RSSAA",


		"SSRAA",

	};

	string play[]= {
		"SSPAA",
		"SSAAP",
		"PSSAA",

		"XSPAX",
	};
	
	string up_left[]= {
		"PXXAA",
		"AAPXX",
		"PSSAA",   // Can move

		"PXXAX",
	};
	string down_right[]= {
		"SSXXP",
		"SSAAP",   // Can't do so results same
		"XXPSS",


		"XSXXP",
	};

	Card::cardType up_left_start_type[]= {
		Card::empty,
		Card::empty, 
		Card::empty,
		Card::siege,
	};
	Card::cardType up_left_end_type[]= {
		Card::siege,
		Card::siege, 
		Card::empty,
		Card::siege,
	};

	Card::cardType down_right_start_type[]= {
		Card::empty,
		Card::empty, 
		Card::empty,
		Card::army,
	};

	Card::cardType down_right_end_type[]= {
		Card::army,
		Card::empty, 
		Card::army,
		Card::army,
	};
	


	// Better push tests

	//// Pushable card testing left right by row
	PushableCard pushable;
	for (int t=0;t<4;t++) {
				
		for (int row=0;row<5;row++)
		{
			string lrStart= "00000,00000,00000,00000,00000";
			string udStart = "00000,00000,00000,00000,00000";
			string lResult= "00000,00000,00000,00000,00000";
			string rResult= "00000,00000,00000,00000,00000";
			string uResult = "00000,00000,00000,00000,00000";
			string dResult = "00000,00000,00000,00000,00000";

			/// Build start and result strings
			for(int r=0;r<5;r++) {
				for(int c=0;c<5;c++) {
					if (r == row) lrStart[r*6+c] = play[t][c];
					else lrStart[r*6+c] = fill[t][c];

					if (r == row) rResult[r*6+c] = down_right[t][c];
					else rResult[r*6+c] = fill[t][c];

					if (r == row) lResult[r*6+c] = up_left[t][c];
					else lResult[r*6+c] = fill[t][c];

					//

					if (r == row) udStart[c*6+r] = play[t][c];
					else udStart[c*6+r] = fill[t][c];

					if (r == row) dResult[c*6+r] = down_right[t][c];
					else dResult[c*6+r] = fill[t][c];

					if (r == row) uResult[c*6+r] = up_left[t][c];
					else uResult[c*6+r] = fill[t][c];
				}
			}
			pushable.fromString(udStart, down_right_start_type[t]);
			bool b = pushable.canPush('D');
			if (!b) {
				assert(udStart[4*6+row] == 'P'); // Doesn't account for empty next to push
			}

			// Reset - Test Left
			pushable.pushOnce('D');
			cout << pushable.toString('\n') << '\n' << '\n';
			assert(pushable.toString() == dResult);
			// Can Push is
			if (b) assert(pushable.getPushedType() == down_right_end_type[t]);
			

			
			pushable.fromString(udStart, up_left_start_type[t]);
			b = pushable.canPush('U');  
			// Can push up P must be in top row
			if (!b) {
				assert(udStart[row] == 'P'); // Doesn't account for empty next to push
			}
			
			// Reset - Test Left
			pushable.pushOnce('U');
			cout << pushable.toString('\n') << '\n' << '\n';
			assert(pushable.toString() == uResult);
			if (b) assert(pushable.getPushedType() == up_left_end_type[t]);

			pushable.fromString(lrStart, up_left_start_type[t]);
			b = pushable.canPush('L');

			if (!b) {
				assert(lrStart[row*6] == 'P'); // Doesn't account for empty next to push
			}
			// Reset - Test Left
			pushable.pushOnce('L');
			cout << pushable.toString('\n') << '\n' << '\n';
			assert(pushable.toString() == lResult);
			if (b) assert(pushable.getPushedType() == up_left_end_type[t]);
	
			// Reset - Test Right
			pushable.fromString(lrStart, down_right_start_type[t]);
			b = pushable.canPush('R');
			if (!b) {
				assert(lrStart[row*6+4] == 'P'); // Doesn't account for empty next to push
			}
			pushable.pushOnce('R');
			cout << pushable.toString('\n') << '\n' << '\n';
			assert(pushable.toString() == rResult);
			if (b) assert(pushable.getPushedType() == down_right_end_type[t]);
		}
	}
	
	int count = 0;
	int totalCount = 0;

	pushable.fromString("RRRRR,RRRAR,RRARR,RRRRR,PRASR");
	string commands = "URDLURL";
	int counts[] = {4,4,4,1,1,1,1};

	for(int i=0;i<7; i++) {
		count = pushable.pushOnce(commands[i]);
		assert(count == counts[i]);
		totalCount += count;
		cout << pushable.toString('\n') << '\n' << '\n';
	}
	assert(!pushable.canPush('U'));
	assert(!pushable.canPush('D'));
	assert(!pushable.canPush('L'));
	assert(!pushable.canPush('R'));
	assert(totalCount==16);

	// Spirals test various types of pushes with each type
	string spirals[]= {
		"RRRRR,RRRAR,RRARR,RRRRR,PRASR",
		"AAAAA,AAARA,AARAA,AAAAA,PARSA",
		"SSSSS,SSSAS,SSASS,SSSSS,PSARS"
	};
	for(int i=0;i<3;i++) {
		pushable.fromString(spirals[i]);
		count = pushable.push(commands);
		assert(count == totalCount);
	}

	pushable.fromString("RRRRR,RRRAR,ARPRS,ARRRS,RRASR");
	assert(pushable.canPush('U'));
	assert(pushable.canPush('D'));
	assert(pushable.canPush('L'));
	assert(pushable.canPush('R'));

	assert(pushable.pushOnce('D')==1);
	assert(!pushable.canPush('U'));
	assert(!pushable.canPush('D'));
	assert(pushable.canPush('L'));
	assert(!pushable.canPush('R'));

	
		
	string strings[5]= {
		"RASSA,RRAAS,SAARR,AARSS,RASAR",
		"RRRRR,RRRRR,RRRRR,RRRRR,RRRRR",
		"AAAAA,AAAAA,AAAAA,AAAAA,AAAAA",
		"SSSSS,SSSSS,SSSSS,SSSSS,SSSSS",
		"AAAAA,SSSSS,SAAAS,AASSS,ASASA"};

	// Move the player every where testing boundaries
	for(int test=0;test <5; test++){
		for(int row=0; row<5;row++)	{
			for(int col=0;col<5;col++) {
				string s = strings[test];
				s[row*6+col] = 'P';

				pushable.fromString(s);

				assert((row!=0) == pushable.canPush('U'));
				assert((row!=4) == pushable.canPush('D'));
				assert((col!=0) == pushable.canPush('L'));
				assert((col!=4) == pushable.canPush('R'));
			}
		}
	}

	/// Push once tests
	for(int test=2;test <4; test++){
		for(int row=0; row<5;row++)	{
			for(int col=0;col<5;col++) {
				string s = strings[test];
				
				// Test 4 test all the corners
				if (test>=4) {
					if (row==4) s[row*6+0] = 'R';
					else if (row==0) s[row*6+4] = 'R';
					else if (col != 0) s[0+col] = 'R';
					else s[4*6+col] = 'R';
				}
				s[row*6+col] = 'P';

				pushable.fromString(s);

				if (test == 4) {
					// Push it once, but you shouldn't be able to push again
					int count = 0;
					if (row==0) count = pushable.pushOnce('D');
					else if (row==4) count = pushable.pushOnce('U');
					else if (col != 0) count = pushable.pushOnce('L');
					else count = pushable.pushOnce('R');
					assert(count == 1);

					assert(!pushable.canPush('U'));
					assert(!pushable.canPush('D'));
					assert(!pushable.canPush('L'));
					assert(!pushable.canPush('R'));
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	// Test game
	CastleGameBoard g;
	int winner = 0;

	QueueItem siege= {Card::siege, 1};
	QueueItem army = {Card::army, 1};
	QueueItem repair = {Card::repair, 1};
	QueueItem empty = {Card::empty, 1};

	QueueItem* player0[] = {&siege,&army,&repair, &empty, &empty,&empty};
	QueueItem* player1[] = {&empty, &empty,&empty,&siege,&army,&repair};
	int winners[] = {1,1,1,2,2,2};

	
	for(int scenario=0; scenario<6;scenario++) {
		g.Reset();
		for(int i=0;i<1000;i++) {
			cout << g.toString();
		
			winner = g.playTurn(*player0[scenario],*player1[scenario]);
			if (winner== 1) cout << "Player one wins" << g.toString();
			if (winner== 2) cout << "Player two wins" << g.toString();
			if (winner== 3) cout << "tie" << g.toString();
			if (winner!= 0) break;
		}
		assert(winner==winners[scenario]);
	}
	
	cin.get();

}
