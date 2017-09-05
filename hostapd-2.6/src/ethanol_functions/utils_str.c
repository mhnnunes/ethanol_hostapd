#include <string.h>
#include <stdlib.h>

// copy origin (orig) to destination (dest)
// starts freeing destination if dest is not NULL
//
void copy_string(char ** dest, char * orig){
  if (*dest) free(*dest);
  if (orig) {
    *dest = malloc(strlen(orig + 1) * sizeof(char));
    strcpy(*dest, orig);
  } else { // orig == NULL, so
    *dest = NULL;
  }
}
