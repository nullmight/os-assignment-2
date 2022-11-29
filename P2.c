#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <assert.h>
#include <sys/sem.h>
#include <semaphore.h>

#define N 50

int n[2];
int q;
int *shmptr;
int msgqid;
const int BUFF = 128 * 1024;
const long int MSGTYPE = 0;
const int MSGSIZ = 8;
const int MSGFLG = 0;

long long res[N][N];
// sem_t sem;
int available;

typedef struct msgbuf {
    long type;
    int matrix_num, row_idx;
} row_info;

typedef struct thr_args
{
    int i, j;
} thread_args;

void *mult_rows(void *args) {
    thread_args *targs = args;
    int i = targs->i;
    int j = targs->j;
    long long sum = 0;
    int offset1 = i * q, offset2 = (n[0] + j) * q;
    for (int k = 0; k < q; ++k) {
        sum += (*(shmptr + offset1 + k)) * 1LL * (*(shmptr + offset2 + k));
    }
    res[i][j] = sum;
    // sem_post(&sem);
    available++;
}

int main(int argc, char **argv) {
    struct timespec ta_start, ta_stop;
    clock_gettime(CLOCK_REALTIME, &ta_start);

    if (argc != 9) {
        printf("Invalid arguments!, argc = %d\n", argc);
        return 0;
    }
    // for (int i = 1; i <= 7; ++i)
    // {
    //     printf("P2: args[%d]: %s\n", i, argv[i]);
    // }

    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);
    char *out = argv[4];
    char *csv = argv[5];
    int num_threads = atoi(argv[6]);
    int num_threads_o = atoi(argv[7]);
    char *ta_fname = argv[8];

    // sem_init(&sem, 0, num_threads);
    available = num_threads;

    key_t shmtoken = 1414;
    if (shmtoken == -1)
    {
        perror("P2: Key");
        return 1;
    }
    int shmid = shmget(shmtoken, BUFF, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("P2: Shared memory id");
        return 1;
    }
    shmptr = shmat(shmid, 0, 0);
    if (shmptr == (void *)-1)
    {
        perror("P2: Shared memory pointer");
        return 1;
    }

    key_t msgtoken = 4545;
    int msgqid = msgget(msgtoken, 0644 | IPC_CREAT);

    key_t runtoken = 1835;
    int runid = shmget(runtoken, 20, 0666);
    if (runid == -1)
    {
        perror("P2 : Shared memory id");
        return 1;
    }
    int *sptr = shmat(runid, 0, 0);
    if (sptr == (void *)-1)
    {
        perror("P2 : Shared memory pointer");
        return 1;
    }


    int rcnt[2] = {0, 0};
    int rcvq[2][N];
    int tcnt = 0;
    pthread_t tid[n[0] * n[1]];

    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    
    for (int i = 0; i < n[0] + n[1]; ++i) {
        row_info info;
        msgrcv(msgqid, (void *)&info, MSGSIZ, MSGTYPE, MSGFLG);
        // printf("Matrix #: %d, Row #: %d\n", info.matrix_num, info.row_idx);
        if (info.matrix_num == 0) {
            rcvq[0][rcnt[0]] = info.row_idx;
            for (int r = 0; r < rcnt[1]; ++r) {
                // sem_wait(&sem);
                while (!available);
                available--;
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
                while (!available);
                available--;
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
    
    FILE *fp = fopen(out, "w+");
    // Print output matrix to out
    for (int i = 0; i < n[0]; ++i) {
        for (int j = 0; j < n[1]; ++j) {
            fprintf(fp, "%lld", res[i][j]);
            if (j + 1 < n[1]) fprintf(fp, " ");
            else fprintf(fp, "\n");
        }
    }
    fclose(fp);

    if (num_threads == 1 && num_threads_o == 1)
    {
        FILE *fcsv = fopen(csv, "w+");
        fprintf(fcsv, "%s,%s,%s\n", "P2", "P1", "Time");
        fprintf(fcsv, "%d,%d,%lld\n", num_threads, num_threads_o, accum);
    }
    else
    {
        FILE *fcsv = fopen(csv, "a");
        fprintf(fcsv, "%d,%d,%lld\n", num_threads, num_threads_o, accum);
    }
    // sem_destroy(&sem);
    msgctl(msgqid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);


    clock_gettime(CLOCK_REALTIME, &ta_stop);
    long long ta_time = (ta_stop.tv_sec - ta_start.tv_sec) * 1000000000LL + (ta_stop.tv_nsec - ta_start.tv_nsec);
    FILE* ta_fp;
    if (access(ta_fname, F_OK) != 0) {
        ta_fp = fopen(ta_fname, "w+");
        fprintf(ta_fp, "%s,%s\n", "Size", "Time");
        fprintf(ta_fp, "%d,%lld\n", q, ta_time);
    } else {
        ta_fp = fopen(ta_fname, "a");
        fprintf(ta_fp, "%d,%lld\n", q, ta_time);
    }
    // printf("P2 completed for %d, %d.\n", num_threads_o, num_threads);

    sptr[1] = 0;
}