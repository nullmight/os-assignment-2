#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

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

    for (int cnt = 1; cnt <= 5; ++cnt) {
        char *csv1 = malloc(BUFF);
        char *csv2 = malloc(BUFF);
        snprintf(csv1, sizeof(csv1), "P1_(%d).csv", cnt);
        snprintf(csv2, sizeof(csv2), "P2_(%d).csv", cnt);

        for (int nt1 = 1; nt1 <= 1; ++nt1) {
            char *nt1_str = malloc(BUFF);
            snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);
            for (int nt2 = 1; nt2 <= 1; ++nt2) {
                char *nt2_str = malloc(BUFF);
                snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);

                if (fork() == 0) {
                    execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv1, nt1_str, nt2_str, NULL);
                }
                if (fork() == 0) {
                    execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv2, nt2_str, nt1_str, NULL);
                }
                wait(NULL);
                wait(NULL);
            }
        }
        printf("Iteration %d completed\n", cnt);
    }
}