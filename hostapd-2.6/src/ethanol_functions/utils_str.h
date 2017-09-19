#ifndef __UTILS_STR_H
#define __UTILS_STR_H

/**
  if dest == NULL, copy_char allocates memory space and then copy
  if dest != NULL, copy_char frees this memory, allocates new space and then copy

  CAUTION:
    1) dest cannot be a char[]
    2) be sure that dest is an assigned variable (malloc'ed) or is NULL
       don't trust that the compiler will set the variable to NULL
 */
void copy_string(char ** dest, char * orig);

/** returns a new string trimmed
    s remains the same
*/
char * trim_string(char * s);

#endif