/* Copyright (c) 2016-∞, Jean Schurger <jean@schurger.org> */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/> */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define EPOCH_LEN 10  /* 1467235059 == 10 chars */
#define OUT_LEN 19  /* 2016-06-29 12:34:56 == 19 chars */


struct s_found {
  char epoch[EPOCH_LEN + 1];
  int position;
};


void find(char *test_s, struct s_found *found) {
  char *c;
  unsigned digit_count;
  char have_first_digit = 0;
  char wait_next_non_digit = 0;
  unsigned start = 0;
  for(c = test_s; *c != 0; c++) {
    if (*c >= '0' && *c <= '9') {
      if (!wait_next_non_digit) {
        if (!have_first_digit) {
          start = c - test_s;
          bzero(found->epoch, (EPOCH_LEN + 1) * sizeof(char));
          digit_count = 0;
          have_first_digit = 1;
        }
        if (digit_count < EPOCH_LEN) {
          found->epoch[digit_count++] = *c;
        } else {
          have_first_digit = 0;
          wait_next_non_digit = 1;
          digit_count = 0;
        }
      }
    } else {
      wait_next_non_digit = 0;
      if (digit_count == EPOCH_LEN) {
        found->position = start;
        return;
      }
      have_first_digit = 0;
      digit_count = 0;
    }
  }
  found->epoch[0] = 0;
  return;
}


void line(char *test_s) {
  time_t     t;
  struct tm  ts;
  char       buf[20];
  unsigned len;
  struct s_found found;
  char *out = NULL;
  unsigned out_offset = 0;
  char *cp_start = test_s;
  found.position = -1;
  while (found.position) {
    found.epoch[0] = 0;
    found.position = 0;
    find(cp_start, &found);
    if (found.epoch[0]) {
      len = 1 + found.position + (OUT_LEN) + (out ? strlen(out) : 0);
      out = realloc(out, len * sizeof(char));
      strncpy(out + out_offset, cp_start, found.position);
      t = (time_t)atoi(found.epoch);
      ts = *localtime(&t);
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
      strncpy(out + out_offset + found.position, buf, OUT_LEN);
      out_offset += found.position + OUT_LEN;
      cp_start += found.position + EPOCH_LEN;
    }
  }
  if (out) {
    fputs(out, stdout);
    free(out);
    if (strlen(cp_start)) {
      fputs(cp_start, stdout);
    }
  } else {
    fputs(test_s, stdout);
  }
}


int main(void) {
  char buf[4096];
  while (fgets(buf, sizeof(buf), stdin)) {
    line(buf);
  }
  return 0;
}
