#include <fxlibc/printf.h>
#include <gint/keyboard.h>

#include "game.h"

int main()
{
	// required for printing floats
	__printf_enable_fp();

	Game game;
	game.cookies = 420e6;
	game.cps = 6.9e3;
	game.buildings[0].buy(15);
	game.buildings[1].buy(6);
	game.buildings[2].buy(1);

	game.render();

	getkey();
	return 1;
}
