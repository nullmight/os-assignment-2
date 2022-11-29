#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

const int BUFF = 50;

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
    int tq = atoi(argv[7]);

    char csv[2][10];
    for (int cnt = 1; cnt <= 5; ++cnt) {
        snprintf(csv[0], sizeof(csv[0]), "P1_%d.csv", cnt);
        snprintf(csv[1], sizeof(csv[1]), "P2_%d.csv", cnt);

        for (int nt1 = 1; nt1 <= 1; ++nt1) {
            char *nt1_str = malloc(BUFF);
            snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);
            for (int nt2 = 1; nt2 <= 1; ++nt2) {
                char *nt2_str = malloc(BUFF);
                snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);

                pid_t p1 = 1, p2 = 1;

                if ((p1 = fork()) == 0) {
                    execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv[0], nt1_str, nt2_str, NULL);
                }
                if ((p2 = fork()) == 0) {
                    execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv[1], nt2_str, nt1_str, NULL);
                }
                int p11 = p1, p22 = p2;
                while(p1 == p11 && p2 == p22)
                {
                    printf("Running!, %d %d\n", p1, p2);
                    if(p1 != 0)
                    {
                        printf("\nSwitching to P1\n");
                        kill(p2, SIGTSTP);
                        kill(p1, SIGCONT);
                        usleep(tq);
                    }

                    if(p2 != 0)
                    {
                        printf("\nSwitching to P2\n");
                        kill(p1, SIGTSTP);
                        kill(p2, SIGCONT);
                        usleep(tq);
                    }
                    if(p1 == p11)
                        p1 = waitpid(p11, NULL, WNOHANG);
                    if(p2 == p22)
                        p2 = waitpid(p22, NULL, WNOHANG);
                }

                wait(NULL);
                wait(NULL);
            }
        }
        printf("Iteration %d completed\n", cnt);
    }
}