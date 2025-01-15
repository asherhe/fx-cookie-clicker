/*
 * general utility functions
 */

#ifndef COOKIE_UTIL_H
#define COOKIE_UTIL_H

#include <string.h>

/*
 * converts a given number into a string, using units of thousands, millions, billions, etc. when applicable.
 *
 * output is written to string `str`, which is returned by the function. for values in the interval [0, 1e15), the
 * resulting string will be at most 4 characters. values 1e15 and up will be at most 5 characters, and negative numbers
 * are one character longer than their positive version (because of the `-` character)
 *
 * LIST OF SUFFIXES
 * - k  - thousand
 * - M  - million
 * - B  - billion
 * - T  - trillion
 * - Qa - quadrillion
 * - Qi - quintillion
 * - Sx - sextillion
 * - Sp - septillion
 * - Oc - octillion
 * - No - nonillion
 * - Dc - decillion
 */
char *num_to_str(double n, char *str);

#endif // COOKIE_UTIL_H