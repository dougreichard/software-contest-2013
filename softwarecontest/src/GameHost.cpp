#include "GameHost.h"
#include "GameClient.h"


GameHost::GameHost(GameClient* host) : _host(host) {
	//	_host = host;
}
GameHost::~GameHost() {
		End();
}
void GameHost::Start() {}
void GameHost::End() {}
void GameHost::Command(string s) {}

void GameHost::Broadcast(string s) {
	_host->BeginWrite(s); 
}