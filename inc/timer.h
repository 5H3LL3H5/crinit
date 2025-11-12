#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/**
 * The type of a timer definition.
 */
typedef struct crinitTimerDef {
    uint8_t wDay;
    uint16_t years[2];
    uint8_t month[2];
    uint8_t days[2];
    uint8_t hours[2];
    uint8_t minutes[2];
    uint8_t seconds[2];
    int8_t timezone[2];
} crinitTimerDef_t;

/**
 * The type of a crinit timer object.
 */
typedef struct crinitTimer {
    crinitTimerDef_t def;
    char *name;
} crinitTimer_t;

/**
 * Set a timer to the default value:
 * Mon..Sun-0000..65535-01..12-01..31-00:00:00+0000
 *
 * @param td  the crinitTimerDef_t to set to default
 */
void crinitTimerSetDefault(crinitTimerDef_t *td);

/**
 * Check if a timer definition gives a valid timer
 */
bool crinitCheckTimerDef(crinitTimerDef_t *td);

/**
 * Check if a value is in a closed-open range (test in [start, end) ) ie start <= test < end.
 */
#define CO_RANGE(start, test, end) (((start) <= (test)) && ((test) < (end)))
/**
 * Check if a value is in a closed-closed range (test in [start, end] ) ie start <= test <= end.
 */
#define CC_RANGE(start, test, end) (((start) <= (test)) && ((test) <= (end)))
/**
 * Check if a value is in a open-closed range (test in (start, end] ) ie start < test <= end.
 */
#define OC_RANGE(start, test, end) (((start) < (test)) && ((test) <= (end)))
/**
 * Check if a value is in a open-open range (test in (start, end) ) ie start < test < end.
 */
#define OO_RANGE(start, test, end) (((start) < (test)) && ((test) < (end)))

/**
 * print a crinitTimerDef_t into a string
 *
 * @param s   the string to print to
 * @param td  the crinitTimerDef_t to print
 *
 * @return the number of bytes written to s excluding the terminating 0 byte
 */
int crinitSPrintTimerDef(char *s, crinitTimerDef_t *td);
/**
 * print a crinitTimerDef_t as crinitInfoPrint message
 *
 * @param td  the crinitTimerDef_t to print
 */
void crinitPrintTimerDef(crinitTimerDef_t *td);

/**
 * Adds a timer to crinit
 *
 * @param timerStr  the configuration string for the timer
 */
void crinitTimerAdd(char *timerStr);
/**
 * Parses a timer definition from a string
 *
 * @param s   the string to parse from
 * @param td  the crinitTimerDef_t to set
 *
 * @return true on success, false otherwise
 */
bool crinitTimerParse(char *s, crinitTimerDef_t *td);

#endif  // __TIMER_H__
