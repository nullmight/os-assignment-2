#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *fp = fopen(argv[1], "r");
    int n = atoi(argv[2]);
    int m = atoi(argv[3]);

    int mat[n][m];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            fscanf(fp, "%d", &mat[i][j]);
        }
    }
    fclose(fp);
    fp = fopen(argv[1], "w");
    for (int j = 0; j < m; ++j) {
        for (int i = 0; i < n; ++i) {
            fprintf(fp, "%d", mat[i][j]);
            if (i + 1 < n) fprintf(fp, " ");
            else fprintf(fp, "\n");
        }
    }
    fclose(fp);
}