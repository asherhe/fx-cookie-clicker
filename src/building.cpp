#include "building.h"
// #include <math.h>

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
  // price = base_price * pow(1.15, qty);

  // math.h straight up just doesn't work so we find the power manually
  price = base_price;
  for (int i = 0; i < qty; ++i)
    price *= 1.15;
}
