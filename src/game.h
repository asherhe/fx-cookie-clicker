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

/* window that spans entire sidebar. dimensions: 20x6 chars */
const struct dwindow SIDEBAR_WINDOW = {46, 9, 126, 55};
/* window for buildings specifically */
const struct dwindow BUILDINGS_WINDOW = {46, 19, 124, 55};

class Game
{
public:
  /* number of cookies we currently have */
  double cookies;
  /* total number of cookies we have ever baked */
  double cookies_all;
  /* cookies baked by buildings per second */
  double cps;

  /* data about all the building types */
  Building buildings[NUM_BUILDS];

  /*
   * makes a new game
   */
  Game();

  /*
   * add cookies to our balance
   * (also increases all-time cookie count)
   */
  void operator+=(double v);

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

  /*
   * scroll to the i-th item in the sidebar
   */
  void sidebar_scroll(int i);

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

  int sidebar_sel;               // currently selected item in sidebar. range is limited to [0, sidebar_sel_max]
  int sidebar_sel_max;           // max scroll limit, inclusive
  struct dwindow sidebar_window; // dwindow that covers the rendering region for the sidebar items
  int sidebar_item_height;       // vertical displacement between each item in the sidebar
  int sidebar_item_maxy;         // maximum y-position the base of each sidebar item can be to remain onscreen
  int sidebar_dy;                // visual scroll offset

  int unlocked_buildings; // number of buildings visible in the UI

  enum
  {
    MSG_NONE,     // no message box
    MSG_UPG_INFO, // shows information about the currently selected upgrade
  } message_type; // the state of the message box display

  /* recalculate current cps */
  void calculate_cps();

  /* sidebar rendering */
  void render_tab_buildings();
  void render_tab_upgrades();
  void render_tab_options();
  void render_tab_stats();

  /* message box rendering */
  void render_message_box();
};

#endif // COOKIE_GAME_H