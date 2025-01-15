#include "game.h"
#include "util.h"
#include <string.h>
#include <gint/display.h>

Game::Game() : cookies(0), cookies_baked(0), cps(0)
{
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    buildings[i] = Building(BUILD_NAMES[i], BUILD_CPS[i], BUILD_PRICES[i]);
  }
}

void Game::render()
{
  extern bopti_image_t img_ui_base;    // background image that has panel separation and title
  extern bopti_image_t img_info_bar;   // bar that appears at the top of the right panel to display information
  extern bopti_image_t img_tabs;       // spritesheet of the tab buttons at the bottom of the screen
  extern bopti_image_t img_cookie;     // inline cookie
  extern bopti_image_t img_big_cookie; // cool cookie
  extern font_t font_small;            // 3x5 font

  char buf[8];

  /* ui base */
  dimage(0, 0, &img_ui_base);

  /* cookie display */

  // cookie count
  dtext_opt(20, 15, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_CENTER, num_to_str(cookies, buf));

  // cps
  dfont(&font_small);
  dtext_opt(13, 22, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, num_to_str(cps, buf));
  int cps_unit_shift = 2 * strlen(buf);
  dimage(15 + cps_unit_shift, 22, &img_cookie);
  dtext(21 + cps_unit_shift, 22, C_BLACK, "/s");

  // big cookie
  dimage(5, 29, &img_big_cookie);

  /* building list */
  int sel_build = 3;

  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    int draw_y = 21 + i * 8;

    // building name
    dtext(47, draw_y, C_BLACK, buildings[i].name);
    if (i == sel_build)
    {
      dtext(73, draw_y, C_BLACK, "~~~~~~~~~~");
      dtext(113, draw_y, C_BLACK, "BUY");
      // price
      dtext_opt(71, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, num_to_str(buildings[i].price, buf));
      dimage(71 + strlen(buf) * 4, draw_y, &img_cookie);
    }
    else
    {
      // dashed line
      dtext(71, draw_y, C_BLACK, "~~~~~~~~~~~~");
      // count
      if (buildings[i].qty)
        dprint_opt(71, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "*%d", buildings[i].qty);
      // price
      dtext_opt(117, draw_y, C_BLACK, C_WHITE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(buildings[i].price, buf));
      dimage(119, draw_y, &img_cookie);
    }
  }

  // selected building
  drect(46, 20 + sel_build * 8, 124, 26 + sel_build * 8, C_INVERT);

  // selected building info
  dimage(45, 9, &img_info_bar);
  dtext(47, 11, C_BLACK, buildings[sel_build].name);
  dtext_opt(109, 11, C_BLACK, C_NONE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(buildings[sel_build].cps, buf));
  dimage(111, 11, &img_cookie);
  dtext(117, 11, C_BLACK, "/s");

  /* tabs */
  dsubimage(108, 55, &img_tabs, 0, 0, 19, 9, DIMAGE_NOCLIP); // rightmost scroll button
  dsubimage(45, 55, &img_tabs, 21, 9, 21, 9, DIMAGE_NOCLIP); // building list
  dsubimage(65, 55, &img_tabs, 0, 18, 21, 9, DIMAGE_NOCLIP); // available upgrades
  dsubimage(86, 55, &img_tabs, 0, 27, 21, 9, DIMAGE_NOCLIP); // view options

  dupdate();
}
