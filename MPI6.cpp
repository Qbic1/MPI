#include "stdafx.h"
#include "time.h"

/*int main(int argc, char* argv[]) {
	int x[8][8];
	int ProcRank, ProcNum, N = 8;
	int resultMin;
	int resultMax;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int elements_per_proc = N * N / ProcNum;
	int *subarr = new int[elements_per_proc];

	int localmax;
	int localmin;
	srand(time(0));
	if (ProcRank == 0) {
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++) {
				x[i][j] = rand() % 100;
				printf(" %d ", x[i][j]);
			}
			printf("\n");
		}
	}
	MPI_Scatter(x, elements_per_proc, MPI_INT,
		subarr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	localmax = 0;
	for (int i = 0; i < elements_per_proc; i++)
		if (subarr[i] > localmax) localmax = subarr[i];
	localmin = 100;
	for (int i = 0; i < elements_per_proc; i++)
		if (subarr[i] < localmin) localmin = subarr[i];

	MPI_Reduce(&localmin, &resultMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Reduce(&localmax, &resultMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	if (ProcRank == 0)
		printf("\nmin = %d\nmax = %d", resultMin, resultMax);
	MPI_Finalize();
	return 0;
}*/