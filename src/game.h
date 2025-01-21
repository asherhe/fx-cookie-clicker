/*
 * controller for cookie clicker game
 */

#ifndef COOKIE_GAME_H
#define COOKIE_GAME_H

#include "building.h"
#include "list.h"
#include <gint/keyboard.h>
#include <gint/display.h>

/* duration of each tick (s) */
const double TICK = 0.100;

/* location of savefiles */
const char SAVE_LOC[] = "SAVE.CLK";

/* size of file buffer for reading/writing. usually, file should be less than 256B */
const int DEFAULT_FILE_BUF_SIZE = 256;

/* number of tabs we have */
const int NUM_TABS = 5;

/* all the tabs that the game has */
enum SidebarTab
{
  TAB_BUILDINGS,
  TAB_UPGRADES,
  TAB_ACHIEVEMENTS,
  TAB_OPTIONS,
  TAB_STATS
};

/* window that spans entire sidebar. dimensions: 20x6 chars */
const struct dwindow SIDEBAR_WINDOW = {46, 9, 126, 55};
/* window for buildings specifically */
const struct dwindow BUILDINGS_WINDOW = {46, 19, 124, 55};

const int NUM_DESCS = 92;
/* lines of descriptions that can be put in the message window */
extern char DESCS[NUM_DESCS][25];

/* UPGRADE DATA */

/* number of upgrades available */
const int NUM_UPGS = 195;
/* name of each upgrade */
extern char UPG_NAME[NUM_UPGS][21];
/* cost to purchase each upgrade */
extern double UPG_PRICE[NUM_UPGS];
/* the building this upgrade is associated with */
extern BuildType UPG_BUILDING[NUM_UPGS];
/* the number of the associated building that we need to unlock this upgrade */
extern int16_t UPG_BUILD_REQ[NUM_UPGS];
/* three lines of description for the upgrade's info box, points to DESC */
extern uint16_t UPG_DESC1[NUM_UPGS], UPG_DESC2[NUM_UPGS], UPG_DESC3[NUM_UPGS];

class Game
{
public:
  /* data about all the building types */
  Building buildings[NUM_BUILDS];
  /* how many buildings do we have? */
  int num_buildings;

  /* keeps track of which upgrades have been bought */
  bool upg_bought[NUM_UPGS];
  /* keeps track of which upgrades have been unlocked */
  bool upg_unlocked[NUM_UPGS];
  /* a list, in order of when they were added, of all unlocked upgrades */
  list<int> upg_unlocked_list;

  /*
   * makes a new game
   */
  Game();

  //////////// SAVING AND LOADING ////////////

  /* writes a new savegame at `SAVE_LOC` */
  bool save_game();

  /* tries to load a savegame at `SAVE_LOC`. if file `SAVE_LOC` does not exist, return false */
  bool load_game();

  //////////// GAME FUNCTIONS ////////////

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
   * attempt to buy the upgrade with id `u`. return `false` if insufficient cookies
   */
  bool buy_upgrade(int u);

  //////////// UI CONTROLS ////////////

  /*
   * switch the sidebar tab to the requested one
   */
  void switch_tab(SidebarTab t);

  /*
   * scroll to the i-th item in the sidebar
   */
  void sidebar_scroll(int i);

  /*
   * add the upgrade with id `u` to the list of available upgrades
   */
  void unlock_upgrade(int u);

  //////////// GAME LOOP ////////////

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
  /* number of cookies we currently have */
  double cookies;
  /* cookies baked by buildings per second */
  double cps;

  /* total number of cookies we have ever baked */
  double cookies_all;
  /* number of cookies we get when we click the cookie */
  double click_cookies;
  /* number of times the big cookie has been clicked */
  unsigned cookie_clicks;
  /* cookies we got from clicking */
  double cookies_from_click;

  /* ticks elapsed since game open (used for animations) */
  unsigned ticks;
  /* time (in ticks) in between autosaves */
  unsigned autosave_interval;
  /* time till next autosave */
  unsigned autosave_timer;

  /* ui state */

  SidebarTab sidebar_tab; /* active sidebar tab */
  int tab_scroll;         /* id of the leftmost tab to display */

  int sidebar_sel;               /* currently selected item in sidebar. range is limited to [0, sidebar_sel_n) */
  int sidebar_sel_n;             /* number of selectable options */
  struct dwindow sidebar_window; /* dwindow that covers the rendering region for the sidebar items */
  int sidebar_item_height;       /* vertical displacement between each item in the sidebar */
  int sidebar_item_maxy;         /* maximum y-position the base of each sidebar item can be to remain onscreen */
  int sidebar_dy;                /* visual scroll offset */

  int unlocked_buildings; /* number of buildings visible in the UI */
  bool new_upgs;          /* shows unread badge on upgrade tab if new upgrades are available */

  enum
  {
    MSG_NONE,     /* no message box */
    MSG_UPG_INFO, /* shows information about the currently selected upgrade */
  } message_type; /* the state of the message box display */

  /* recalculate current cps */
  void calc_cps();

  /* set the number of selection options, changing the cursor possition if it goes out of bounds */
  void set_scroll_sel_n(int n);

  /* sidebar rendering */
  void render_tab_buildings();
  void render_tab_upgrades();
  void render_tab_achievements();
  void render_tab_options();
  void render_tab_stats();

  /* message box rendering */
  void render_message_box();
};

#endif // COOKIE_GAME_H
