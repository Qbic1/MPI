#include "stdafx.h"
#include "time.h"


int main(int argc, char* argv[]) {
	int x[8];
	int ProcRank, ProcNum, N = 8;
	int resultSum;
	int resultCount;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	int elements_per_proc = N / ProcNum;
	int *subarr = new int[elements_per_proc];

	int sum;
	int count;
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

	sum = 0;
	count = 0;
	for (int i = 0; i < elements_per_proc; i++)
		if (subarr[i] > 0)
		{
			sum += subarr[i];
			count++;
		}

	MPI_Reduce(&sum, &resultSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&count, &resultCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (ProcRank == 0)
	printf("\navg of positive = %f\n", (double)resultSum/resultCount);
	MPI_Finalize();
	return 0;
}
