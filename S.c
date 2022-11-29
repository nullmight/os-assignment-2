#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

const int BUFF = 10;
const int NUM_ITER = 3;

int n[2], q;
char *in[2];
char *out;

int main(int argc, char **argv) {
    n[0] = atoi(argv[1]);
    q = atoi(argv[2]);
    n[1] = atoi(argv[3]);
    in[0] = argv[4];
    in[1] = argv[5];
    out = argv[6];

    int *sptr;
    int sid;
    key_t stoken = 1832;
    sid = shmget(stoken, 8, 0666 | IPC_CREAT);
    if (sid == -1)
    {
        perror("Sched : Shared memory id");
        return 1;
    }
    sptr = shmat(sid, 0, 0);
    if (sptr == (void *)-1)
    {
        perror("Sched : Shared memory pointer");
        return 1;
    }
    int time_quantum = 1000;

    for (int time_quantum = 1000; time_quantum <= 2000; time_quantum += 1000) {
        char csv[2][20];
        for (int iter = 1; iter <= NUM_ITER; ++iter) {
            snprintf(csv[0], sizeof(csv[0]), "P1_%d_%d.csv", time_quantum / 1000, iter);
            snprintf(csv[1], sizeof(csv[1]), "P2_%d_%d.csv", time_quantum / 1000, iter);

            for (int nt1 = 1; nt1 <= n[0] + n[1]; ++nt1) {
                char *nt1_str = malloc(BUFF);
                snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);
                for (int nt2 = 1; nt2 <= n[0] * n[1]; ++nt2) {
                    char *nt2_str = malloc(BUFF);
                    snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);
                    
                    pid_t p1, p2;
                    if ((p1 = fork()) == 0) {
                        execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv[0], nt1_str, nt2_str, NULL);
                    }
                    if ((p2 = fork()) == 0) {
                        execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv[1], nt2_str, nt1_str, NULL);
                    }

                    sptr[0] = 1;
                    sptr[1] = 1;
                    while (sptr[0] && sptr[1])
                    {
                        if (sptr[0])
                        {
                            // printf("Switching to p2\n");
                            kill(p1, SIGSTOP);
                            kill(p2, SIGCONT);
                            usleep(time_quantum);
                        }
                        if (sptr[1])
                        {
                            // printf("Switching to p1\n");
                            kill(p2, SIGSTOP);
                            kill(p1, SIGCONT);
                            usleep(time_quantum);
                        }
                    }
                    if (sptr[0])
                    {
                        // printf("Continuing with p1\n");
                        kill(p1, SIGCONT);
                    }
                    if (sptr[1])
                    {
                        // printf("Continuing with p2\n");
                        kill(p2, SIGCONT);
                    }
                    wait(NULL);
                    wait(NULL);

                    printf("%d, %d completed.\n", nt1, nt2);
                    if (nt2 >= 30)
                        nt2 += 9;
                    if (nt2 >= 100)
                        nt2 += 90;
                    if (nt2 >= 1000)
                        nt2 += 900;
                }

                if (nt1 >= 30)
                    nt1 += 9;
                if (nt1 >= 100)
                    nt1 += 90;
                if (nt1 >= 1000)
                    nt1 += 900;
            }
            printf("Iteration %d completed\n", iter);
        }
    }
}