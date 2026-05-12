#include <stdio.h>

int main(int argc, char* argv[]){
    if(argc == 1){
        puts("wunzip: file1 [file2 ...]");
        return 1;
    }
    for(int i=1;i<argc;i++){
        FILE *f = fopen(argv[i], "r");
        int cnt;
        while(fread(&cnt, 4, 1, f)>0){
            char c;
            fread(&c, 1, 1, f);
            for(int j=0;j<cnt;j++){
                printf("%c", c);
            }
        }
        fclose(f);
    }
}
