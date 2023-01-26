/* This file contains miscellaneous macros that are useful to other files but
 * aren't too niche to only be needed by a single file.
 */

#ifndef MACROS_H
#define MACROS_H

// This can only be used on array types with size of at least 1.
// DO NOT use this on pointers, as the result will be meaningless.
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#endif // MACROS_H
