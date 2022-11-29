#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

const int BUFF = 10;
const int NUM_ITER = 1;

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

    if (fork() == 0) {
        execlp("./transpose.out", "transpose", in[1], argv[2], argv[3], NULL);
    }
    wait(NULL);

    int *sptr;
    int sid;
    key_t stoken = 1835;
    sid = shmget(stoken, 20, 0666 | IPC_CREAT);
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

    for (int time_quantum = 1000; time_quantum <= 2000; time_quantum += 1000) {
        char wt_name[20];
        snprintf(wt_name, sizeof(wt_name), "WT_%d.csv", time_quantum / 1000);
        FILE *wt_fp;
        if (access(wt_name, F_OK) != 0) {
            wt_fp = fopen(wt_name, "w+");
            fprintf(wt_fp, "%s,%s,%s\n", "Size", "P1", "P2");
        } else {
            wt_fp = fopen(wt_name, "a");
        }
        struct timespec wt_start1, wt_stop1;
        struct timespec wt_start2, wt_stop2;
        long long wt_sum1 = 0, wt_sum2 = 0;

        char csv[2][20];
        char ta_fname[2][20];
        for (int iter = 1; iter <= NUM_ITER; ++iter) {
            snprintf(csv[0], sizeof(csv[0]), "P1_%d_%d.csv", time_quantum / 1000, iter);
            snprintf(csv[1], sizeof(csv[1]), "P2_%d_%d.csv", time_quantum / 1000, iter);
            snprintf(ta_fname[0], sizeof(ta_fname[0]), "TA_P1_%d.csv", time_quantum / 1000);
            snprintf(ta_fname[1], sizeof(ta_fname[1]), "TA_P2_%d.csv", time_quantum / 1000);

            for (int nt1 = 1; nt1 <= n[0] + n[1]; ++nt1) {
                char *nt1_str = malloc(BUFF);
                snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);
                
                for (int nt2 = 1; nt2 <= n[0] * n[1]; ++nt2) {
                    char *nt2_str = malloc(BUFF);
                    snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);

                    pid_t p1, p2;
                    if ((p1 = fork()) == 0) {
                        execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv[0], nt1_str, nt2_str, ta_fname[0], NULL);
                    }
                    if ((p2 = fork()) == 0) {
                        execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv[1], nt2_str, nt1_str, ta_fname[1], NULL);
                    }

                    sptr[0] = 1;
                    sptr[1] = 1;
                    while (waitpid(p1, NULL, WNOHANG) == 0 && waitpid(p2, NULL, WNOHANG) == 0)
                    {
                        if (waitpid(p1, NULL, WNOHANG) == 0)
                        {
                            clock_gettime(CLOCK_REALTIME, &wt_start1);
                            // printf("Switching to p2\n");
                            kill(p1, SIGSTOP);
                            kill(p2, SIGCONT);
                            usleep(time_quantum);
                            clock_gettime(CLOCK_REALTIME, &wt_stop1);
                            wt_sum1 += (wt_stop1.tv_sec - wt_start1.tv_sec) * 1000000000LL + (wt_stop1.tv_nsec - wt_start1.tv_nsec);
                        }
                        if (waitpid(p2, NULL, WNOHANG) == 0)
                        {
                            clock_gettime(CLOCK_REALTIME, &wt_start2);
                            // printf("Switching to p1\n");
                            kill(p2, SIGSTOP);
                            kill(p1, SIGCONT);
                            usleep(time_quantum);
                            clock_gettime(CLOCK_REALTIME, &wt_stop2);
                            wt_sum2 += (wt_stop2.tv_sec - wt_start2.tv_sec) * 1000000000LL + (wt_stop2.tv_nsec - wt_start2.tv_nsec);
                        }
                    }
                    if (waitpid(p1, NULL, WNOHANG) == 0)
                    {
                        // printf("Continuing with p1\n");
                        kill(p1, SIGCONT);
                    }
                    if (waitpid(p2, NULL, WNOHANG) == 0)
                    {
                        // printf("Continuing with p2\n");
                        kill(p2, SIGCONT);
                    }

                    printf("%d, %d before wait.\n", nt1, nt2);
                    
                    // waitpid(p1, NULL, 0);
                    // waitpid(p2, NULL, 0);
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
        wt_sum1 /= (NUM_ITER) * (n[0] + n[1]) * (n[0] * n[1]);
        wt_sum2 /= (NUM_ITER) * (n[0] + n[1]) * (n[0] * n[1]);
        fprintf(wt_fp, "%d,%lld,%lld\n", q, wt_sum1, wt_sum2);
    }
}