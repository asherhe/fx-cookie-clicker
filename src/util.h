/*
 * general utility functions
 */

#ifndef COOKIE_UTIL_H
#define COOKIE_UTIL_H

#include <string.h>

/*
 * converts a given number into a string, using units of thousands, millions, billions, etc. when applicable.
 *
 * output is written to string `str`, which is returned by the function. expect all outputs to be at most length 7
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

/*
 * converts a given number to a string.
 * - if number is below 1,000, write it with 2 decimal places
 * - if number is below 1 million, write it with a comma separator at the thousands place
 * - otherwise, write the number with 4 sig figs and a suffix.
 */
char *num_to_str_long(double n, char *str);

/*
 * MATH FUNCTIONS
 * math.h is not available in freestanding mode for some reason,
 * so here is the reimplementation of a few functions i need
 */

double floor(double x);
double mod(double a, double b);
double pow(double x, int p);
int max(int a, int b);
int min(int a, int b);

#endif // COOKIE_UTIL_H