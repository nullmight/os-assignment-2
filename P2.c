#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <string.h>

#define N 1005

int n[2];
int q;
int *shmptr;
int msgqid;
const int BUFF = 1e4;
const long int MSGTYPE = 0;
const int MSGSIZ = 8;
const int MSGFLG = 0;

int res[N][N];
sem_t sem;

typedef struct msgbuf {
    long type;
    int matrix_num, row_idx;
} row_info;

typedef struct thr_args {
    int i, j;
} thread_args;


void *mult_rows(void *args) {
    thread_args *targs = args;
    int i = targs->i;
    int j = targs->j;
    int sum = 0;
    int offset1 = i * q, offset2 = (n[0] + j) * q;
    for (int k = 0; k < q; ++k) {
        sum += (*(shmptr + offset1 + k)) * (*(shmptr + offset2 + k));
    }
    res[i][j] = sum;
    // sem_post(&sem);
}

int main(int argc, char **argv) {
    if (argc != 9) {
        printf("Invalid arguments!, argc = %d\n", argc);
        return 0;
    }

    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);
    char *out = argv[4];
    char *csv = argv[5];
    int num_threads = atoi(argv[6]);
    int num_threads_o = atoi(argv[7]);
    char *save = argv[8];
    
    // printf("%d\n", num_threads);
    // sem_init(&sem, 0, num_threads);
    // printf("Start nt1: %d, nt2: %d\n", num_threads, num_threads_o);

    key_t shmtoken = ftok("/", 10);
    if (shmtoken == -1) {
        perror("P2: Key");
        return 1;
    }
    int shmid = shmget(shmtoken, BUFF, 0666 | IPC_CREAT);
    if (shmid == -1) {
        if (errno == EEXIST) {
            shmid = shmget(shmtoken, BUFF, 0);
        } else {
            perror("P2: Shared memory id");
            return 1;
        }
    }
    shmptr = shmat(shmid, 0, 0);
    if (shmptr == (void *)-1) {
        perror("P2: Shared memory pointer");
        return 1;
    }

    key_t msgtoken = ftok("/", 110);
    int msgqid = msgget(msgtoken, 0644 | IPC_CREAT);

    int rcnt[2] = {0, 0};
    int rcvq[2][N];
    int tcnt = 0;

    pthread_t tid[n[0] * n[1]];

    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < n[0] + n[1]; ++i) {
        row_info info;
        msgrcv(msgqid, (void *)&info, MSGSIZ, MSGTYPE, MSGFLG);
        // printf("P2 recv: Matrix #: %d, Row #: %d\n", info.matrix_num, info.row_idx);
        if (info.matrix_num == 0) {
            rcvq[0][rcnt[0]] = info.row_idx;
            for (int r = 0; r < rcnt[1]; ++r) {
                // sem_wait(&sem);
                thread_args *targs = malloc(sizeof(thread_args));
                targs->i = rcvq[0][rcnt[0]];
                targs->j = rcvq[1][r];
                pthread_create(&tid[tcnt], NULL, mult_rows, (void *)targs);
                tcnt++;
            }
            rcnt[0]++;
        } else {
            rcvq[1][rcnt[1]] = info.row_idx;
            for (int r = 0; r < rcnt[0]; ++r) {
                // sem_wait(&sem);
                thread_args *targs = malloc(sizeof(thread_args));
                targs->i = rcvq[0][r];
                targs->j = rcvq[1][rcnt[1]];
                pthread_create(&tid[tcnt], NULL, mult_rows, (void *)targs);
                tcnt++;
            }
            rcnt[1]++;
        }
    }
    
    for (int i = 0; i < n[0] * n[1]; ++i) {
        pthread_join(tid[i], NULL);
    }
    
    clock_gettime(CLOCK_REALTIME, &stop);
    long long accum = (stop.tv_sec - start.tv_sec) * 1000000000LL + (stop.tv_nsec - start.tv_nsec);

    printf("Outside nt1: %d, nt2: %d\n", num_threads, num_threads_o);
    if (strcmp(save, "True") == 0) {
        if (num_threads == 1 && num_threads_o == 1) {
            // printf("Inside");
            FILE *fcsv = fopen(csv, "w+");
            fprintf(fcsv, "%s,%s,%s\n", "#(threads) in P2", "#(threads) in P1", "time taken(in ns)");
            fprintf(fcsv, "%d,%d,%lld\n", num_threads, num_threads_o, accum);
            fclose(fcsv);
        } else {
            FILE *fcsv = fopen(csv, "a");
            fprintf(fcsv, "%d,%d,%lld\n", num_threads, num_threads_o, accum);
            fclose(fcsv);
        }
    }
    // Print output matrix
    for (int i = 0; i < n[0]; ++i)
    {
        for (int j = 0; j < n[1]; ++j)
        {
            printf("%d", res[i][j]);

            if (j + 1 < n[1])
                printf(" ");
            else
                printf("\n");
        }
    }
    // sem_destroy(&sem);
    msgctl(msgqid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
}