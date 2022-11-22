#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>

int q;
typedef struct thr_args {
    int matrix_num;     //0 or 1 corresponding to inp1.txt or inp2.txt
    int row_idx;        //starting row index for the thread
    int read_cnt;       //number of rows to be read
    FILE *ptr;          //should point to row_idx inside file containing matrix_num
} thread_args;

void *read_row(void *args) {
    thread_args *targs = args;
    // printf("Inside thread with tid: %ld\n", pthread_self());
    // printf("Matrix #: %d, Row #: %d\n", targs->matrix_num, targs->row_idx);
    for (int rc = 0; rc < targs->read_cnt; ++rc) {
        int *buff = malloc(q * sizeof(int));
        for (int i = 0; i < q; ++i) {
            fscanf(targs->ptr, "%d", &buff[i]);
            // printf("Tid: %ld, x = %d\n", pthread_self(), buff[i]);
        }
    }
}

int main(int argc, char **argv) {
    int n[2];
    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);
    char *in[2];
    in[0] = argv[4];
    in[1] = argv[5];
    int num_threads = atoi(argv[6]);

    FILE *fp[2];
    for (int i = 0; i < 2; ++i) {
        fp[i] = fopen(in[i], "r");
        assert(fp[i] != NULL);
    }
    
    if (num_threads > n[0] + n[1]) {
        num_threads = n[0] + n[1];
    }

    
    if (num_threads == 1) {
        pthread_t tid;
        for (int mtx_num = 0; mtx_num < 2; ++mtx_num) {
            
            int tot_rows = n[mtx_num];
            int row_cnt = 1;

            FILE *fp_arr[tot_rows];
            fp_arr[0] = fp[mtx_num];
            
            int offset = 0;
            for (int i = 1; i < tot_rows; ++i) {
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

                fp_arr[i] = ptr;
            }

            for (int i = 0; i < tot_rows; ++i) {
                thread_args *targs = malloc(sizeof(thread_args));
                targs->matrix_num = mtx_num;
                targs->row_idx = i;
                targs->read_cnt = row_cnt;
                targs->ptr = fp_arr[i];
                pthread_create(&tid, NULL, read_row, (void *)targs);
                pthread_join(tid, NULL);
            }
        }
    } else {
        pthread_t tid[num_threads];
        int num_th[2];
        // Do we use float here (messy but more accurate)
        num_th[0] = n[0] * num_threads / (n[0] + n[1]);
        if (num_th[0] == 0) {
            num_th[0] = 1;
        } else if (num_th[0] == num_threads) {
            num_th[0] = num_threads - 1;
        }
        num_th[1] = num_threads - num_th[0];
        for (int i = 0; i < 2; ++i) {
            printf("num_th[%d]: %d\n", i, num_th[i]);
        }
        for (int mtx_num = 0; mtx_num < 2; ++mtx_num) {
            int row_cnt = (n[mtx_num] + num_th[mtx_num] - 1) / num_th[mtx_num];
            
            FILE *fp_arr[num_th[mtx_num]];
            fp_arr[0] = fp[mtx_num];
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

                fp_arr[i] = ptr;
            }

            int rem = n[mtx_num];
            int tid_off = mtx_num * num_th[0];

            for (int i = 0; i < num_th[mtx_num]; ++i) {
                thread_args *targs = malloc(sizeof(thread_args));
                
                targs->matrix_num = mtx_num;
                targs->row_idx = i * row_cnt;
                targs->read_cnt = rem < row_cnt ? rem : row_cnt;
                targs->ptr = fp_arr[i];
                
                pthread_create(&tid[tid_off + i], NULL, read_row, (void *)targs);

                rem -= row_cnt;
            }
        }

        for (int i = 0; i < num_threads; ++i) {
            pthread_join(tid[i], NULL);
        }
    }

}