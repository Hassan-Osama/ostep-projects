#include <stdio.h>

int main(int argc, char *argv[]){
    char buffer[4096];
    for(int i=1;i<argc;i++){
        FILE *f = fopen(argv[i], "r");
        if(f == NULL){
            printf("wcat: cannot open file\n");
            return 1;
        }
        while(fgets(buffer, sizeof(buffer), f)){
            fputs(buffer, stdout);
        }
        fclose(f);
    }
    return 0;
}
