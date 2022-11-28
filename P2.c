#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <assert.h>

#define N 50

int n[2];
int q;
int *shmptr;
int msgqid;
const int BUFF = 512;
const long int MSGTYPE = 0;
const int MSGSIZ = 8;
const int MSGFLG = 0;

int res[N][N];

typedef struct msgbuf {
    long type;
    int matrix_num, row_idx;
} row_info;

void mult_rows(int i, int j) {
    int sum = 0;
    int offset1 = i * q, offset2 = (n[0] + j) * q;
    for (int k = 0; k < q; ++k)
    {
        sum += (*(shmptr + offset1 + k)) * (*(shmptr + offset2 + k));
    }
    res[i][j] = sum;
}

int main(int argc, char **argv) {
    if (argc != 8) {
        printf("Invalid arguments!, argc = %d\n", argc);
        return 0;
    }
    for (int i = 1; i <= 7; ++i)
    {
        printf("P2: args[%d]: %s\n", i, argv[i]);
    }

    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);
    char *out = argv[4];
    char *csv = argv[5];
    int num_threads = atoi(argv[6]);
    int num_threads_o = atoi(argv[7]);

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

    int rcnt[2] = {0, 0};
    int rcvq[2][N];
    int tcnt = 0;

    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    
    for (int i = 0; i < n[0] + n[1]; ++i) {
        row_info info;
        msgrcv(msgqid, (void *)&info, MSGSIZ, MSGTYPE, MSGFLG);
        // printf("Matrix #: %d, Row #: %d\n", info.matrix_num, info.row_idx);
        if (info.matrix_num == 0) {
            rcvq[0][rcnt[0]] = info.row_idx;
            for (int r = 0; r < rcnt[1]; ++r) {
                mult_rows(rcvq[0][rcnt[0]], rcvq[1][r]);
                tcnt++;
            }
            rcnt[0]++;
        } else {
            rcvq[1][rcnt[1]] = info.row_idx;
            for (int r = 0; r < rcnt[0]; ++r)
            {
                mult_rows(rcvq[0][r], rcvq[1][rcnt[1]]);
                tcnt++;
            }
            rcnt[1]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &stop);
    long long accum = (stop.tv_sec - start.tv_sec) * 1000000000LL + (stop.tv_nsec - start.tv_nsec);
    // Print contents of shared memory
    for (int i = 0; i < n[0]; ++i) {
        for (int j = 0; j < n[1]; ++j) {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }
    msgctl(msgqid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
}