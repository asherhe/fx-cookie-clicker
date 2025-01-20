#include "game.h"
#include "util.h"
#include "list.h"
#include "files.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <gint/display.h>
#include <gint/keyboard.h>

Game::Game() : upg_unlocked_list(), autosave_interval(600), click_cookies(1), unlocked_buildings(2)
{
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    buildings[i] = Building((BuildType)i, this);
  }

  load_game();

  autosave_timer = autosave_interval;

  switch_tab(TAB_BUILDINGS);
}

bool Game::save_game()
{
  char buf[FILE_BUF_SIZE];
  char *cur = buf; // output progress

  // file format: https://docs.google.com/spreadsheets/d/1XEMjNKJkmM5mCc2lz7IlXEv5vYw1xzqRuGZv8csOpEI/edit?usp=sharing
  // note: we're using big-endian

  /* write signature */
  cur += sprintf(cur, "COOKIEv%s", COOKIE_VERSION);
  for (; cur - buf < 16; ++cur)
    *cur = '\0';

  /* game data */
  // leave 2 bytes to write size later
  char *gdata_start = cur;
  cur += 2;
  cur += write_chunk_var("nCKS", cookies, cur);            // current number of cookies
  cur += write_chunk_var("CKat", cookies_all, cur);        // all-time num. cookies made
  cur += write_chunk_var("TICK", ticks, cur);              // ticks since game start
  cur += write_chunk_var("CLKS", cookie_clicks, cur);      // number of times cookie has been clicked
  cur += write_chunk_var("cCLK", cookies_from_click, cur); // cookies we made by clicking
  cur += write_chunk_var("nwUP", new_upgs, cur);           // whether there are new upgrades available

  // write game data block size
  uint16_t gdata_size = cur - gdata_start;
  cpy_bytes_from_var(gdata_size, gdata_start);

  /* options */
  // TODO: ignore this for now
  memcpy(cur, "\x00\x02", 2); // options block: 2 bytes (refers to itself)
  cur += 2;

  /* building data */
  size_t bld_qty_size = sizeof(buildings->qty); // should be 2
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    cpy_bytes((char *)&buildings[i].qty, bld_qty_size, cur);
    cur += bld_qty_size;
  }

  /* upgrade data */
  // bought updates bitset
  char *upg_bought_start = cur++; // leave one byte for bitset size
  *upg_bought_start = 0;
  for (int i = 0; i < NUM_UPGS; i += 8)
  {
    ++*upg_bought_start;
    char upgs8 = 0; // group of 8 upgrades
    for (int j = 0; j < 8; ++j)
      if (i + j < NUM_UPGS)
        upgs8 |= (upg_bought[i + j] != 0) << j;
    *(cur++) = upgs8;
  }
  // unlocked upgrades list
  uint16_t upg_unlocked_n = upg_unlocked_list.size();
  cur += cpy_bytes_from_var(upg_unlocked_n, cur);
  for (auto it = upg_unlocked_list.begin(); it != upg_unlocked_list.end(); ++it)
  {
    uint16_t upg_id = *it;
    cur += cpy_bytes_from_var(upg_id, cur);
  }

  // write to file
  FILE *fp = fopen(SAVE_LOC, "wb");
  if (!fp)
    return false;
  fwrite(buf, sizeof(*buf), cur - buf, fp);
  fclose(fp);
  return true;
}

