#include "building.h"
#include "game.h"
#include "util.h"

void Building::buy()
{
  ++qty;
  calc_price();

  // tiered upgrades for building i starts at id 15i
  for (int i = 15 * type; i < 15 * (type + 1); ++i)
  {
    if (qty >= UPG_BUILD_REQ[i])
      game->unlock_upgrade(i);
  }
}

void Building::calc_cps()
{
  cps_multiplier = 1.0;

  // cps bonus from the thousand fingers upgrade
  double thousand_fingers = 0.0;
  if (type == B_CURSOR)
  {
    for (int i = 0; i < 15; ++i)
      if (game->upg_bought[i])
      {
        if (i < 3)
          cps_multiplier *= 2;
        else if (i == 3)
          thousand_fingers = 0.1;
        else if (i == 4)
          thousand_fingers *= 5;
        else if (i == 5)
          thousand_fingers *= 10;
        else
          thousand_fingers *= 20;
      }
    thousand_fingers *= game->num_buildings - qty;
  }
  else
  {
    for (int i = 15 * type; i < 15 * (type + 1); ++i)
    {
      if (game->upg_bought[i])
        cps_multiplier *= 2;
    }
  }

  cps = base_cps * cps_multiplier + thousand_fingers;
}

void Building::calc_price()
{
  price = base_price * pow(1.15, qty);
}
