#include "stdafx.h"
#include "time.h"
#include "mpi.h"

int main(int argc, char* argv[]) {
	int x[8];
	int y[8];
	int ProcRank, ProcNum, N = 8;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int elements_per_proc = N / ProcNum;
	int *subarr1 = new int[elements_per_proc];
	int *subarr2 = new int[elements_per_proc];
	srand(time(0));
	if (ProcRank == 0) {
		for (int i = 0; i < N; i++)
		{
			x[i] = rand() % 100;
			printf("%d ", x[i]);
		}
		printf("\n");
		for (int i = 0; i < N; i++)
		{
			y[i] = rand() % 100;
			printf("%d ", y[i]);
		}
		printf("\n");
	}
	MPI_Scatter(x, elements_per_proc, MPI_INT,
		subarr1, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(y, elements_per_proc, MPI_INT,
		subarr2, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	int sum = 0;
	for (int i = 0; i < elements_per_proc; i++)
		sum+=subarr1[i] * subarr2[i];

	if (ProcRank != 0)
	{
		MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		int temp = 0;
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += temp;
		}
		printf("result = %d\n", sum);
	}
	MPI_Finalize();
	return 0;
}