/*
 * controller for cookie clicker game
 */

#ifndef COOKIE_GAME_H
#define COOKIE_GAME_H

#include "building.h"
#include <gint/keyboard.h>
#include <gint/display.h>

// duration of each tick in s
const double TICK = 0.100;

// number of tabs we have
const int NUM_TABS = 4;

// all the tab options available
enum SidebarTab
{
  TAB_BUILDINGS,
  TAB_UPGRADES,
  TAB_OPTIONS,
  TAB_STATS
};

class Game
{
public:
  /* number of cookies we currently have */
  double cookies;
  /* cookies baked by buildings per second */
  double cps;

  /* data about all the building types */
  Building buildings[NUM_BUILDS];

  /*
   * makes a new game
   */
  Game();

  /*
   * click the big cookie
   */
  void click();

  /*
   * attempt to buy a building of type `b`. if we don't have enough cookies, return `false`
   */
  bool buy_building(BuildType b);

  /*
   * switch the sidebar tab to the requested one
   */
  void switch_tab(SidebarTab t);

  /* GAME CONTROLS */

  /*
   * handler for key presses
   */
  void key_pressed(key_event_t k);

  /*
   * advance the game by 1 tick, or `TICK` milliseconds
   * this function should increment cookie count and all that stuff
   */
  void tick();

  /*
   * fully renders the game at its current state
   */
  void render();

private:
  // ticks elapsed since game open (used for animations)
  unsigned int ticks;

  /* ui state */

  SidebarTab sidebar_tab; // active sidebar tab
  int tab_scroll;         // id of the leftmost tab to display

  int sidebar_scroll;     // current scroll progress in sidebar. range is limited to [0, sidebar_scroll_max]
  int sidebar_scroll_max; // max scroll limit, inclusive
  int sidebar_dy;         // visual scroll offset, positive is down

  /* recalculate current cps */
  void calculate_cps();

  /* sidebar rendering functions */

  void render_tab_buildings();
  void render_tab_upgrades();
  void render_tab_options();
  void render_tab_stats();
};

#endif // COOKIE_GAME_H