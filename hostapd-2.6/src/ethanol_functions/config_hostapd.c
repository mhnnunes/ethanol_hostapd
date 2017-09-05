#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h> // log, trunc

/** acha o PID do hostapd */
char * get_pid(void){
  FILE *pp;
  pp = popen("ps axf | grep -w \"hostapd \" | grep -v grep | awk '{print $1}'", "r");
  char * pidfile = NULL;
  int hostapd_pid = -1;
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;      
      //if (line[0] == 'd') 
      int pid;
      sscanf(line, "%d", &pid);
      if (hostapd_pid == -1 || pid < hostapd_pid) hostapd_pid = pid;
    }
    pclose(pp);
  }
  if (hostapd_pid >= 0) {
    int digitos = (int) trunc(log10(hostapd_pid)) + 2; // já considera o \0
    pidfile = (char*)malloc(digitos * sizeof(char));
    sprintf(pidfile, "%d", hostapd_pid);    
  }

  return pidfile;
}

/** recarrega o hostapd */
void hostapd_reload(const char *pidfile) {
  FILE *pp;
  char cmd[20];
  sprintf((char*) &cmd, "kill -1 %s", pidfile);
  pp = popen(cmd, "r");
  if (pp != NULL) pclose(pp);
}

typedef struct pair {
    char * var;
    char * val;
} type_pair;

void limpa_string(char * s) {
    int i;
    for(i = 0; i < strlen(s); i++)
        if (s[i] == '\n') s[i] = ' ';
    
    int j;
    for(j = 0; j < strlen(s); j++)
        if (s[j] != ' ') break;
    int k;
    for(k = strlen(s)-1; k > 0; k--)
        if (s[k] != ' ') break;

    for(i = 0; i < k - j; i++)
        s[i] = s[j+i];

    for(i = k - j + 1; i < strlen(s); i++)
        s[i] = '\0';
}

type_pair * hostapd_split_conf(const char * entry) {

  char * line = malloc(sizeof(char) * (strlen(entry)+1));
  strcpy(line, entry);

  type_pair * result = malloc(sizeof(type_pair));
  result->var = NULL;
  result->val = NULL;
  char * pch = strtok (line,"=");
  if (pch) {
    result->var = malloc(sizeof(char) * (strlen(pch)+1));
    strcpy(result->var, pch);
    limpa_string(result->var);
    pch = strtok (NULL,"=");
    if (pch) {
        result->val = malloc(sizeof(char) * (strlen(pch)+1));
        strcpy(result->val, pch);
        limpa_string(result->val);
    }
    #ifdef DEBUG
        printf("<%s><%s>\n", result->var, result->val);
    #endif
  }
  return result;
}

#define LINE_SIZE 2048 
typedef struct conf_lines {
    type_pair l;
    struct conf_lines * n; 
} conf_type;

// TODO: tem um erro neste procedimento
//       duplica a última linha, quando a última linha do arquivo não tem \n
conf_type * hostapd_read_conf(const char * conffile) {
    conf_type * ultimo = NULL;
    conf_type * result = NULL;

    FILE * fp;
    if ( (fp = fopen(conffile, "r")) != NULL) {
        while (!feof(fp)) {
            char line[LINE_SIZE];
            if (fgets(line, LINE_SIZE, fp) != NULL) {
                type_pair * pair = hostapd_split_conf(line);
                conf_type * temp = malloc(sizeof(conf_type));
                temp->l.var = pair->var;
                temp->l.val = pair->val;
                free(pair);
                temp->n = NULL;
                
                if (ultimo == NULL) {
                    ultimo = result = temp; // result é a cabeça da lista
                } else {
                    ultimo->n = temp;
                    ultimo = temp;
                }                
            }
        }
        fclose(fp);
    }
    return result;
}

int change_entry(conf_type * conf, const char * var, const char * val) {
    conf_type * p = conf;
    while (p) {
        if (strcmp(p->l.var, var) == 0) {
            free(p->l.val);
            p->l.val = malloc(sizeof(char) * (strlen(val)+1) );
            strcpy(p->l.val, val);
            break;
        }
        p = p->n;
    }
    return true;
}


void hostapd_write_conf(const char * conffile, conf_type * conf) {
    conf_type * p = conf;
    FILE * fp;
    if ( (fp = fopen(conffile, "w")) != NULL) {
        while (p) {
            if (p->l.val == NULL) {
                fprintf(fp, "%s\n", p->l.var);
            } else {
                fprintf(fp, "%s=%s\n", p->l.var, p->l.val);
            }
            p = p->n;
        }
        fclose(fp);
    }
}

void print_conf(conf_type * conf){
  printf("****** Arquivo de configuração ******\n");
  conf_type * p = conf;
  while (p) {
    printf("%s=%s\n", p->l.var, p->l.val);
    p = p->n;
  }    
}

int change_hostapd_entry(const char * conffile, const char * var, const char * new_val) {
  char * pidfile = get_pid();
  if (pidfile == NULL) {
    #ifdef DEBUG
        printf("Hostapd process is not running.\n");
    #endif
    return -1;
  }
  #ifdef DEBUG
    printf("pid do hostapd: %s\n", pidfile);
  #endif
  conf_type * conf = hostapd_read_conf(conffile);
  #ifdef DEBUG
    printf("Arquivo lido:\n");
    print_conf(conf);
  #endif

  if (change_entry(conf, var, new_val) < 0) {
    #ifdef DEBUG
        printf("Entry <%s> not found!\n", var);
    #endif
    return -2;
  }
  hostapd_write_conf(conffile, conf);
  #ifdef DEBUG
    printf("\nArquivo escrito:\n");
    print_conf(conf);
  #endif
  hostapd_reload(pidfile);
  return 0;
}

int change_ap_channel(const char * conffile, int new_val) {
    int digitos = (int) trunc(log10(new_val)) + 2;
    char * val = (char*)malloc(sizeof(char) * digitos);
    sprintf(val, "%d", new_val);    

    return change_hostapd_entry(conffile, "channel", val);
}

#ifdef USE_MAIN
int main(int argc, char * argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Uso: %s <CONFFILE> <VAL>\n", argv[0]);
    return 1;
  }
  const char *conffile = argv[1];
  int val = atoi(argv[2]);

  if (change_ap_channel(conffile, val) >= 0) 
    printf("comando para troca do canal foi executado\n");

  return 0;
}
#endif