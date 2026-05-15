#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "buffer.h"

static int *buffer;
static int fill_ptr = 0;
static int use_ptr = 0;
static int count = 0;
static int max_size = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_space_available = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_data_available = PTHREAD_COND_INITIALIZER;

void buffer_init(int size){
    max_size = size;
    buffer = (int *)malloc(size * sizeof(int));
    if(buffer == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for the bounded buffer.\n");
        exit(1);
    }
}

void buffer_put(int item){
    pthread_mutex_lock(&mutex);
    while(count == max_size){
        pthread_cond_wait(&cond_space_available, &mutex);
    }
    buffer[fill_ptr] = item;
    fill_ptr = (fill_ptr + 1) % max_size;
    count++;
    pthread_cond_signal(&cond_data_available);
    pthread_mutex_unlock(&mutex);
}

int buffer_get(){
    pthread_mutex_lock(&mutex);
    while(count == 0){
        pthread_cond_wait(&cond_data_available, &mutex);
    }
    int item = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % max_size;
    count--;
    pthread_cond_signal(&cond_space_available);
    pthread_mutex_unlock(&mutex);
    return item;
}
