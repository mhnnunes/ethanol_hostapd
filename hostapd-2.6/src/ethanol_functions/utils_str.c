#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

/** trim string "s"  returning a new string
    "s" remains unchanged
*/
char * trim_string(char * s) {
    char * result = NULL;
    if (s) {
        int i = 0;
        while (i < strlen(s) && (s[i] == ' ' || s[i] == '\t')) i++;
        int f = strlen(s)-1;
        while (f >= i && (s[f] == ' ' || s[f] == '\t' || s[f] == '\n')) f--;
        if (f >= i) {
            int tam = f - i + 1;
            result = malloc(sizeof(char) * (tam + 1));
            strncpy(result, (const char *)&s[i], tam );
            result[tam] = '\0';
        }
    }
    return result;
}