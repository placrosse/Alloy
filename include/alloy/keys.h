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

/** The 'Return' key. Also
 * labelled as 'Enter'.
 * */
#define ALLOY_KEY_RETURN 0x1c

/** The 'Backspace' key.
 * */

#define ALLOY_KEY_BACKSPACE 0x0e

/** This is a pseudo key.
 * When this key is encountered,
 * it means the application window
 * was closed. This is only relevent
 * in GUI implementations.
 * */

#define ALLOY_KEY_QUIT 0x7f

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_KEYS_H */
