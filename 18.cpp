#include "stdafx.h"
#include "time.h"
#include "mpi.h"

using namespace std;

#define LENGHT 8

void printMatrix(int *matrix, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << matrix[i*n + j] << " ";
		}
		cout << endl;
	}
}

int main(int argc, char* argv[]) {
	srand(time(0));
	int rank, size;
	MPI_Request request;
	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		int *matrix;

		//Инициализация матрицы в главном процессе
		matrix = (int *)malloc(sizeof(int) * (LENGHT * LENGHT));

		for (int i = 0; i < LENGHT; i++) {
			for (int j = 0; j < LENGHT; j++) {
				if (i == j)
					matrix[i*LENGHT + j] = 0;
				else
					matrix[i*LENGHT + j] = rand() % 10 + 1;
			}
		}

		cout << "first matrix:" << endl;
		printMatrix(matrix, LENGHT);
		cout << endl;

		//Сколько строк матрицы получит каждый процесс
		int workSize = size - 1;
		int *rowCounts = (int*)malloc(sizeof(int) * workSize);

		for (int i = 0; i < workSize; i++) {
			rowCounts[i] = LENGHT / workSize;
		}

		if (LENGHT % workSize != 0) // увеличиваем кол-во строк для последних потоков, при необходимости
		{
			int k = LENGHT - (workSize*rowCounts[0]);
			int n = workSize;
			while (k != 0)
			{
				rowCounts[n]++;
				n--;
				k--;
			}
		}

		//Сколько строк получит каждый процесс
		for (int i = 1; i < size; i++)
			MPI_Send(&rowCounts[i - 1], 1, MPI_INT, i, 0, MPI_COMM_WORLD);

		//Сам алгоритм Флойда
		for (int k = 0; k < LENGHT; k++)
		{
			//раздаем к строку всем процессам кроме основного
			for (int p = 1; p < size; p++)
				MPI_Isend(&matrix[k*LENGHT], LENGHT, MPI_INT, p, 0, MPI_COMM_WORLD, &request);

			//раздаем всем процессам их строки
			int num = 0;
			for (int i = 0; i < workSize; i++)
			{
				for (int j = 0; j < rowCounts[i]; j++)
				{
					//Отдаем нужное кол-во строк
					MPI_Isend(&matrix[num*LENGHT], LENGHT, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &request);
					num++;
				}
			}

			//Получаем измененные строки от процессов
			num = 0;
			for (int i = 0; i < workSize; i++)
			{
				for (int j = 0; j < rowCounts[i]; j++)
				{
					//получаем нужное кол-во строк
					MPI_Recv(&matrix[num*LENGHT], LENGHT, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &status);
					num++;
				}
			}
		}

		cout << endl << "Root process. Print final matrix:" << endl;
		printMatrix(matrix, LENGHT);
	}
	else
	{
		//Получаем кол-во строк, которые будем менять
		int rowCount;
		MPI_Recv(&rowCount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		cout << rank << " : " << rowCount << endl;

		int *kRow = (int *)malloc(sizeof(int) * LENGHT);
		int *rows = (int *)malloc(sizeof(int) * (rowCount * LENGHT));

		//Алгоритм Флойда
		for (int k = 0; k < LENGHT; k++)
		{
			//Получаем к-ю строку
			MPI_Recv(kRow, LENGHT, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			//Получаем свои строки для каждого процесса
			for (int i = 0; i < rowCount; i++)
				MPI_Recv(&rows[i*LENGHT], LENGHT, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			//Меняем нужные строки
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < LENGHT; j++)
					rows[i*LENGHT + j] = min(rows[i*LENGHT + j], rows[i*LENGHT + k] + kRow[j]);
			}

			//Отправляем изменные строки
			for (int i = 0; i < rowCount; i++) {
				MPI_Send(&rows[i*LENGHT], LENGHT, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
		}
	}

	MPI_Finalize();
}