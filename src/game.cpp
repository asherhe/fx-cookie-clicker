#include "game.h"
#include "util.h"
#include <string.h>
#include <gint/display.h>
#include <gint/keyboard.h>

Game::Game() : cookies(0), cps(0), ticks(0), tab_scroll(0)
{
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    buildings[i] = Building((BuildType)i);
  }

  switch_tab(TAB_BUILDINGS);
}

void Game::click()
{
  ++cookies;
}

bool Game::buy_building(BuildType b)
{
  if (buildings[b].price > cookies)
    return false;
  cookies -= buildings[b].price;
  buildings[b].buy();

  calculate_cps();

  return true;
}

void Game::switch_tab(SidebarTab t)
{
  sidebar_tab = t;

  // reset scroll
  sidebar_scroll = 0;
  sidebar_dy = 0;

  switch (t)
  {
  case TAB_BUILDINGS:
    sidebar_scroll_max = NUM_BUILDS - 1;
    break;
  default:
    break;
  }
}

void Game::key_pressed(key_event_t k)
{
  switch (k.key)
  {
  case KEY_0:
    click();
    break;

  /* tab scrolling */
  case KEY_F3:
    switch_tab((SidebarTab)tab_scroll);
    break;
  case KEY_F4:
    switch_tab((SidebarTab)(tab_scroll + 1));
    break;
  case KEY_F5:
    switch_tab((SidebarTab)(tab_scroll + 2));
    break;
  case KEY_F6:
    if (tab_scroll++ + 3 == NUM_TABS)
      tab_scroll = 0;
    break;

  /* sidebbar menu controls */
  case KEY_UP:
    if (--sidebar_scroll < 0)
      sidebar_scroll = 0;
    break;
  case KEY_DOWN:
    if (++sidebar_scroll > sidebar_scroll_max)
      sidebar_scroll = sidebar_scroll_max;
    break;
  case KEY_EXE:
    switch (sidebar_tab)
    {
    case TAB_BUILDINGS:
      buy_building((BuildType)sidebar_scroll);
      break;
    default:
      break;
    }

  default:
    break;
  }
}

void Game::tick()
{
  ++ticks;

  cookies += cps * TICK;
}

// hovering animation, oscillates back and forth
const int hover[] = {0, 1, 1, 1, 0, -1, -1, -1};

void Game::render()
{
  extern bopti_image_t img_ui_base;    // background image that has panel separation
  extern bopti_image_t img_title;      // stylized "cookie clicker" title
  extern bopti_image_t img_tabs;       // spritesheet of the tab buttons at the bottom of the screen
  extern bopti_image_t img_cookie;     // inline cookie
  extern bopti_image_t img_big_cookie; // cool cookie
  extern font_t font_small;            // 3x5 font

  char buf[8];

  /* ui base */
  dimage(0, 0, &img_ui_base);

  // title (add wave effect)
  int wave_progress = ticks % (img_title.width / 4 + 10) * 4;
  dsubimage(10, 0, &img_title, 0, 0, wave_progress, img_title.height, DIMAGE_NONE);
  dsubimage(10 + wave_progress, -1, &img_title, wave_progress, 0, 8, img_title.height, DIMAGE_NONE);
  dsubimage(18 + wave_progress, 0, &img_title, wave_progress + 8, 0, img_title.width, img_title.height, DIMAGE_NONE);

  /* cookie display */

  // cookie count
  dfont(dfont_default());
  dtext_opt(20, 15, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_CENTER, num_to_str(cookies, buf));

  // cps
  dfont(&font_small);
  dtext_opt(13, 22, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, num_to_str(cps, buf));
  int cps_unit_shift = 2 * strlen(buf);
  dimage(15 + cps_unit_shift, 22, &img_cookie);
  dtext(21 + cps_unit_shift, 22, C_BLACK, "/s");

  // big cookie
  dimage(5, 29 + hover[ticks / 2 % 8], &img_big_cookie);

  /* sidebar */
  switch (sidebar_tab)
  {
  case TAB_BUILDINGS:
    render_tab_buildings();
    break;
  case TAB_UPGRADES:
    render_tab_upgrades();
    break;
  case TAB_OPTIONS:
    render_tab_options();
    break;
  case TAB_STATS:
    render_tab_stats();
    break;
  default:
    break;
  }

  /* tabs */
  for (int i = 0; i < 3; ++i)
  {
    dsubimage(45 + i * 21, 55,
              &img_tabs,
              (tab_scroll + i == sidebar_tab) ? 21 : 0,
              9 * (tab_scroll + i + 1),
              21, 9, DIMAGE_NOCLIP);
  }
  dsubimage(108, 55, &img_tabs, 0, 0, 19, 9, DIMAGE_NOCLIP); // tab scroll button

  dupdate();
}

void Game::calculate_cps()
{
  cps = 0;
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    cps += buildings[i].qty * buildings[i].cps;
  }
}

/* tab-specific sidebar rendering */

// sidebar window range
const struct dwindow sidebar_window = {45, 9, 128, 64};

void Game::render_tab_buildings()
{
  char buf[8];

  extern bopti_image_t img_info_bar; // bar that appears at the top of the right panel to display information
  extern bopti_image_t img_cookie;   // inline cookie

  struct dwindow old_window = dwindow_set(sidebar_window);

  int sel_draw_y = sidebar_scroll * 8 - sidebar_dy;
  if (sel_draw_y < 0)
    sidebar_dy -= -sel_draw_y;
  else if (sel_draw_y > 28)
    sidebar_dy += sel_draw_y - 28;

  int draw_y = 21 - sidebar_dy;
  for (int i = 0; i < NUM_BUILDS; ++i, draw_y += 8)
  {
    // building name
    dtext(47, draw_y, C_BLACK, buildings[i].name);
    // dashed line
    dtext(71, draw_y, C_BLACK, "~~~~~~~~~~~~");
    if (i == sidebar_scroll && cookies > buildings[i].price && ticks & 0b1000)
    {
      // buy button
      dtext_opt(71, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "[BUY]");
    }
    else
    {
      // building qty
      if (buildings[i].qty)
        dprint_opt(71, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "*%d", buildings[i].qty);
    }
    // price
    dtext_opt(117, draw_y, C_BLACK, C_WHITE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(buildings[i].price, buf));
    dimage(119, draw_y, &img_cookie);
  }

  // selected building
  drect(46, 20 - sidebar_dy + sidebar_scroll * 8, 124, 26 - sidebar_dy + sidebar_scroll * 8, C_INVERT);

  // selected building info
  dimage(45, 9, &img_info_bar);
  dtext(47, 11, C_BLACK, buildings[sidebar_scroll].name);
  dprint(71, 11, C_BLACK, "*%d", buildings[sidebar_scroll].qty);
  dtext_opt(109, 11, C_BLACK, C_NONE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(buildings[sidebar_scroll].cps, buf));
  dimage(111, 11, &img_cookie);
  dtext(117, 11, C_BLACK, "/s");

  dwindow_set(old_window);
}

void Game::render_tab_upgrades()
{
  dtext(47, 11, C_BLACK, "upgrades");
}

void Game::render_tab_options()
{
  dtext(47, 11, C_BLACK, "options");
}

void Game::render_tab_stats()
{
  dtext(47, 11, C_BLACK, "statistics");
}
