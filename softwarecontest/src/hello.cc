#include <iostream>
#include <uv.h>


using namespace std;

#include "GameLobby.h"

extern void doTest();

GameLobby lobby;
void start_game(uv_loop_t* loop) {
	lobby.Listen(loop);
}

int main ()
{
	//	doTest();
	//if (true) return 0;

	uv_loop_t* loop = uv_loop_new();
	
	start_game(loop);
	
	cout << "Hello World!" << '\n';
	uv_run(loop, UV_RUN_DEFAULT);
	cout << "Goodbye!" << '\n';

	
	
	return 0;
}

