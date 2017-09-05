#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include "get_phyintf.h"

struct phy_entries * get_entries() {

    DIR* FD;
    struct dirent* entry;

    struct phy_entries * ans = NULL;

    /* Scanning the in directory */
    if (NULL == (FD = opendir("/sys/class/ieee80211"))) {
    	fprintf(stderr, "Couldn't open physical interfaces system dir!\n");
        return ans;
    }
    ans = malloc(sizeof(struct phy_entries));
    ans->num = 0;
    while ((entry = readdir(FD))) {
        if (!(strcmp(entry->d_name, "."))) continue; // pula diretórios ponto
        if (!(strcmp(entry->d_name, ".."))) continue; // pula diretórios ponto
        ans->num++;
    }
    rewinddir(FD);
    ans->phy = malloc(ans->num * sizeof(char *));
    int i = 0;
    while ((entry = readdir(FD))) {
        if (!(strcmp(entry->d_name, "."))) continue; // pula diretórios ponto
        if (!(strcmp(entry->d_name, ".."))) continue; // pula diretórios ponto
        ans->phy[i] = malloc(strlen(entry->d_name)+1);
        strcpy(ans->phy[i], entry->d_name);
        i++;
    }
    return ans;
}

#ifdef CREATE_MAIN
int main(int argc, char **argv) {
    
    struct phy_entries * e = get_entries();
    printf("%d\n", e->num);
    int i;
    for(i = 0; i < e->num; i++)
        printf("%s\n",e->phy[i]);
    return 0;
}
#endif
