#include "stdafx.h"
#include "time.h"

/*int main(int argc, char* argv[]) {
	int x[8];
	int ProcRank, ProcNum, N = 8;

	// init and stuff
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int elements_per_proc = N / ProcNum;
	int *subarr1 = new int[elements_per_proc];

	srand(time(0));
	if (ProcRank == 0) {
		for (int i = 0; i < N; i++)
		{
			x[i] = rand() % 100;
			printf("%d ", x[i]);
		}
		printf("\n");
	}

	if (ProcRank == 0) {
		MPI_Sendrecv(x, elements_per_proc, MPI_INT, 0, 0, subarr1, elements_per_proc, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int i = elements_per_proc; i < N; i += elements_per_proc) {
			MPI_Send(x + i, elements_per_proc, MPI_INT, i / elements_per_proc, 0, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(subarr1, elements_per_proc, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	for (int i = 0; i < elements_per_proc; i++) {
		printf("%d ", subarr1[i]);
	}

	printf(" : from process %d\n", ProcRank);

	MPI_Finalize();
	return 0;
}*/