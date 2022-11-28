#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

int n[2];
int q;
int *shmptr;
int msgqid;
const int BUFF = 1e9;
const int MSGTYPE = 1;
const int MSGSIZ = 8;
const int MSGFLG = -1;

typedef struct thr_args {
    int matrix_num;     //0 or 1 corresponding to inp1.txt or inp2.txt
    int row_idx;        //starting row index for the thread
    int read_cnt;       //number of rows to be read
    FILE *ptr;          //should point to row_idx inside file containing matrix_num
} thread_args;

typedef struct msgbuf {
    long type;
    int matrix_num, row_idx;
} row_info;

void *read_row(void *args) {
    thread_args *targs = args;
    // printf("Inside thread with tid: %ld\n", pthread_self());
    // printf("Matrix #: %d, Row #: %d\n", targs->matrix_num, targs->row_idx);
    for (int rc = 0; rc < targs->read_cnt; ++rc) {
        int offset = (targs->matrix_num * n[0] + (targs->row_idx + rc)) * q;
        for (int i = 0; i < q; ++i) {
            fscanf(targs->ptr, "%d", (shmptr + offset + i));
            // printf("Tid: %ld, x = %d\n", pthread_self(), buff[i]);
        }
        row_info info = {
            .type = MSGTYPE,
            .matrix_num = targs->matrix_num,
            .row_idx = targs->row_idx + rc
        };
        // printf("P1: Matrix #: %d, Row #: %d\n", info.matrix_num, info.row_idx);
        msgsnd(msgqid, (void *)&info, MSGSIZ, MSGFLG);
    }
}

int main(int argc, char **argv) {

    if (argc != 9) {
        printf("Invalid arguments!, argc = %d\n", argc);
        return 0;
    }
    for (int i = 1; i <= 8; ++i) {
        printf("P1: args[%d]: %s\n", i, argv[i]);
    }
    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);
    char *in[2];
    in[0] = argv[4];
    in[1] = argv[5];
    char *csv = argv[6];
    int num_threads = atoi(argv[7]);
    int num_threads_o = atoi(argv[8]);

    FILE *fp[2];
    for (int i = 0; i < 2; ++i) {
        fp[i] = fopen(in[i], "r");
        assert(fp[i] != NULL);
    }
    
    if (num_threads > n[0] + n[1]) {
        num_threads = n[0] + n[1];
    }
    // printf("Num_threads: %d\n", num_threads);

    key_t shmtoken = 1414;
    if (shmtoken == -1)
    {
        perror("P1: Key");
        return 1;
    }
    int shmid = shmget(shmtoken, BUFF, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("P1: Shared memory id");
        return 1;
    }
    shmptr = shmat(shmid, 0, 0);
    if (shmptr == (void *)-1)
    {
        perror("P1: Shared memory pointer");
        return 1;
    }

    key_t msgtoken = 4545;
    msgqid = msgget(msgtoken, 0644 | IPC_CREAT);

    pthread_t tid[num_threads];

    int num_th[2];
    if (num_threads > 1) {
        num_th[0] = n[0] * num_threads / (n[0] + n[1]);
        if (num_th[0] == 0) {
            num_th[0] = 1;
        } else if (num_th[0] == num_threads) {
            num_th[0] = num_threads - 1;
        }
        num_th[1] = num_threads - num_th[0];
    } else {
        for (int i = 0; i < 2; ++i) {
            num_th[i] = 1;
        }
    }

    FILE *fp_arr[num_th[0] + num_th[1]];

    for (int mtx_num = 0; mtx_num < 2; ++mtx_num) {
        int row_cnt = (n[mtx_num] + num_th[mtx_num] - 1) / num_th[mtx_num];

        int fp_off = mtx_num * num_th[0];
        fp_arr[fp_off] = fp[mtx_num];
        int offset = 0;
        
        for (int i = 1; i < num_th[mtx_num]; ++i) {
            FILE *ptr = fopen(in[mtx_num], "r");
            assert(ptr != NULL);
            
            fseek(ptr, offset, SEEK_SET);
            
            int line_cnt = 0;
            while (line_cnt < row_cnt) {
                int x = fgetc(ptr);
                // printf("%c", x);
                line_cnt += x == '\n' || x == EOF;
                offset++;
                // assert(x != EOF);
            }

            fp_arr[fp_off + i] = ptr;
        }
    }

    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int mtx_num = 0; mtx_num < 2; ++mtx_num) {
        int row_cnt = (n[mtx_num] + num_th[mtx_num] - 1) / num_th[mtx_num];
        int rem = n[mtx_num];
        int fp_off = mtx_num * num_th[0];
        int tid_off = mtx_num * num_th[0];

        for (int i = 0; i < num_th[mtx_num]; ++i) {
            thread_args *targs = malloc(sizeof(thread_args));
            
            targs->matrix_num = mtx_num;
            targs->row_idx = i * row_cnt;
            targs->read_cnt = rem < row_cnt ? rem : row_cnt;
            targs->ptr = fp_arr[fp_off + i];
            
            if (num_threads == 1) {
                pthread_create(&tid[0], NULL, read_row, (void *)targs);
                pthread_join(tid[0], NULL);
            } else {
                pthread_create(&tid[tid_off + i], NULL, read_row, (void *)targs);
                rem -= row_cnt;
            }

        }
    }

    if (num_threads > 1) {
        for (int i = 0; i < num_threads; ++i) {
            pthread_join(tid[i], NULL);
        }
    }

    clock_gettime(CLOCK_REALTIME, &stop);
    long long accum = (stop.tv_sec - start.tv_sec) * 1000000000LL + (stop.tv_nsec - start.tv_nsec);
    // printf("Time: %lld\n", accum);

    if (num_threads == 1) {
        FILE *fcsv = fopen(csv, "w+");
        fprintf(fcsv, "%s,%s\n", "#(threads)", "time taken(in ns)");
        fprintf(fcsv, "%d,%lld\n", num_threads, accum);
    } else {
        FILE *fcsv = fopen(csv, "a");
        fprintf(fcsv, "%d,%lld\n", num_threads, accum);
    }
    // Print contents of shared memory
    // for (int r = 0; r < n[0] + n[1]; ++r) {
    //     for (int i = 0; i < q; ++i) {
    //         printf("%d ", *(shmptr + q * r + i));
    //     }
    //     printf("\n");
    // }
}