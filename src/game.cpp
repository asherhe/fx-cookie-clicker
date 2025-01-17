#include "game.h"
#include "util.h"
#include <string.h>
#include <gint/display.h>
#include <gint/keyboard.h>

Game::Game() : cookies(0), cookies_all(0), cps(0), ticks(0), tab_scroll(0), unlocked_buildings(2), message_type(MSG_NONE)
{
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    buildings[i] = Building((BuildType)i);
  }

  switch_tab(TAB_BUILDINGS);
}

void Game::operator+=(double v)
{
  cookies += v;
  cookies_all += v;

  // unlock the next building if we have enough all-time cookies
  if (cookies_all >= BUILD_PRICES[unlocked_buildings - 2])
  {
    ++unlocked_buildings;
    if (sidebar_tab == TAB_BUILDINGS)
      sidebar_sel_max = unlocked_buildings - 1;
  }
}

void Game::click()
{
  *this += 1;
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
  sidebar_sel = 0;
  sidebar_dy = 0;

  switch (t)
  {
  case TAB_BUILDINGS:
    sidebar_sel_max = unlocked_buildings - 1;
    sidebar_item_height = 8;
    sidebar_window = BUILDINGS_WINDOW;
    break;
  case TAB_UPGRADES:
    sidebar_sel_max = NUM_UPGS - 1;
    sidebar_item_height = 15;
    sidebar_window = SIDEBAR_WINDOW;
    break;
  case TAB_OPTIONS:
    sidebar_sel_max = 0;
    sidebar_item_height = 8;
    sidebar_window = SIDEBAR_WINDOW;
    break;
  case TAB_STATS:
    sidebar_sel_max = 0;
    sidebar_item_height = 8;
    sidebar_window = SIDEBAR_WINDOW;
    break;
  default:
    break;
  }

  sidebar_item_maxy = sidebar_window.bottom - sidebar_window.top - sidebar_item_height;
}

void Game::sidebar_scroll(int i)
{
  sidebar_sel = i;

  // ensure sidebar_sel is within bounds
  if (sidebar_sel < 0)
    sidebar_sel = 0;
  if (sidebar_sel > sidebar_sel_max)
    sidebar_sel = sidebar_sel_max;

  // scroll the rendering of the sidebar to fit the selected item
  int sel_draw_y = sidebar_sel * sidebar_item_height + sidebar_dy;
  if (sel_draw_y < 0)
    sidebar_dy -= sel_draw_y;
  else if (sel_draw_y > sidebar_item_maxy)
    sidebar_dy -= sel_draw_y - sidebar_item_maxy;
}

void Game::key_pressed(key_event_t k)
{
  if (message_type == MSG_NONE)
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
      sidebar_scroll(sidebar_sel - 1);
      break;
    case KEY_DOWN:
      sidebar_scroll(sidebar_sel + 1);
      break;
    case KEY_EXE:
      switch (sidebar_tab)
      {
      case TAB_BUILDINGS:
        buy_building((BuildType)sidebar_sel);
        break;
        // TODO: buy upgrades
      default:
        break;
      }
      break;
    case KEY_1:
      if (sidebar_tab == TAB_UPGRADES)
        message_type = MSG_UPG_INFO;
      break;

    default:
      break;
    }
  }
  else
  {
    if (k.key == KEY_EXIT)
      message_type = MSG_NONE;
  }
}

void Game::tick()
{
  ++ticks;

  *this += cps * TICK;
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

  // selected item
  drect(sidebar_window.left, sidebar_window.top + 1 + sidebar_dy + sidebar_sel * sidebar_item_height,
        sidebar_window.right, sidebar_window.top + sidebar_item_height - 1 + sidebar_dy + sidebar_sel * sidebar_item_height, C_INVERT);

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

  // draw message box
  if (message_type != MSG_NONE)
    render_message_box();

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

void Game::render_tab_buildings()
{
  char buf[8];

  extern bopti_image_t img_info_bar; // bar that appears at the top of the right panel to display information
  extern bopti_image_t img_cookie;   // inline cookie

  struct dwindow old_window = dwindow_set(sidebar_window);

  int draw_y = sidebar_window.top + 2 + sidebar_dy;
  for (int i = 0; i <= sidebar_sel_max; ++i, draw_y += sidebar_item_height)
  {
    // building name
    dtext(47, draw_y, C_BLACK, buildings[i].name);
    // dashed line
    dtext(71, draw_y, C_BLACK, "~~~~~~~~~~~~");
    if (i == sidebar_sel && cookies >= buildings[i].price && ticks & 0b1000)
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

  dwindow_set(old_window);

  // selected building info
  dimage(45, 9, &img_info_bar);
  dtext(47, 11, C_BLACK, buildings[sidebar_sel].name);
  dprint(71, 11, C_BLACK, "*%d", buildings[sidebar_sel].qty);
  dtext_opt(109, 11, C_BLACK, C_NONE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(buildings[sidebar_sel].cps, buf));
  dimage(111, 11, &img_cookie);
  dtext(117, 11, C_BLACK, "/s");
}

void Game::render_tab_upgrades()
{
  char buf[8];

  extern bopti_image_t img_cookie; // inline cookie

  struct dwindow old_window = dwindow_set(sidebar_window);

  int draw_y = sidebar_window.top + 2 + sidebar_dy;
  for (int i = 0; i <= sidebar_sel_max; ++i, draw_y += sidebar_item_height)
  {
    // upgrade name
    dtext(47, draw_y, C_BLACK, UPG_NAME[i]);

    // buy/info button, swap animation
    if (i == sidebar_sel)
      dtext_opt(47, draw_y + 6, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, (ticks & 0b10000) ? "[EXE]BUY" : "[1]INFO");

    // price
    dtext_opt(119, draw_y + 6, C_BLACK, C_WHITE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(UPG_PRICE[i], buf));
    dimage(121, draw_y + 6, &img_cookie);
  }

  dwindow_set(old_window);
}

void Game::render_tab_options()
{
  dtext(47, 11, C_BLACK, "options");
}

void Game::render_tab_stats()
{
  char buf[21];
  int base_y = sidebar_window.top + 2 + sidebar_dy;

  // number of cookies
  dprint(47, base_y, C_BLACK, "Cookies:%s", num_to_str_long(cookies, buf));
}

void Game::render_message_box()
{
  // message box base
  drect(15, 59, 117, 11, C_BLACK);                   // shadow
  drect_border(12, 6, 115, 57, C_WHITE, 1, C_BLACK); // outer border
  drect_border(14, 8, 113, 55, C_WHITE, 1, C_BLACK); // inner border

  switch (message_type)
  {
  case MSG_UPG_INFO:
    // window title
    dtext(40, 10, C_BLACK, "==UPG INFO==");
    dline(15, 16, 112, 16, C_BLACK);

    dtext(16, 18, C_BLACK, UPG_NAME[sidebar_sel]);  // upgrade name
    dtext(16, 27, C_BLACK, UPG_DESC1[sidebar_sel]); // description line 1
    dtext(16, 33, C_BLACK, UPG_DESC2[sidebar_sel]); // description line 2
    dtext(16, 39, C_BLACK, UPG_DESC3[sidebar_sel]); // description line 3
    break;

  default:
    break;
  }

  // exit button
  dtext(89, 47, C_BLACK, "[EXIT]");
}
