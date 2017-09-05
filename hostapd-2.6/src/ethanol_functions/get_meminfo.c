#include "get_meminfo.h"

#define MAX 50


/*Manipulate the memory file of Kernel Linux to gather information
about memtotal and memfree */

struct mem_info * get_mem_info(){
  struct mem_info *str = NULL;
  FILE *file = fopen(PROC_MEMINFO, "r");
  int i;
  long *values;
  char buffer[MAX], * column1, *column2;

  values = malloc(sizeof(long)* MAX);
  column1 = malloc(sizeof(char) * MAX);
	column2 = malloc(sizeof(char) * MAX);

  if(file == NULL) {
  	#ifdef DEBUG
    	printf("Error: couldn't find the required file.\n");
    #endif
  }
  else {
    for(i = 0; fgets(buffer, MAX, file) != NULL; i++) {
      sscanf(buffer, "%s %s", column1, column2);
      //printf("column2: %s\n", column2);
      values[i] = atoi(column2);
    }
    fclose(file);
  }

  str = malloc(sizeof(struct mem_info));
  str->memtotal = values[0];
  str->memfree = values[1];

  free(values);
  free(column1);
  free(column2);

  return str;
}

/*Compute the percentage usage of memory. To do this, it is necessary only
memory total lenght minus memory free.*/

float get_mem_porcentage(){
  float percentage;
  long mem_used;
  struct mem_info * q = get_mem_info();

  mem_used = q->memtotal - q->memfree;
  percentage = (mem_used * 100.0)/q->memtotal;

  free(q);
  return percentage;
}

#ifdef USE_MAIN
int main() {
  long result;
  struct mem_info * q = get_mem_info();

  printf("memtotal: %li memfree:%li\n",
           q->memtotal,
           q->memfree);

  free(q);

  result = get_mem_porcentage();
  printf("memory usage: %li\n", result);
  return 0;
}
#endif
