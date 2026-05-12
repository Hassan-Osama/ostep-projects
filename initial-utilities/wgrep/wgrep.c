#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void grep_file(FILE *f, const char *word){
    char *line = NULL;
    size_t linecap = 0;
    while(getline(&line, &linecap, f)>0){
        if(strstr(line, word) != NULL){
            printf("%s", line);
        }
    }
    free(line);
}

int main(int argc, char* argv[]){
    if(argc == 1){
        puts("wgrep: searchterm [file ...]");
        return 1;
    }
    const char* word = argv[1];
    if(argc == 2){
        grep_file(stdin, word);
    }
    for(int i=2;i<argc;i++){
        FILE *f = fopen(argv[i], "r");
        if(f==NULL){
            puts("wgrep: cannot open file");
            return 1;
        }
        grep_file(f, word);
        fclose(f);
    }
    return 0;
}
