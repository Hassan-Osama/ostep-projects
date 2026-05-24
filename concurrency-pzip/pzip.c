#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    uint32_t count;
    char c;
} RLERun;

typedef struct {
    RLERun *runs;
    size_t run_count;
    size_t capacity;
    int completed;
} ChunkResult;

ChunkResult *results_array;
pthread_mutex_t results_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t results_cond = PTHREAD_COND_INITIALIZER;

typedef struct{
    int chunk_id;
    char *start_ptr;
    size_t size;
}Task;

typedef struct{
    Task *tasks;
    int head;
    int tail;
    int cnt;
    int capacity;
    pthread_mutex_t lock;
    pthread_cond_t cond;
}TaskQueue;

void queue_init(TaskQueue *q, int capacity){
    q->tasks = malloc(capacity * sizeof(Task));
    q->head = 0;
    q->tail = 0;
    q->cnt = 0;
    q->capacity = capacity;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void queue_push(TaskQueue *q, Task t){
    pthread_mutex_lock(&q->lock);
    q->tasks[q->tail] = t;
    q->tail = (q->tail + 1) % q->capacity;
    q->cnt++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
}

Task queue_pop(TaskQueue *q){
    pthread_mutex_lock(&q->lock);
    while(q->cnt == 0){
        pthread_cond_wait(&q->cond, &q->lock);
    }
    Task ret = q->tasks[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->cnt--;
    pthread_mutex_unlock(&q->lock);
    return ret;
}

void queue_destroy(TaskQueue *q){
    free(q->tasks);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->cond);
}


void write_run(int cnt, char c){
    fwrite(&cnt, 4, 1, stdout);
    fwrite(&c, 1, 1, stdout);
}

void *worker_routine(void* arg){
    TaskQueue *q = (TaskQueue *)arg;

    while(1){
        Task t = queue_pop(q);
        if(t.size == 0){
            break;
        }
        size_t capacity = 1024;
        RLERun *local_runs = malloc(capacity * sizeof(RLERun));
        size_t run_cnt = 0;
        char curr_char = t.start_ptr[0];
        uint32_t curr_cnt = 1;
        for(size_t i=1;i<t.size;i++){
            if(t.start_ptr[i] == curr_char){
                curr_cnt++;
            }
            else{
                if(run_cnt >= capacity){
                    capacity *=2;
                    local_runs = realloc(local_runs, capacity * sizeof(RLERun));
                }
                local_runs[run_cnt].count = curr_cnt;
                local_runs[run_cnt].c = curr_char;
                run_cnt++;
                curr_char = t.start_ptr[i];
                curr_cnt = 1;
            }
        }
        if(run_cnt>=capacity){
            capacity +=1;
            local_runs = realloc(local_runs, capacity*sizeof(RLERun));
        }
        local_runs[run_cnt].count = curr_cnt;
        local_runs[run_cnt].c = curr_char;
        run_cnt++;
        pthread_mutex_lock(&results_lock);
        results_array[t.chunk_id].runs = local_runs;
        results_array[t.chunk_id].run_count = run_cnt;
        results_array[t.chunk_id].capacity = capacity;
        results_array[t.chunk_id].completed = 1;
        pthread_cond_broadcast(&results_cond);
        pthread_mutex_unlock(&results_lock);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: ./pzip <filename>\n");
        return 1;
    }

    int f = open(argv[1], O_RDONLY);
    if(f == -1){
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    struct stat sb;
    if(fstat(f, &sb) == -1){
        fprintf(stderr, "Error getting file size\n");
        close(f);
        return 1;
    }

    size_t file_size = sb.st_size;

    if(file_size == 0){
        close(f);
        return 0;
    }

    char *data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);
    if(data == MAP_FAILED){
        fprintf(stderr, "Error mapping file\n");
        close(f);
        return 1;
    }
    close(f);

    int nprocs = (int)sysconf(_SC_NPROCESSORS_ONLN);
    fprintf(stderr, "[system] Detected %d CPU cores\n", nprocs);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    TaskQueue queue;
    queue_init(&queue, nprocs * 2);

    size_t chunk_size = file_size / nprocs;
    size_t remainder = file_size % nprocs;
    results_array = calloc(nprocs, sizeof(ChunkResult));
    pthread_t *pool = malloc(nprocs * sizeof(pthread_t));
    for(int i=0;i<nprocs;i++){
        pthread_create(&pool[i], NULL, worker_routine, &queue);
    }
    size_t current_offset = 0;
    int tasks_created = 0;
    for(int i=0;i<nprocs;i++){
        Task t;
        t.chunk_id = i;
        t.start_ptr = data+current_offset;
        t.size = chunk_size + (i==nprocs-1?remainder: 0);
        if(t.size>0){
            queue_push(&queue, t);
            current_offset += t.size;
            tasks_created++;
        }
    }
    for(int i = 0; i < nprocs; i++) {
        Task pill;
        pill.size = 0;
        queue_push(&queue, pill);
    }
    uint32_t pending_count = 0;
    char pending_char = '\0';
    int has_pending = 0;

    for(int i=0;i<tasks_created;i++){
        pthread_mutex_lock(&results_lock);
        while (results_array[i].completed == 0) {
            pthread_cond_wait(&results_cond, &results_lock);
        }
        pthread_mutex_unlock(&results_lock);
        for(size_t j=0;j<results_array[i].run_count;j++){
            RLERun current_run = results_array[i].runs[j];
            if(!has_pending){
                pending_char = current_run.c;
                pending_count = current_run.count;
                has_pending = 1;
            }
            else if(current_run.c== pending_char){
                pending_count+=current_run.count;
            }
            else{
                write_run(pending_count, pending_char);
                pending_char=current_run.c;
                pending_count = current_run.count;
            }
        }
        free(results_array[i].runs);
    }
    if (has_pending) {
        write_run(pending_count, pending_char);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fprintf(stderr, "[benchmark] Time: %.4f s\n", time_taken);
    fprintf(stderr, "[benchmark] Throughput: %.2f MB/s\n", (file_size / 1024.0 / 1024.0) / time_taken);

    for(int i = 0; i < nprocs; i++) {
        pthread_join(pool[i], NULL);
    }
    free(results_array);
    free(pool);
    queue_destroy(&queue);
    munmap(data, file_size);
    return 0;
}
