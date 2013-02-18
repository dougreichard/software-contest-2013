#pragma once

#include "GameHost.h"


class TournamentGame : public GameHost {
public:
	TournamentGame(GameClient* host) : GameHost(host) {}

};