#include "building.h"
#include "util.h"

void Building::buy()
{
  ++qty;
  calc_price();
}

void Building::buy(int n)
{
  qty += n;
  calc_price();
}

void Building::calc_cps()
{
  cps = base_cps;
}

void Building::calc_price()
{
  price = base_price * pow(1.15, qty);
}
