#include "util.h"
#include <stdio.h>
#include <string.h>
#include <gint/display.h>

const int NUM_SUFFIXES = 15;
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
    "Ud",
    "Dd",
    "Td",
    "Qd",
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

  // count the number of thousands (complete groups of 3 digits in n)
  int k = 0;
  for (; n >= 1000.0; n /= 1000.0, ++k)
    ;

  // max 4 chars for number portion
  if (n < 10.0)
    out += sprintf(out, "%.2f", n);
  else if (n < 100.0)
    out += sprintf(out, "%.1f", n);
  else
    out += sprintf(out, "%.0f", n);

  if (k > 1)
  {
    if (k < NUM_SUFFIXES)
      strcpy(out, SUFFIX_STR[k - 1]);
    else if (k <= 33)
    {
      // write exponent in superscript
      char *sup_start = out;
      sprintf(out, "%d", 3 * k);
      *(sup_start++) -= 0x20;
      *sup_start -= 0x20;
    }
    else
      strcpy(out, "??");
  }

  return str;
}

char *num_to_str_long(double n, char *str)
{
  if (n == 0)
    strcpy(str, "0");

  // output progress, points to the next available character in str
  char *out = str;

  // add negative sign for negative numbers
  if (n < 0.0)
  {
    *(out++) = '-';
    n = -n;
  }

  // less than 1,000: write the string with 2 decimal places
  if (n < 1e3)
  {
    sprintf(out, "%.2f", n);
    return str;
  }

  // less than 1 million: we want a comma separating the digits
  if (n < 1e6)
  {
    int before = (int)(n * 0.001);
    double after = n - before * 1000;
    sprintf(out, "%d,%.0f", before, after);
    return str;
  }

  // count the number of thousands (complete groups of 3 digits in n)
  int k = 0;
  for (; n >= 1000.0; n /= 1000.0, ++k)
    ;

  sprintf(out, "%.4g", n);

  if (k < NUM_SUFFIXES)
    strcpy(out, SUFFIX_STR[k - 1]);
  else if (k <= 33)
  {
    // write exponent in superscript
    char *sup_start = out;
    sprintf(out, "%d", 3 * k);
    *(sup_start++) -= 0x20;
    *sup_start -= 0x20;
  }
  else
    strcpy(out, "??");

  return str;
}

double floor(double x) { return (double)((int)x); }

double mod(double a, double b) { return a - (int)(a / b) * b; }

double pow(double x, int p)
{
  if (p == 0)
    return 1.0;
  if (p == 1)
    return x;

  double half = pow(x, p / 2);
  if (p % 2)
    return x * half * half;
  else
    return half * half;
}
