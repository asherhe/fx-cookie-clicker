#include <fxlibc/printf.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include <gint/timer.h>

#include "game.h"

Game game;

// tick routine to run in timer
void timer_tick()
{
	game.tick();
	game.render();
}

int main()
{
	// required for printing floats
	__printf_enable_fp();

	game.render();

	int timer = timer_configure(TIMER_ANY, TICK * 1e6, GINT_CALL(timer_tick));
	timer_start(timer);

	while (1)
	{
		game.key_pressed(getkey());
		game.render();
	}

	return 1;
}
