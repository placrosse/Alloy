/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_KEYS_H
#define ALLOY_KEYS_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup keys Keys
 * Defines several keyboard keys
 * used for input processing.
 * */

/** The left arrow key.
 * @ingroup keys
 * */

#define ALLOY_KEY_LEFT 0x01

/** The right arrow key.
 * @ingroup keys
 * */

#define ALLOY_KEY_RIGHT 0x02

/** The up arrow key.
 * @ingroup keys
 * */

#define ALLOY_KEY_UP 0x03

/** The down arrow key.
 * @ingroup keys
 * */

#define ALLOY_KEY_DOWN 0x04

/** The 'Return' key. Also
 * labelled as 'Enter'.
 * @ingroup keys
 * */

#define ALLOY_KEY_RETURN 0x0a

/** The 'Backspace' key.
 * @ingroup keys
 * */

#define ALLOY_KEY_BACKSPACE 0x0e

/** This is a pseudo key.
 * When this key is encountered,
 * it means the application window
 * was closed. This is only relevent
 * in GUI implementations.
 * @ingroup keys
 * */

#define ALLOY_KEY_QUIT 0x7f

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_KEYS_H */
