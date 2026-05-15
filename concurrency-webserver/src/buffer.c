#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"

static int *buffer;
static int fill_ptr = 0;
static int use_ptr = 0;
static int count = 0;
static int max_size = 0;

void buffer_init(int size){
    max_size = size;
    buffer = (int *)malloc(size * sizeof(int));
    if(buffer == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for the bounded buffer.\n");
        exit(1);
    }
}
