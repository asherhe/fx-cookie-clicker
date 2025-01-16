/*
 * COOKIE CLICKER BUILDINGS
 */

#ifndef COOKIE_BUILD_H
#define COOKIE_BUILD_H

/*
 * number of building types
 */
const int NUM_BUILDS = 20;

enum BuildType
{
  B_CURSOR = 0,
  B_GRANDMA = 1,
  B_FARM = 2,
  B_MINE = 3,
  B_FACTORY = 4,
  B_BANK = 5,
  B_TEMPLE = 6,
  B_WIZARD_TOWER = 7,
  B_SHIPMENT = 8,
  B_ALCHEMY_LAB = 9,
  B_PORTAL = 10,
  B_TIME_MACHINE = 11,
  B_ANTIMATTER_CONDENSER = 12,
  B_PRISM = 13,
  B_CHANCEMAKER = 14,
  B_FRACTAL_ENGINE = 15,
  B_JAVASCRIPT_CONSOLE = 16,
  B_IDLEVERSE = 17,
  B_CORTEX_BAKER = 18,
  B_YOU = 19
};

/*
 * shortened display names for all the buildings
 * name length is limited to 6 characters to save space
 */
extern char BUILD_NAMES[NUM_BUILDS][7];
/* base cps of each building type */
extern double BUILD_CPS[NUM_BUILDS];
/* price of building the first one of every building */
extern double BUILD_PRICES[NUM_BUILDS];
/* upgrade base price */
extern double TIER_UPG_PRICE[NUM_BUILDS];

/* price multiplier of each upgrade tier */
const double TIER_UPG_MULTIPLIER[] = {1, 5, 50, 5000, 500000, 50e6, 50e9, 50e12, 50e15, 50e18, 500e21, 5e27, 50e30, 500e33, 5e39};
/* qty needed to unlock the next upgrade tier for a building */
const double TIER_UPG_REQ[] = {1, 5, 25, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600};

class Building
{
public:
  BuildType type;
  const char *name;         // building display name, max 6 chars
  double cps, base_cps;     // cookies per second
  double price, base_price; // cost to buy this building
  int qty;                  // amount of this building we own

  Building() : name(nullptr), base_cps(0), base_price(0), qty(0) {}

  Building(BuildType type) : type(type), name(BUILD_NAMES[type]), base_cps(BUILD_CPS[type]), base_price(BUILD_PRICES[type]), qty(0)
  {
    calc_cps();
    calc_price();
  }

  /* buy one instance of this building */
  void buy();
  /* buy several buildings */
  void buy(int n);

private:
  /* calculates the cps of this building */
  void calc_cps();
  /* calculates the price of this building */
  void calc_price();
};

#endif // COOKIE_BUILD_H