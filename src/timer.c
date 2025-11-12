// SPDX-License-Identifier: MIT
/**
 * @file timer.c
 * @brief Implementation of functions related to a single timer.
 */
#include "timer.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logio.h"

/**
 * check if a year is a leap year
 *
 * @param year  the year to check
 *
 * @return true if it's a leap year, false if not
 */
static bool crinitIsLeapYear(uint16_t year);
/**
 * get a static srting with the short month name
 *
 * @param m  the month starting at 1 for jan
 *
 * @return "Jan" for 1, "Feb" for 2, ...
 */
static const char *crinitMonthToStr(uint8_t m);

void crinitTimerSetDefault(crinitTimerDef_t *td) {
    td->wDay = 0x7f;
    td->years[0] = 0;
    td->years[1] = 0xffff;
    td->month[0] = 1;
    td->month[1] = 12;
    td->days[0] = 1;
    td->days[1] = 31;
    td->hours[0] = 0;
    td->hours[1] = 0;
    td->minutes[0] = 0;
    td->minutes[1] = 0;
    td->seconds[0] = 0;
    td->seconds[1] = 0;
    td->timezone[0] = 0;
    td->timezone[1] = 0;
}

static bool crinitIsLeapYear(uint16_t year) {
    return (year % 4 == 0) && (!(year % 100 == 0) || (year % 400 == 0));
}

static const char *const crinitMonthLookup[] = {
    [0] = "",    [1] = "Jan", [2] = "Feb", [3] = "Mar",  [4] = "Apr",  [5] = "May",  [6] = "Jun",
    [7] = "Jul", [8] = "Aug", [9] = "Sep", [10] = "Oct", [11] = "Nov", [12] = "Dec",
};

static const char *crinitMonthToStr(uint8_t m) {
    return m <= 12 ? crinitMonthLookup[m] : "";
}

int crinitSPrintTimerDef(char *s, crinitTimerDef_t *td) {
    int off = 0;
    if (td->years[0] == td->years[1]) {
        off += sprintf(s + off, "%04d-", td->years[0]);
    } else {
        off += sprintf(s + off, "%04d..%04d-", td->years[0], td->years[1]);
    }
    if (td->month[0] == td->month[1]) {
        off += sprintf(s + off, "%s-", crinitMonthToStr(td->month[0]));
    } else {
        off += sprintf(s + off, "%s..%s-", crinitMonthToStr(td->month[0]), crinitMonthToStr(td->month[1]));
    }
    if (td->days[0] == td->days[1]) {
        off += sprintf(s + off, "%02d ", td->days[0]);
    } else {
        off += sprintf(s + off, "%02d..%02d ", td->days[0], td->days[1]);
    }
    if (td->hours[0] == td->hours[1]) {
        off += sprintf(s + off, "%02d:", td->hours[0]);
    } else {
        off += sprintf(s + off, "%02d..%02d:", td->hours[0], td->hours[1]);
    }
    if (td->minutes[0] == td->minutes[1]) {
        off += sprintf(s + off, "%02d:", td->minutes[0]);
    } else {
        off += sprintf(s + off, "%02d..%02d:", td->minutes[0], td->minutes[1]);
    }
    if (td->seconds[0] == td->seconds[1]) {
        off += sprintf(s + off, "%02d ", td->seconds[0]);
    } else {
        off += sprintf(s + off, "%02d..%02d ", td->seconds[0], td->seconds[1]);
    }
    off += sprintf(s + off, "%c%02d%02d ", td->timezone[0] < 0 ? '-' : '+', abs(td->timezone[0]), td->timezone[1]);
    off +=
        sprintf(s + off, "|%s %s %s %s %s %s %s|", td->wDay & 1 << 0 ? "Mon" : "_  ", td->wDay & 1 << 1 ? "Tue" : "_  ",
                td->wDay & 1 << 2 ? "Wed" : "_  ", td->wDay & 1 << 3 ? "Thu" : "_  ", td->wDay & 1 << 4 ? "Fri" : "_  ",
                td->wDay & 1 << 5 ? "Sat" : "_  ", td->wDay & 1 << 6 ? "Sun" : "_  ");
    return off;
}

void crinitPrintTimerDef(crinitTimerDef_t *td) {
    char s[100];
    crinitSPrintTimerDef(s, td);
    crinitInfoPrint("@timer:%s", s);
}

bool crinitCheckTimerDef(crinitTimerDef_t *td) {
    bool res = true;
    for (size_t i = 0; i < 2; ++i) {
        if (td->month[i] == 0 || td->month[i] > 12) {
            crinitErrPrint("%02d is not a month!\n", td->month[i]);
            res = false;
        }
        if (td->days[i] > 31) {
            crinitErrPrint("%02d is not a valid day of month!\n", td->days[i]);
            res = false;
        }
        if (td->hours[i] > 23) {
            crinitErrPrint("%02d is not a valid hour of the day!\n", td->hours[i]);
            res = false;
        }
        if (td->minutes[i] > 59) {
            crinitErrPrint("%02d is not a valid minute in an hour!\n", td->hours[i]);
            res = false;
        }
        if (td->seconds[i] > 59) {
            crinitErrPrint("%02d is not a valid second in a minute!\n", td->hours[i]);
            res = false;
        }
    }
    if (0 > td->timezone[1] || 59 < td->timezone[1]) {
        crinitErrPrint("%c%02d%02d is not a well formated timezone", td->timezone[0] < 0 ? '-' : '+',
                       abs(td->timezone[0]), td->timezone[1]);
        res = false;
    } else if (-11 > td->timezone[0] || 15 < td->timezone[0]) {
        crinitErrPrint("%c%02d%02d is probbably not a correct timezone", td->timezone[0] < 0 ? '-' : '+',
                       abs(td->timezone[0]), td->timezone[1]);
    }
    if (td->month[0] == td->month[1] &&
        (4 == td->month[0] || 6 == td->month[0] || 9 == td->month[0] || 11 == td->month[0])) {
        if (31 == td->days[0] || 31 == td->days[1]) {
            crinitErrPrint("31 is not a valid day for %s!\n", crinitMonthToStr(td->month[0]));
            res = false;
        }
    }
    if (td->month[0] == td->month[1] && 2 == td->month[0]) {
        uint8_t maxWrong = 31;
        uint8_t minWrong = 30;
        if (td->years[0] == td->years[1] && crinitIsLeapYear(td->years[0])) {
            minWrong = 29;
        }
        for (size_t i = 0; i < 2; ++i) {
            if (minWrong <= td->days[i] && maxWrong > td->days[i]) {
                crinitErrPrint("%02d is not a valid day in %s!\n", td->days[i], crinitMonthToStr(2));
                res = false;
            }
        }
    }
    return res;
}

void crinitTimerAdd(char *timerStr) {
    crinitInfoPrint("@timer:%s", timerStr);
    crinitTimer_t timer = {0};
    timer.name = strdup(timerStr);
    crinitTimerParse(timer.name, &(timer.def));
    crinitPrintTimerDef(&(timer.def));
    // TODO:  initialize, start timerfd and polling in timer thread
}
