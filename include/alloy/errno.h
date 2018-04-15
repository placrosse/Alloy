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

/** An invalid address was passed
 * to a function.
 * @ingroup errno
 * */

#define ALLOY_EFAULT 3

/** An input or output error occured.
 * @ingroup errno
 * */

#define ALLOY_EIO 4

/** An invalid argument was passed
 * to a function.
 * @ingroup errno
 * */

#define ALLOY_EINVAL 5

#endif /* ALLOY_ERRNO_H */