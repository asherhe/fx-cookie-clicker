#include "util.h"
#include <stdio.h>
#include <string.h>
#include <gint/display.h>

const int NUM_SUFFIXES = 11;
const char SUFFIX_STR[NUM_SUFFIXES][3] = {
    "k",
    "M",
    "B",
    "T",
    "Qa",
    "Qi",
    "Sx",
    "Sp",
    "Oc",
    "No",
    "Dc",
};

char *num_to_str(double n, char *str)
{
  if (n == 0.0)
    return strcpy(str, "0");

  // output progress, points to the next available character in str
  char *out = str;

  // add negative sign for negative numbers
  if (n < 0.0)
  {
    *(out++) = '-';
    n = -n;
  }

  // small numbers don't have a suffix, we can take advantage of that to squeeze
  // out some extra precision from our number (number length capped at 4)
  if (n < 10.0) // less than 10: two decimal places
  {
    sprintf(out, "%.2f", n);
    return str;
  }
  if (n < 100.0) // less than 100: three sig figs
  {
    sprintf(out, "%.3g", n);
    return str;
  }

  // general case: number in front, suffix behind
  // we can afford to add one decimal point to single-digit numbers

  // count the number of thousands (complete groups of 3 digits in n)
  int k = 0;
  for (; n >= 1000.0; n /= 1000.0, ++k)
    ;

  // numbers less than 10 get one decimal digit cuz we have space
  if (n < 10.0)
    out += sprintf(out, "%.2g", n);
  else
    out += sprintf(out, "%.0f", n);

  if (k--)
  {
    if (k < NUM_SUFFIXES)
      strcpy(out, SUFFIX_STR[k]);
    else
      strcpy(out, "?");
  }

  return str;
}
