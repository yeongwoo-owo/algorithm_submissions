#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char base[4];

int main(void){
    FILE* fp=fopen("hw2_input.txt", "w");
    srand(time(NULL));
    base[0]='A', base[1]='T', base[2]='G', base[3]='C';

    fprintf(fp, "%d\n$\n", 5);
    for(int i=0;i<5;i++){
        // int len=rand()%99+1;
        for(int j=0;j<60;j++){
            fprintf(fp, "%c", base[rand()%4]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}