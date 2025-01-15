/*
 * COOKIE CLICKER BUILDINGS
 */

#ifndef COOKIE_BUILD_H
#define COOKIE_BUILD_H

/*
 * number of building types
 */
const int NUM_BUILDS = 20;

/*
 * shortened display names for all the buildings
 * name length is limited to 6 characters to save space
 */
const char BUILD_NAMES[NUM_BUILDS][7] = {
    "CURSOR", // cursor
    "GRNDMA", // grandma
    "FARM",   // farm
    "MINE",   // mine
    "FACTRY", // factory
    "BANK",   // bank
    "TEMPLE", // temple
    "WIZARD", // wizard tower
    "SHIPMT", // shipment
    "ALCHMY", // alchemy lab
    "PORTAL", // portal
    "TIMEMC", // time machine
    "ANTMTR", // antimatter condenser
    "PRISM",  // prism
    "CHANCE", // chancemaker
    "FRACTL", // fractal engine
    "JSCONS", // javascript console
    "IDLVRS", // idleverse
    "CORTEX", // cortex baker
    "YOU",    // you
};
/* base cps of each building type */
const double BUILD_CPS[NUM_BUILDS] = {0.1, 1, 8, 47, 260, 1400, 7800, 44000, 260000, 1.6e6, 10e6, 65e6, 430e6, 2.9e9, 21e9, 150e9, 1.1e12, 8.3e12, 64e12, 510e12};
/* price of building the first one of every building */
const double BUILD_PRICES[NUM_BUILDS] = {15, 100, 1100, 12000, 130000, 1.4e6, 20e6, 330e6, 5.1e9, 75e9, 1e12, 14e12, 170e12, 2.1e15, 26e15, 310e15, 71e18, 12e21, 1.9e24, 540e27};

/* upgrade base price */
const double TIER_UPG_PRICE[] = {100, 1000, 11000, 120000, 1.3e6, 14e6, 200e6, 3.3e9, 51e9, 750e9, 10e12, 140e12, 1.7e15, 21e15, 260e15, 3.1e18, 710e18, 120e21, 19e24, 5.4e27};
/* price multiplier of each upgrade tier */
const double TIER_UPG_MULTIPLIER[] = {1, 5, 50, 5000, 500000, 50e6, 50e9, 50e12, 50e15, 50e18, 500e21, 5e27, 50e30, 500e33, 5e39};
/* qty needed to unlock the next upgrade tier for a building */
const double TIER_UPG_REQ[] = {1, 5, 25, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600};

class Building
{
public:
  const char *name;         // building display name, max 6 chars
  double cps, base_cps;     // cookies per second
  double price, base_price; // cost to buy this building
  int qty;                  // amount of this building we own

  Building() : name(nullptr), base_cps(0), base_price(0), qty(0) {}

  Building(const char *name, double cps, double price) : name(name), base_cps(cps), base_price(price), qty(0)
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