// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <mpi.h>
#include <time.h>

#define POINTS 50000

int main(int argc, char **argv)
{
	srand(time(0));
	int ProcRank, ProcNum;
	double point[2][POINTS];
	double x, y;
	int result = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	if (ProcRank == 0)
	{
		for (int i = 0; i < POINTS; i++)
		{
			x = (double)(rand() % 1000) / 1000;
			y = (double)(rand() % 1000) / 1000;
			point[0][i] = x;
			point[1][i] = y;	
		}
	}
	MPI_Bcast(point, POINTS + POINTS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	int elements_per_proc = POINTS / ProcNum;
	int i1 = ProcRank * elements_per_proc;
	int i2 = (ProcRank + 1) * elements_per_proc;
	if (ProcRank == ProcNum - 1)
		i2 = POINTS;

	int count = 0;

	for (int i = i1; i < i2; i++)
	{
		if ((point[0][i] * point[0][i]) + (point[1][i] * point[1][i]) < 1)
		{
			count++;
		}
	}
	printf("from %d cnt = %d\n", ProcRank, count);

	if (ProcRank != 0)
	{
		MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		int temp;
		result = count;
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			result += temp;
		}
		printf("\nCount point = %d \n", POINTS);
		printf("Count point in circle (result) = %d \n", result);
		printf("Pi = 4/N * result = %f", 4.000 / POINTS * result);
	}

	MPI_Finalize();
}

