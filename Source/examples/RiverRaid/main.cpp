/*
 *
 *  Created by Gustavo de Lima, Matheus Alano and Matheus Britzke
 *  Copyright 2018 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>

#include "Game.h"
#include "PlayState.h"

int main(int argc, char **argv)
{
    cgf::Game game(5,30);

	game.init("Framework com estados, sprites e tiling",800,600,false);

    game.changeState(PlayState::instance());

	while(game.isRunning())
	{
		game.handleEvents();
		game.update();
		game.draw();
	}

	// cleanup the engine
	game.clean();

    return 0;
}
