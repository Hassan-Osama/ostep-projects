#include <stdio.h>
#include <pthread.h>
#include "request.h"
#include "io_helper.h"
#include "buffer.h"

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 

void *worker_thread_loop(void *arg){
	int id = *((int *)arg);
	free(arg);
	while(1){
		int conn_fd= buffer_get();
		printf("Worker [%d] received connection [%d]\n", id, conn_fd);
		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
}

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	int threads = 1;
	int buffers = 1;
	char *schedalg = "FIFO";
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		threads = atoi(optarg);
		if(threads <= 0){
			fprintf(stderr, "Error: threads must be a positive integer.\n");
			exit(1);
		}
		break;
	case 'b':
		buffers = atoi(optarg);
		if(buffers <= 0){
			fprintf(stderr, "Error: buffers must be a positive integer.\n");
			exit(1);
		}
		break;
	case 's':
		schedalg = optarg;
		if(strcmp(schedalg, "FIFO")!=0 && strcmp(schedalg, "SFF")!=0){
			fprintf(stderr, "Error: schedalg must be FIFO or SFF.\n");
			exit(1);
		}
		break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]\n");
	    exit(1);
	}

	buffer_init(buffers);

	pthread_t pool[threads];
	for(int i=0;i<threads;i++){
		int *id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&pool[i], NULL, worker_thread_loop, id);
	}

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		buffer_put(conn_fd);
    }
    return 0;
}