bool Game::load_game()
{
  char buf[FILE_BUF_SIZE];

  FILE *fp = fopen(SAVE_LOC, "rb");

  // do nothing if file doesn't exist
  if (!fp)
    return false;

  size_t size = fread(buf, sizeof(*buf), FILE_BUF_SIZE, fp);

  if (size == FILE_BUF_SIZE) // oh shit, our buffer is too small
    return false;

  fclose(fp);

  char *cur = buf + 16; // skip header

  /* game data */
  // reset game data
  cookies = 0.0f;
  cookies_all = 0.0f;
  ticks = 0;
  cookie_clicks = 0;
  new_upgs = 0;
  // get gdata block size
  char *gdata_start = cur;
  uint16_t gdata_bytes;
  cur += cpy_bytes_to_var(cur, gdata_bytes);
  // read game data chunks
  while (cur - gdata_start < gdata_bytes)
  {
    if (!memcmp(cur, "nCKS", 4)) // number of cookies
      cpy_bytes_to_var(cur + 5, cookies);
    else if (!memcmp(cur, "CKat", 4)) // number of cookies (all time)
      cpy_bytes_to_var(cur + 5, cookies_all);
    else if (!memcmp(cur, "TICK", 4)) // ticks elapsed
      cpy_bytes_to_var(cur + 5, ticks);
    else if (!memcmp(cur, "CLKS", 4)) // number of times cookie has been clicked
      cpy_bytes_to_var(cur + 5, cookie_clicks);
    else if (!memcmp(cur, "cCLK", 4)) // cookies made from clicking
      cpy_bytes_to_var(cur + 5, cookies_from_click);
    else if (!memcmp(cur, "nwUP", 4)) // whether there are new upgrades available
      cpy_bytes_to_var(cur + 5, new_upgs);

    // advance to next chunk
    uint8_t data_size;
    cpy_bytes_to_var(cur + 4, data_size);
    cur += 5 + data_size;
  }

  /* options */
  char *optn_start = cur;
  uint16_t optn_bytes;
  cur += cpy_bytes_to_var(cur, optn_bytes);
  // TODO: do something with this

  /* buildings */
  for (int i = 0; i < NUM_BUILDS; ++i)
    cur += cpy_bytes_to_var(cur, buildings[i].qty);

  /* upgrades */
  // clear previous upgrade info
  memset(upg_bought, 0, sizeof(upg_bought));
  memset(upg_unlocked, 0, sizeof(upg_unlocked));
  upg_unlocked_list.clear();
  // bought upgrades bitset
  uint8_t upg_bought_size;
  cur += cpy_bytes_to_var(cur, upg_bought_size);
  for (int i = 0; i < 8 * upg_bought_size && i < NUM_UPGS; i += 8)
  {
    // group of 8 upgrades
    char upgs8;
    cur += cpy_bytes_to_var(cur, upgs8);
    for (int j = 0; j < 8; ++j)
    {
      if (i + j < NUM_UPGS)
        upg_bought[i + j] = upgs8 & (1 << j);
    }
  }
  // unlocked upgrades list
  uint16_t upg_unlocked_n;
  cur += cpy_bytes_to_var(cur, upg_unlocked_n);
  for (int i = 0; i < upg_unlocked_n; ++i)
  {
    uint16_t upg_id;
    cur += cpy_bytes_to_var(cur, upg_id);
    upg_unlocked[upg_id] = 1;
    upg_unlocked_list.push_back(upg_id);
  }

  /* reinitialize fields from read data */

  num_buildings = 0;
  unlocked_buildings = 2;
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    buildings[i].calc_price();
    num_buildings += buildings[i].qty;
    if (cookies_all > BUILD_PRICES[i])
      ++unlocked_buildings;
  }

  calc_cps();

  return true;
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
      set_scroll_sel_n(min(unlocked_buildings, NUM_BUILDS));
  }
}

void Game::click()
{
  *this += click_cookies;
  cookies_from_click += click_cookies;
  ++cookie_clicks;
}

bool Game::buy_building(BuildType b)
{
  if (buildings[b].price > cookies)
    return false;
  cookies -= buildings[b].price;
  buildings[b].buy();
  ++num_buildings;
  calc_cps();
  return true;
}

