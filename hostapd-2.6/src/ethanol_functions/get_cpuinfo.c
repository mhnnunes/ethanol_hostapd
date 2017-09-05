#include "get_cpuinfo.h"

#define MAX 5
// units of values from /proc/stat is in USER_HZ

/*  legenda: /proc/stat
	user: normal processes executing in user mode
    nice: niced processes executing in user mode
    system: processes executing in kernel mode
    idle: twiddling thumbs
    iowait: waiting for I/O to complete
    irq: servicing interrupts
    softirq: servicing softirqs*/
/*
user niced system idle iowait   irq   softirq
cpu  342903 2540 83802 19313932 58122 1 228 0 0 0
cpu0 88448 542 23472 4927035 31712 1 174 0 0 0
cpu1 83544 496 19218 4801334 7240 0 11 0 0 0
cpu2 87274 918 22071 4790217 5286 0 30 0 0 0
cpu3 83636 583 19040 4795345 13884 0 12 0 0 0
*/

/*A execução desse método varia conforme a definição de MAX
no caso se MAX = 10, então no mínimo será requirido 10 segundos (sleep) para a execução dos calculos
para verificação da procentagem de utilização da cpu*/

 double get_cpu_info(){
    FILE *file; 
    int i;
     double fst[4], snd[4], percent = 0.0, values[MAX], sum = 0.0, avg_percent;

    for(i = 0;i < MAX; i++) {
        file = fopen(PROC_STAT,"r");
        if(fscanf(file,"%*s %lf %lf %lf %lf",&fst[0],&fst[1],&fst[2],&fst[3])){}
        //printf("1 - %.0f %.0f %.0f %.0f\n",fst[0],fst[1],fst[2],fst[3]);
        fclose(file);
        sleep(1);//para segundos
        //usleep(500);// para microsegundos

        file = fopen(PROC_STAT,"r");
        if(fscanf(file,"%*s %lf %lf %lf %lf",&snd[0],&snd[1],&snd[2],&snd[3])){}
        //printf("2 - %.0f %.0f %.0f %.0f\n",snd[0],snd[1],snd[2],snd[3]);
        fclose(file);


        percent = ((snd[0]+snd[1]+snd[2]) - (fst[0]+fst[1]+fst[2])) / ((snd[0]+snd[1]+snd[2]+snd[3]) - (fst[0]+fst[1]+fst[2]+fst[3]));
        //printf(" 0 = 2(niced   system   idle) - 1(niced   system    idle) / (2 niced system idle iowait) - (1 niced system idle iowait)\n");
        //printf(" %.0f = (%.0f +  %.0f +  %.0f) - (%.0f +  %.0f +  %.0f) / (%.0f + %.0f + %.0f + %.0f) - (%.0f + %.0f + %.0f + %.0f)\n", percent, snd[0], snd[1], snd[2], fst[0], fst[1], fst[2], snd[0], snd[1], snd[2], snd[3], fst[0], fst[1], fst[2], fst[3]);
        //printf("CPU usage : %.4f\n",percent);
        values[i]= percent;
    }
    sum = 0;
    for(i = 0; i < MAX; i++){
        sum += values[i];
        // printf("Somatorio: %.4f - valor: %.4f\n",sum, values[i]);
    }
    avg_percent = (sum * 100.00) / MAX;

return avg_percent;
}

#ifdef USE_MAIN
int main() {
    double cpu_used;
    printf("Calculating cpu usage... Wait a moment\n");
    cpu_used = get_cpu_info();
    printf("cpu_used: %.2lf\n", cpu_used);
    return 0;


 }

#endif
