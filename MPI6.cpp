#include "stdafx.h"
#include "time.h"
#include "mpi.h"

#define LENGHT 8

int main(int argc, char* argv[]) {
	int x[8][LENGHT];
	int ProcRank, ProcNum, N = 8;

	int resultMin;
	int resultMax;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int elements_per_proc = N * LENGHT / ProcNum;
	int *subarr = new int[elements_per_proc];

	int localmax, minmax;
	int localmin, maxmin;

	srand(time(0));
	if (ProcRank == 0) {
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < LENGHT; j++) {
				x[i][j] = rand() % 100;
				printf(" %d ", x[i][j]);
			}
			printf("\n");
		}
	}

	MPI_Scatter(x, elements_per_proc, MPI_INT,
		subarr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	maxmin = 0;
	minmax = 100;
	for (int i = 0; i < elements_per_proc / LENGHT; i++)
	{
		localmin = 100;
		localmax = 0;
		for (int j = i*LENGHT; j < (i + 1)*LENGHT; j++)
		{
			if (subarr[j] < localmin) localmin = subarr[j];
			if (subarr[j] > localmax) localmax = subarr[j];
		}
		if (localmin > maxmin)
			maxmin = localmin;
		if (localmax < minmax)
			minmax = localmax;
	}

	MPI_Reduce(&minmax, &resultMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Reduce(&maxmin, &resultMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	if (ProcRank == 0)
		printf("\nminmax = %d\nmaxmin = %d", resultMin, resultMax);

	MPI_Finalize();
	return 0;
}