bool Game::buy_upgrade(int u)
{
  if (upg_bought[u] || cookies < UPG_PRICE[u])
    return false;

  cookies -= UPG_PRICE[u];

  // remove u from unlocked list
  if (upg_unlocked[u])
  {
    upg_unlocked[u] = 0;
    for (auto it = upg_unlocked_list.begin(); it != upg_unlocked_list.end(); ++it)
      if (*it == u)
      {
        upg_unlocked_list.erase(it);
        if (sidebar_tab == TAB_UPGRADES)
          set_scroll_sel_n(upg_unlocked_list.size());
        break;
      }
  }

  upg_bought[u] = 1;

  calc_cps();

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
    set_scroll_sel_n(min(unlocked_buildings, NUM_BUILDS));
    sidebar_item_height = 8;
    sidebar_window = BUILDINGS_WINDOW;
    break;
  case TAB_UPGRADES:
    set_scroll_sel_n(upg_unlocked_list.size());
    sidebar_item_height = 15;
    sidebar_window = SIDEBAR_WINDOW;
    new_upgs = false;
    break;
  case TAB_ACHIEVEMENTS:
    set_scroll_sel_n(0);
    sidebar_item_height = 8;
    sidebar_window = SIDEBAR_WINDOW;
    break;
  case TAB_OPTIONS:
    set_scroll_sel_n(3);
    sidebar_item_height = 8;
    sidebar_window = SIDEBAR_WINDOW;
    break;
  case TAB_STATS:
    set_scroll_sel_n(11 + NUM_BUILDS);
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
  if (sidebar_sel_n > 0)
  { // we can't just directly use modulo when i<0 because that would give us a negative number
    if (i < 0)
      sidebar_sel = i + sidebar_sel_n * (1 - i / sidebar_sel_n);
    else
      sidebar_sel = i;
    sidebar_sel %= sidebar_sel_n;

    // scroll the rendering of the sidebar to fit the selected item
    int sel_draw_y = sidebar_sel * sidebar_item_height + sidebar_dy;
    if (sel_draw_y < 0)
      sidebar_dy -= sel_draw_y;
    else if (sel_draw_y > sidebar_item_maxy)
      sidebar_dy -= sel_draw_y - sidebar_item_maxy;
  }
  else
  {
    sidebar_sel = 0;
    sidebar_dy = 0;
  }
}

void Game::unlock_upgrade(int u)
{
  if (upg_bought[u] || upg_unlocked[u])
    return;
  upg_unlocked[u] = 1;
  upg_unlocked_list.push_back(u);

  if (sidebar_tab == TAB_UPGRADES)
    set_scroll_sel_n(upg_unlocked_list.size());
  else
    new_upgs = 1;
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
      if (tab_scroll + 3 == NUM_TABS)
        tab_scroll = 0;
      else
        tab_scroll = min(tab_scroll + 2, NUM_TABS - 3);
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
      case TAB_UPGRADES:
        buy_upgrade(upg_unlocked_list.at(sidebar_sel));
        break;
      case TAB_OPTIONS:
        if (sidebar_sel == 2)
          save_game();
        break;
      default:
        break;
      }
      break;
    case KEY_MINUS:
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

  if (--autosave_timer <= 0)
  {
    autosave_timer = autosave_interval;
    save_game();
  }
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
  case TAB_ACHIEVEMENTS:
    render_tab_achievements();
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
    SidebarTab tab = (SidebarTab)(tab_scroll + i);
    dsubimage(46 + i * 21, 56,
              &img_tabs,
              (tab == sidebar_tab) ? 20 : 0,
              8 * (tab + 1),
              20, 8, DIMAGE_NOCLIP);
    if (tab == TAB_UPGRADES && new_upgs && ticks & 0b1000)
      drect(64 + i * 21, 55, 66 + i * 21, 57, C_BLACK); // unread badge
  }
  dsubimage(109, 56, &img_tabs, 0, 0, 18, 8, DIMAGE_NOCLIP); // tab scroll button

  // draw message box
  if (message_type != MSG_NONE)
    render_message_box();

  dupdate();
}

