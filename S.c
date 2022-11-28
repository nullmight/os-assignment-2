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

    char cnt_str[20];
    char nt_str[10];
    for (int cnt = 1; cnt <= 10; ++cnt) {
        snprintf(cnt_str, sizeof(cnt_str), "P1_%d.csv", cnt);
        for (int nt = 1; nt <= n[0] + n[1]; ++nt) {
            snprintf(nt_str, sizeof(nt_str), "%d", nt);
            if (fork() == 0) {
                execlp("./P1.out", "P1", argv[1], argv[2], argv[3], in[0], in[1], cnt_str, nt_str, NULL);
            }
            if (fork() == 0) {
                execlp("./P2.out", "P2", argv[1], argv[2], argv[3], in[0], in[1], cnt_str, nt_str, NULL);
            }
            wait(NULL);
        }
        printf("Iteration %d completed\n", cnt);
    }
}