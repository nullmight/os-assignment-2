#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

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

    char csv[2][10];
    char nt_str[2];
    for (int cnt = 1; cnt <= 5; ++cnt) {
        snprintf(csv[0], sizeof(csv[0]), "P1_%d.csv", cnt);
        snprintf(csv[1], sizeof(csv[1]), "P2_%d.csv", cnt);

        for (int nt1 = 1; nt1 <= n[0] + n[1]; ++nt1) {
            char *nt1_str = malloc(10);
            snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);
            for (int nt2 = 1; nt2 <= n[0] * n[1]; ++nt2) {
                char *nt2_str = malloc(10);
                snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);

                if (fork() == 0) {
                    execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv[0], nt1_str, nt2_str, "True", NULL);
                }
                if (fork() == 0) {
                    execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv[1], nt2_str, nt1_str, "False", NULL);
                }
            }
        }
        printf("Iteration %d completed\n", cnt);
    }

    for (int cnt = 1; cnt <= 5; ++cnt) {
        snprintf(csv[0], sizeof(csv[0]), "P1_%d.csv", cnt);
        snprintf(csv[1], sizeof(csv[1]), "P2_%d.csv", cnt);

        for (int nt2 = 1; nt2 <= n[0] + n[1]; ++nt2) {
            char *nt2_str = malloc(10);
            snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);
            for (int nt1 = 1; nt1 <= n[0] * n[1]; ++nt1) {
                char *nt1_str = malloc(10);
                snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);

                if (fork() == 0) {
                    execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv[0], nt1_str, nt2_str, "False", NULL);
                }
                if (fork() == 0) {
                    execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv[1], nt2_str, nt1_str, "True", NULL);
                }
            }
        }
        printf("Iteration %d completed\n", cnt);
    }
}