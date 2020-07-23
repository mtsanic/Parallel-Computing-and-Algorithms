#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <mpi.h>

void alloc_matrix(void ***a, int m, int n, int size) {
    int i;
    void *storage;
    storage = (void *) malloc(m * n * size);
    *a = (void **) malloc(m * sizeof(void *));
    for (i = 0; i < m; i++) {
        (*a)[i] = storage + i * n * size;
    }
}

void print_root(int **root, int low, int high) {
    printf("Root spanning %d-%d is %d\n", low, high, root[low][high + 1]);
    if (low < root[low][high + 1] - 1) {
        print_root(root, low, root[low][high + 1] - 1);
    }
    if (root[low][high + 1] < high - 1) {
        print_root(root, root[low][high + 1] + 1, high);
    }
}

int main(int argc, char *argv[]) {
    int low, high, i, j, d, n = 10;
    int bestroot,**root = NULL;
    float bestcost,**cost = NULL;
    float rcost;

    int id, np, mpi_root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    n = 8;
    float p[8] = {0.16, 0.13, 0.06, 0.08, 0.07, 0.17, 0.05, 0.28};
    
    // p = (float *) malloc(n * sizeof(float));
    // if (id == mpi_root) { for (i = 0; i < n; i++) { scanf("%f", &p[i]); }}
    // for (i = 0; i < n; i++) { MPI_Bcast(&p[i], 1, MPI_FLOAT, mpi_root, MPI_COMM_WORLD); }

    alloc_matrix((void ***) &cost, n + 1, n + 1, sizeof(float));
    alloc_matrix((void ***) &root, n + 1, n + 1, sizeof(int));

    for (d = 0; d <= n + 1; d++) {
        for (low = id; low + d <= n; low += np) {
            high = low + d;
            if (low == high) {
                cost[low][low] = 0.0;
                root[low][low] = low;
            } else {
                bestcost = FLT_MAX;
                bestroot = -1;
                for (i = low; i < high; i++) {
                    rcost = cost[low][i] + cost[i + 1][high];
                    for (j = low; j < high; j++) { rcost += p[j]; }
                    if (rcost < bestcost) {
                        bestcost = rcost;
                        bestroot = i;
                    }
                }
                cost[low][high] = bestcost;
                root[low][high] = bestroot;
            }
        }
        for (low = 0; low + d <= n; low++) {
            high = low + d;

            MPI_Bcast(&cost[low][high], 1, MPI_FLOAT, low % np, MPI_COMM_WORLD);
            MPI_Bcast(&root[low][high], 1, MPI_INT, low % np, MPI_COMM_WORLD);
        }
    }

    if (id == mpi_root) { print_root(root, 0, n - 1); }

    MPI_Finalize();
    return 0;
}
