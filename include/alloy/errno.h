#ifndef ALLOY_ERRNO_H
#define ALLOY_ERRNO_H

/** @defgroup errno Error Codes
 * Used to describe what went wrong when
 * a function fails.
 * */

/** This error code means that the
 * functionality required by the function
 * was not implemented.
 * @ingroup errno
 * */

#define ALLOY_ENOSYS 1

/** This means the function could not
 * be completed because a memory allocation
 * failed.
 * @ingroup errno
 * */

#define ALLOY_ENOMEM 2

#endif /* ALLOY_ERRNO_H */
