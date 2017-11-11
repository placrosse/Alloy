#ifndef ALLOY_ERROR_H
#define ALLOY_ERROR_H

/** An operation can't be completed
 * because there is no more space available
 * in a buffer. */

#define ALLOY_ERROR_BUFFER_FULL 0x01

/** Alloy doesn't understand (or doesn't support) a
 * character that was passed to it.*/

#define ALLOY_ERROR_UNKNOWN_CHARACTER 0x02

#endif /* ALLOY_ERROR_H */
