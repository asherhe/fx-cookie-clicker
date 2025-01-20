/*
 * COOKIE CLICKER BUILDINGS AND UPGRADES
 */

#ifndef COOKIE_BUILD_H
#define COOKIE_BUILD_H

#include <stdint.h>

/*
 * number of building types
 */
const int NUM_BUILDS = 20;

enum BuildType : uint8_t
{
  B_CURSOR,
  B_GRANDMA,
  B_FARM,
  B_MINE,
  B_FACTORY,
  B_BANK,
  B_TEMPLE,
  B_WIZARD_TOWER,
  B_SHIPMENT,
  B_ALCHEMY_LAB,
  B_PORTAL,
  B_TIME_MACHINE,
  B_ANTIMATTER_CONDENSER,
  B_PRISM,
  B_CHANCEMAKER,
  B_FRACTAL_ENGINE,
  B_JAVASCRIPT_CONSOLE,
  B_IDLEVERSE,
  B_CORTEX_BAKER,
  B_YOU,
};

/* shortened display names for all the buildings (max 6 chars) */
extern char BUILD_NAMES[NUM_BUILDS][7];
/* base cps of each building type */
extern double BUILD_CPS[NUM_BUILDS];
/* price of building the first one of every building */
extern double BUILD_PRICES[NUM_BUILDS];

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
/* three lines of description for the upgrade's info box */
extern char UPG_DESC1[NUM_UPGS][25], UPG_DESC2[NUM_UPGS][25], UPG_DESC3[NUM_UPGS][25];

class Game;

struct Building
{
  BuildType type;
  const char *name;                     // building display name, max 6 chars
  double cps, base_cps, cps_multiplier; // cookies per second
  double price, base_price;             // cost to buy this building
  uint16_t qty;                         // amount of this building we own
  double thousand_fingers;              // for B_CURSOR only: bonus cookies from the Thousand Fingers upgrade
  Game *game;                           // game instance this building exists in

  Building() : name(nullptr), base_cps(0.0), cps_multiplier(1.0), base_price(0.0), qty(0) {}

  Building(BuildType type, Game *game)
      : type(type),
        name(BUILD_NAMES[type]),
        base_cps(BUILD_CPS[type]),
        cps_multiplier(1.0),
        base_price(BUILD_PRICES[type]),
        qty(0), game(game)
  {
    calc_cps();
    calc_price();
  }

  /* buy one instance of this building */
  void buy();

  /* calculates the cps of this building */
  void calc_cps();
  /* calculates the price of this building */
  void calc_price();
};

#endif // COOKIE_BUILD_H