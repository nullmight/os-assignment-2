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

    char csv[2][10];
    for (int cnt = 1; cnt <= 5; ++cnt) {
        snprintf(csv[0], sizeof(csv[0]), "P1_%d.csv", cnt);
        snprintf(csv[1], sizeof(csv[1]), "P2_%d.csv", cnt);

        for (int nt1 = 1; nt1 <= n[0] + n[1]; ++nt1) {
            char *nt1_str = malloc(BUFF);
            snprintf(nt1_str, sizeof(nt1_str), "%d", nt1);
            for (int nt2 = 1; nt2 <= n[0] * n[1]; ++nt2) {
                char *nt2_str = malloc(BUFF);
                snprintf(nt2_str, sizeof(nt2_str), "%d", nt2);

                if (fork() == 0) {
                    execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], csv[0], nt1_str, nt2_str, NULL);
                }
                if (fork() == 0) {
                    execlp("./P2.out", "P2", argv[1], argv[2], argv[3], out, csv[1], nt2_str, nt1_str, NULL);
                }
                wait(NULL);
                wait(NULL);

                if (nt2 >= 25)
                    nt2 += 9;
                if (nt2 >= 100)
                    nt2 += 90;
                if (nt2 >= 1000)
                    nt2 += 900;
            }

            if (nt1 >= 25)
                nt1 += 9;
            if (nt1 >= 100)
                nt1 += 90;
            if (nt1 >= 1000)
                nt1 += 900;
        }
        printf("Iteration %d completed\n", cnt);
    }
}