#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    int cnt = 0;
    int prev = -1;
    char buffer[2048];
    for(int i=1;i<argc;i++){
        FILE *f = fopen(argv[i], "r");
        size_t n;
        while((n = fread(buffer, 1, sizeof buffer, f)) > 0){
            for(size_t j=0;j<n;j++){
                if(prev == -1){
                    prev = buffer[j];
                    cnt=1;
                }
                else if(buffer[j] == prev){
                    cnt++;
                }else{
                    fwrite(&cnt, 4, 1, stdout);
                    char outc = (char) prev;
                    fwrite(&outc, 1, 1, stdout);
                    prev = buffer[j];
                    cnt = 1;
                }
            }
        }
        fclose(f);
    }
    if(prev != -1){
        fwrite(&cnt, 4, 1, stdout);
        char outc = (char)prev;
        fwrite(&outc, 1, 1, stdout);
    }
    return 0;
}
