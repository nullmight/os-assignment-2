#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <assert.h>

int n[2];
int q;
int *shmptr;
int msgqid;
const int BUFF = 1e3;
const long int MSGTYPE = 0;
const int MSGSIZ = 8;
const int MSGFLG = 0;

typedef struct msgbuf {
    long type;
    int matrix_num, row_idx;
} row_info;

int main(int argc, char **argv) {
    if (argc != 8) {
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

    key_t shmtoken = ftok("/", 5);
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

    key_t msgtoken = ftok("/", 110);
    int msgqid = msgget(msgtoken, 0644 | IPC_CREAT);

    for (int i = 0; i < n[0] + n[1]; ++i) {
        row_info info;
        msgrcv(msgqid, (void *)&info, MSGSIZ, MSGTYPE, MSGFLG);
        printf("Matrix #: %d, Row #: %d\n", info.matrix_num, info.row_idx);
    }
    // Print contents of shared memory
    for (int r = 0; r < n[0] + n[1]; ++r) {
        for (int i = 0; i < q; ++i) {
            printf("%d ", *(shmptr + q * r + i));
        }
        printf("\n");
    }
    msgctl(msgqid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
}