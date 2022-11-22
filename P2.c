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

int n[2];
int q;
typedef struct msgbuf {
    long type;
    int matrix_num, row_idx;
} row_info;

int main(int argc, char **argv) {
    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);

    key_t msgtoken = ftok("/", 110);
    int msgqid = msgget(msgtoken, 0644 | IPC_CREAT);

    for (int i = 0; i < n[0] + n[1]; ++i) {
        row_info info;
        msgrcv(msgqid, (void *)&info, MSGSIZ, MSGTYPE, MSGFLG);
        printf("Matrix #: %d, Row #: %d\n", info.matrix_num, info.row_idx);
    }
    
    msgctl(msgqid, IPC_RMID, NULL); 
}