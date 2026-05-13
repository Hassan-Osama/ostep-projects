#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct Node
{
    char* data;
    struct Node* next;
} Node;

Node* createNode(char* data){
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = strdup(data);
    node->next = NULL;
    return node;
}

void insert(Node** list, char* data){
    Node *node = createNode(data);
    node->next = *list;
    *list = node;
}

void print(Node* list, FILE* out){
    Node* curr = list;
    FILE *f=NULL;
    while(curr != NULL){
        fprintf(out, "%s", curr->data);
        curr = curr->next;
    }
}

void freeList(Node* list){
    Node* curr = list;
    Node* next;
    while(curr != NULL){
        next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }
}

int main(int argc, char*argv[]){
    FILE *in = stdin;
    FILE *out = stdout;
    if(argc > 3){
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }
    if(argc >= 2){
        in = fopen(argv[1], "r");
        if(in == NULL){
            fprintf(stderr, "reverse: cannot open file '%s'\n",argv[1]);
            return 1;
        }
        out = fopen(argv[2], "w");
    }
    if(argc == 3){
        struct stat st1, st2;
        if(stat(argv[1], &st1)==0 && stat(argv[2], &st2)==0){
            if(st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino){
                fprintf(stderr, "reverse: input and output file must differ\n");
                return 1;
            }
        }
    }
    char *line = NULL;
    size_t linecap=0;
    Node* list = NULL;
    while(getline(&line, &linecap, in) != -1){
        insert(&list, line);
    }
    free(line);
    print(list, out);
    freeList(list);
    fclose(in);
    fclose(out);
    return 0;

}
