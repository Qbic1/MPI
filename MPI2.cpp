#include "stdafx.h"
#include "time.h"


/*int main(int argc, char* argv[]) {
	int x[8];
	int ProcRank, ProcNum, N = 8;
	int result;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	int elements_per_proc = N / ProcNum;
	int *subarr = new int[elements_per_proc];

	int localmax;
	srand(time(0));
	if (ProcRank == 0) {
	for (int i = 0; i < N; i++)
	{
		x[i] = rand() % 100;
		printf("%d ", x[i]);
	}
	}
	MPI_Scatter(x, elements_per_proc, MPI_INT,
		subarr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	localmax = 0;
	for (int i = 0; i < elements_per_proc; i++)
		if (subarr[i] > localmax) localmax = subarr[i];

	MPI_Reduce(&localmax, &result, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	if (ProcRank == 0)
		printf("\nmax = %d\n", result);
	MPI_Finalize();
	return 0;
}*/
