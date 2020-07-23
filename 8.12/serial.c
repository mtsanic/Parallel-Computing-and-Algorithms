#include <stdio.h>
#include <stdlib.h>
#include <values.h>

// Print the root of the subtree spanning keys 
// 'low' through 'high'
void print_root(int **root, int low, int high) {
    printf("Root of tree spanning %d-%d is %d\n", 
        low, high, root[low][high+1]);
    if (low < root[low][high+1] - 1)
        print_root(root, low, root[low][high+1]-1);
    if (root[low][high+1] < high - 1)
        print_root(root, root[low][high+1]+1, high);
}

// Allocate a two-dimensional array with 'm' rows and
// 'n' columns, where each entry occupies 'size' bytes
void alloc_matrix(void ***a, int m, int n, int size) {
    int i;
    void *storage;
    storage = (void *) malloc(m * n * size);
    *a = (void **) malloc(m * sizeof(void *));
    for (i = 0; i < m; i++) {
        (*a)[i] = storage + i * n * size;
    }
}

int main(int argc, char *argv[]){
    float bestcost;         // Lowest cost subtree found so far
    int bestroot;           // Root of lowest cost subtree
    int high;               // Highest key in subtree
    int i, j;
    int low;                // Lowest key in subtree
    int n;                  // Number of keys
    int r;                  // Possible subtree root
    float rcost;            // Cost of subtree rooted by r
    int **root;             // Best subtree roots
    float **cost;           // Best subtree costs
    // float *p;               // Probability of each key
    // void alloc_matrix(void ***, int, int, int);
    // void print_root(int **, int, int);

    // Input the number of keys and probabilities
    // scanf("%d", &n);
    // p = (float *)malloc(n * sizeof(float));
    // for (i = 0; i < n; i++)
    //     scanf("%f", &p[i]);

    n = 8;
    float p[8] = {0.16, 0.13, 0.06, 0.08, 0.07, 0.17, 0.05, 0.28};

    // Find optimal binary search tree
    alloc_matrix((void ***)&cost, n+1, n+1, sizeof(float));
    alloc_matrix((void ***)&root, n+1, n+1, sizeof(int));

    for (low = n; low >= 0; low--) {
        cost[low][low] = 0.0;
        root[low][low] = low;
        for (high = low+1; high <= n; high++) {
            bestcost = MAXFLOAT;
            for (r = low; r < high; r++) {
                rcost = cost[low][r] + cost[r+1][high];
                for (j = low; j < high; j++)
                    rcost += p[j];
                if (rcost < bestcost) {
                    bestcost = rcost;
                    bestroot = r;
                }
            }

            cost[low][high] = bestcost;
            root[low][high] = bestroot;
        }
    }

    // Print structures of binary search tr
    print_root(root, 0, n-1);
    
    return 0;
}