/*
 * controller for cookie clicker game
 */

#ifndef COOKIE_GAME_H
#define COOKIE_GAME_H

#include "building.h"

class Game
{
public:
  /* number of cookies we currently have */
  double cookies;
  /* number of cookies we've made since the game has started */
  double cookies_baked;
  /* cookies baked by buildings per second */
  double cps;

  /* data about all the building types */
  Building buildings[NUM_BUILDS];

  /*
   * makes a new game
   */
  Game();

  /*
   * fully renders the game at its current state
   */
  void render();

private:
  /* recalculate current cps */
  void calculate_cps();
};

#endif // COOKIE_GAME_H