void Game::calc_cps()
{
  cps = 0;
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    buildings[i].calc_cps();
    cps += buildings[i].qty * buildings[i].cps;
  }

  // clicking the cookie has the same upgrades as the cursor building
  click_cookies = buildings[B_CURSOR].cps_multiplier + buildings[B_CURSOR].thousand_fingers;
}

void Game::set_scroll_sel_n(int n)
{
  sidebar_sel_n = n;
  sidebar_sel = max(0, min(sidebar_sel, n - 1));
}

void Game::render_tab_buildings()
{
  char buf[8];

  extern bopti_image_t img_info_bar; // bar that appears at the top of the right panel to display information
  extern bopti_image_t img_cookie;   // inline cookie

  struct dwindow old_window = dwindow_set(sidebar_window);

  int draw_y = sidebar_window.top + 2 + sidebar_dy;
  for (int i = 0; i < sidebar_sel_n; ++i, draw_y += sidebar_item_height)
  {
    // dashed line
    dtext(47, draw_y, C_BLACK, "~~~~~~~~~~~~~~~~~~");
    // building name
    dtext_opt(47, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, (i < unlocked_buildings - 2) ? buildings[i].name : "???");
    if (i == sidebar_sel && cookies >= buildings[i].price)
    {
      // buy button
      if (ticks & 0b1000)
        dtext_opt(71, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "[EXE]");
      else
        dtext_opt(75, draw_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "BUY");
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
  dtext(47, 11, C_BLACK, (sidebar_sel < unlocked_buildings - 2) ? buildings[sidebar_sel].name : "???");
  dprint(71, 11, C_BLACK, "*%d", buildings[sidebar_sel].qty);
  dtext_opt(109, 11, C_BLACK, C_NONE, DTEXT_RIGHT, DTEXT_TOP,
            (sidebar_sel < unlocked_buildings - 2)
                ? num_to_str(buildings[sidebar_sel].cps, buf)
                : "???");
  dimage(111, 11, &img_cookie);
  dtext(117, 11, C_BLACK, "/s");
}

void Game::render_tab_upgrades()
{
  char buf[8];

  extern bopti_image_t img_cookie; // inline cookie

  struct dwindow old_window = dwindow_set(sidebar_window);

  int draw_y = sidebar_window.top + 2 + sidebar_dy;

  if (upg_unlocked_list.empty())
  {
    dtext(47, draw_y, C_BLACK, "NO UPGRADES");
    dtext(47, draw_y + 6, C_BLACK, "AVAILABLE");
  }
  else
  {
    int i = 0;
    for (auto it = upg_unlocked_list.begin(); it != upg_unlocked_list.end(); ++it, ++i, draw_y += sidebar_item_height)
    {
      // upgrade name
      dtext(47, draw_y, C_BLACK, UPG_NAME[*it]);

      // buy/info button, swap animation
      if (i == sidebar_sel)
      {
        if (ticks & 0b10000)
        {
          if (cookies >= UPG_PRICE[*it])
            dtext_opt(47, draw_y + 6, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "[EXE]BUY");
        }
        else
        {
          dtext_opt(47, draw_y + 6, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "[-]INFO");
        }
      }

      // price
      dtext_opt(119, draw_y + 6, C_BLACK, C_WHITE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(UPG_PRICE[*it], buf));
      dimage(121, draw_y + 6, &img_cookie);
    }
  }

  dwindow_set(old_window);
}

void Game::render_tab_achievements()
{
  dtext(47, 11, C_BLACK, "achievements");
}

void Game::render_tab_options()
{
  struct dwindow old_window = dwindow_set(sidebar_window);

  int print_y = sidebar_window.top + 2 + sidebar_dy;

  dtext(47, print_y, C_BLACK, "COOKIE CLICKER");
  dprint(47, (print_y += sidebar_item_height), C_BLACK, "version v%s", COOKIE_VERSION);

  dtext(47, (print_y += sidebar_item_height), C_BLACK, "[EXE] SAVE GAME"); // save button

  dwindow_set(old_window);
}

void Game::render_tab_stats()
{
  extern bopti_image_t img_cookie; // inline cookie

  char buf[21];

  struct dwindow old_window = dwindow_set(sidebar_window);

  int print_y = sidebar_window.top + 2 + sidebar_dy;
  dprint(47, print_y, C_BLACK, "Cookies:%s", num_to_str_long(cookies, buf));                               // number of cookies
  dprint(51, (print_y += sidebar_item_height), C_BLACK, "All time:%s", num_to_str_long(cookies_all, buf)); // number of cookies (all time)
  dprint(47, (print_y += sidebar_item_height), C_BLACK, "CpS:%s", num_to_str_long(cps, buf));              // cps

  int time_played = (int)(ticks * TICK), h, m, s;
  s = time_played % 60;
  m = (time_played /= 60) % 60;
  h = time_played / 60;
  dprint(47, (print_y += sidebar_item_height), C_BLACK, "Time played:%d:%02d:%02d", h, m, s);
  dprint(47, (print_y += sidebar_item_height), C_BLACK, "Next autosave:%ds", (int)(autosave_timer * TICK));

  dimage(47, (print_y += sidebar_item_height), &img_cookie);
  dprint(53, print_y, C_BLACK, "/click:%s", num_to_str(click_cookies, buf));                // how many cookies we get each time we click the big cookie
  dprint(47, (print_y += sidebar_item_height), C_BLACK, "Cookie clicks:%d", cookie_clicks); // times we've clicked the big cookie
  dimage(51, (print_y += sidebar_item_height), &img_cookie);
  dprint(61, print_y, C_BLACK, "made:%s", num_to_str(cookies_from_click, buf)); // cookies we've made from clicking

  dprint(47, (print_y += sidebar_item_height), C_BLACK, "Buildings owned:%d", num_buildings); // number of buildings we own
  dtext(47, (print_y += sidebar_item_height), C_BLACK, "==Income Breakdown==");
  for (int i = 0; i < NUM_BUILDS; ++i)
  {
    dtext(51, (print_y += sidebar_item_height), C_BLACK, "~~~~~~~~~~~~~~~~~~~");
    dtext_opt(51, print_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, buildings[i].name);
    dprint_opt(75, print_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "*%d", buildings[i].qty);
    dtext_opt(125, print_y, C_BLACK, C_WHITE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(buildings[i].qty * buildings[i].cps, buf));
  }
  dtext(51, (print_y += sidebar_item_height), C_BLACK, "~~~~~~~~~~~~~~~~~~~");
  dtext_opt(51, print_y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, "TOTAL");
  dtext_opt(125, print_y, C_BLACK, C_WHITE, DTEXT_RIGHT, DTEXT_TOP, num_to_str(cps, buf));

  dwindow_set(old_window);
}

void Game::render_message_box()
{
  // message box base
  drect(15, 59, 117, 11, C_BLACK);                   // shadow
  drect_border(12, 6, 115, 57, C_WHITE, 1, C_BLACK); // outer border
  drect_border(14, 8, 113, 55, C_WHITE, 1, C_BLACK); // inner border

  int sel_id;
  switch (message_type)
  {
  case MSG_UPG_INFO:
    sel_id = upg_unlocked_list.at(sidebar_sel);

    // window title
    dtext(40, 10, C_BLACK, "==UPG INFO==");
    dline(15, 16, 112, 16, C_BLACK);

    dtext(16, 18, C_BLACK, UPG_NAME[sel_id]);  // upgrade name
    dtext(16, 27, C_BLACK, UPG_DESC1[sel_id]); // description line 1
    dtext(16, 33, C_BLACK, UPG_DESC2[sel_id]); // description line 2
    dtext(16, 39, C_BLACK, UPG_DESC3[sel_id]); // description line 3
    break;

  default:
    break;
  }

  // exit button
  dtext(89, 47, C_BLACK, "[EXIT]");
}
