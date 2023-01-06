#define N 250
//mpiexec -np N

#include <cstdlib>
#include <mpi.h>
#include <time.h>
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS
using namespace std;

int main(int argc, char* argv[])
{
    int i, j;
    int size,rank;

    int A[N][N], B[N][N];
    int C[N][N];
    int CC[N], AA[N];

    double startTime, thisTime;
    double totalTime;

    //MPI Init
    MPI_Init(&argc, &argv);
    startTime = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand((unsigned)time(NULL) + rank);

    //Create A, B
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = rand() % 11;
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            B[i][j] = rand() % 11;
        }
    }

    //Scatter rows A to all processes and broadcast B
    MPI_Scatter(A, N * N / size, MPI_INT, AA, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //Multiplication area
    for (i = 0; i < N; i++)
    {
        CC[i] = 0;
        for (j = 0; j < N; j++)
            CC[i] = CC[i] + AA[j] * B[j][i];
    }

    //Gather CC results and send to C
    MPI_Gather(CC, N * N / size, MPI_INT, C, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //Count time
    thisTime = MPI_Wtime() - startTime;
    MPI_Reduce(&thisTime, &totalTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    //Results
    if (rank == 0) {
        cout << "Total time: " << totalTime << " sec";
    }
    MPI_Finalize();
    return 0;
}