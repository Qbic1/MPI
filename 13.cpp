#include "stdafx.h"
#include "time.h"
#include "mpi.h"

using namespace std;

#define LENGHT 8

int compare(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}

int* get_part_of_array(int* arr, int start_pos, int end_pos)
{
	int* part_arr = new int[end_pos - start_pos];
	int j = 0;
	for (int i = start_pos; i < end_pos; i++)
	{
		part_arr[j] = arr[i];
		j++;
	}
	return part_arr;
}

int* merge_arrays(int* arr1, int* arr2, int block_size)
{
	int* merged_arr = new int[block_size * 2];
	for (int i = 0; i < block_size; i++)
	{
		merged_arr[i] = arr1[i];
		merged_arr[i + block_size] = arr2[i];
	}
	return merged_arr;
}

int* merge_arrays(int* arr1, int* arr2, int arr1_block_size, int arr2_block_size)
{
	int* merged_arr = new int[arr1_block_size + arr2_block_size];
	int j = 0;
	for (int i = 0; i < arr1_block_size; i++)
	{
		merged_arr[j] = arr1[i];
		j++;
	}
	for (int i = 0; i < arr2_block_size; i++)
	{
		merged_arr[j] = arr2[i];
		j++;
	}
	return merged_arr;
}

int* get_randomized_array(int size)
{
	int* array = new int[size];
	int range = size;
	for (int i = 0; i < size; i++)
	{
		array[i] = rand() % range + 1;
	}
	return array;
}

int main(int argc, char* argv[]) {
	srand(time(0));

	int* array = new int[LENGHT];
	array = get_randomized_array(LENGHT);

	int nodes_count;
	int current_node;

	int* recv_array;
	int* tmp_recv_array;
	int** send_array;
	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &nodes_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &current_node);

	if (nodes_count > LENGHT)
		nodes_count = LENGHT - 1;

	const int LAST_NODE = nodes_count - 1;
	const int ROOT_NODE = 0;

	if (current_node == ROOT_NODE)
	{
		for (int i = 0; i < LENGHT; i++)
			printf("%d ", array[i]);
		cout << endl;
	}

	int block_size = LENGHT / nodes_count;
	bool diff_slices;
	if (LENGHT % nodes_count != 0)
		diff_slices = true;
	else
		diff_slices = false;

	if (current_node != ROOT_NODE && current_node != LAST_NODE) // Для не первого и не последнего процесса получаем массив
	{
		recv_array = new int[block_size];
		MPI_Recv(recv_array, block_size, MPI_INT, ROOT_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}
	else if (current_node == LAST_NODE && current_node != ROOT_NODE) // для последнего процесса для равномерного и неравномерного разделения
	{
		if (diff_slices)
		{
			recv_array = new int[block_size + LENGHT % nodes_count];
			MPI_Recv(recv_array, block_size + LENGHT % nodes_count, MPI_INT, ROOT_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
		else
		{
			recv_array = new int[block_size];
			MPI_Recv(recv_array, block_size, MPI_INT, ROOT_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
	}
	else // Из первого процесса отправляем в остальные
	{
		send_array = new int*[nodes_count]; // массив для каждого процесса
		for (int i = ROOT_NODE; i < LAST_NODE; i++)
			send_array[i] = new int[block_size];

		if (diff_slices)
			send_array[LAST_NODE] = new int[block_size + LENGHT % nodes_count];
		else
			send_array[LAST_NODE] = new int[block_size];

		for (int i = ROOT_NODE; i < LAST_NODE; i++)
			for (int j = 0; j < block_size; j++)
				send_array[i][j] = array[i * block_size + j];

		if (diff_slices)
			for (int j = 0; j < block_size + LENGHT % nodes_count; j++)
				send_array[LAST_NODE][j] = array[block_size*(LAST_NODE)+j];
		else
			for (int j = 0; j < block_size; j++)
				send_array[LAST_NODE][j] = array[(LAST_NODE)*block_size + j];

		if (nodes_count != 1)
		{
			for (int i = 1; i < LAST_NODE; i++)
			{
				MPI_Send(send_array[i], block_size, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			if (diff_slices)
				MPI_Send(send_array[LAST_NODE], block_size + LENGHT % nodes_count, MPI_INT, LAST_NODE, 0, MPI_COMM_WORLD);
			else
				MPI_Send(send_array[LAST_NODE], block_size, MPI_INT, LAST_NODE, 0, MPI_COMM_WORLD);
		}

		recv_array = new int[block_size];
		recv_array = send_array[0];

	}

	MPI_Barrier(MPI_COMM_WORLD); //ждем пока все потоки не получат свои доли

	for (int j = 0; j < nodes_count; j++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if (j == current_node)
			if (diff_slices && current_node == LAST_NODE)
			{
				printf("node[%d] count %d\n", current_node, block_size + LENGHT % nodes_count);
				for (int i = 0; i < block_size + LENGHT % nodes_count; i++)
					printf("node[%d] array[%d] is %d \n", current_node, i, recv_array[i]);
			}
			else
			{
				for (int i = 0; i < block_size; i++)
					printf("node[%d] array[%d] is %d \n", current_node, i, recv_array[i]);
			}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (nodes_count == 1)
		qsort(recv_array, LENGHT, sizeof(int), compare);

	//сам алгоритм 
	for (int n = 0; n < nodes_count; n++)
	{
		/* Четная итерация */
		if (n % 2 == 0)
		{
			for (int j = 0; j < LAST_NODE; j += 2)
			{
				if (current_node == j)
				{
					/* отправляем свой массив в следующий процесс */
					MPI_Send(recv_array, block_size, MPI_INT, j + 1, 0, MPI_COMM_WORLD);

					if (diff_slices && j + 1 == LAST_NODE)
					{
						tmp_recv_array = new int[block_size + LENGHT % nodes_count];
						MPI_Recv(tmp_recv_array, block_size + LENGHT % nodes_count, MPI_INT, j + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size, block_size + LENGHT % nodes_count);
						qsort(merged_array, block_size + block_size + LENGHT % nodes_count, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, 0, block_size);
					}
					else
					{
						tmp_recv_array = new int[block_size];
						MPI_Recv(tmp_recv_array, block_size, MPI_INT, j + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size);
						qsort(merged_array, block_size * 2, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, 0, block_size);
					}
				}
				else if (current_node == j + 1)
				{
					tmp_recv_array = new int[block_size];
					/* отправление с остатком */
					if (diff_slices && current_node == LAST_NODE)
						MPI_Send(recv_array, block_size + LENGHT % nodes_count, MPI_INT, j, 0, MPI_COMM_WORLD);
					else
						MPI_Send(recv_array, block_size, MPI_INT, j, 0, MPI_COMM_WORLD);

					MPI_Recv(tmp_recv_array, block_size, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					if (diff_slices && current_node == LAST_NODE)
					{
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size + LENGHT % nodes_count, block_size);
						qsort(merged_array, block_size * 2 + LENGHT % nodes_count, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, block_size, 2 * block_size + LENGHT % nodes_count);
					}
					else
					{
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size);
						qsort(merged_array, block_size * 2, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, block_size, 2 * block_size);
					}
				}
			}
		}
		else // нечетная итерация
		{
			for (int j = 1; j < LAST_NODE; j += 2)
			{
				if (current_node == j)
				{
					MPI_Send(recv_array, block_size, MPI_INT, j + 1, 0, MPI_COMM_WORLD);
					if (diff_slices && j + 1 == LAST_NODE)
					{
						tmp_recv_array = new int[block_size + LENGHT % nodes_count];
						MPI_Recv(tmp_recv_array, block_size + LENGHT % nodes_count, MPI_INT, j + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size, block_size + LENGHT % nodes_count);
						qsort(merged_array, block_size * 2 + LENGHT % nodes_count, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, 0, block_size);
					}
					else
					{
						tmp_recv_array = new int[block_size];
						MPI_Recv(tmp_recv_array, block_size, MPI_INT, j + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size);
						qsort(merged_array, block_size * 2, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, 0, block_size);
					}
				}
				else if (current_node == j + 1)
				{
					tmp_recv_array = new int[block_size];
					if (diff_slices && current_node == LAST_NODE)
						MPI_Send(recv_array, block_size + LENGHT % nodes_count, MPI_INT, j, 0, MPI_COMM_WORLD);
					else
						MPI_Send(recv_array, block_size, MPI_INT, j, 0, MPI_COMM_WORLD);

					MPI_Recv(tmp_recv_array, block_size, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					if (diff_slices && current_node == LAST_NODE)
					{
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size + LENGHT % nodes_count, block_size);
						qsort(merged_array, block_size * 2 + LENGHT % nodes_count, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, block_size, 2 * block_size + LENGHT % nodes_count);
					}
					else
					{
						int* merged_array = merge_arrays(recv_array, tmp_recv_array, block_size);
						qsort(merged_array, block_size * 2, sizeof(int), compare);
						recv_array = get_part_of_array(merged_array, block_size, 2 * block_size);
					}
				}
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (current_node != ROOT_NODE)
	{
		if (diff_slices && current_node == LAST_NODE)
			MPI_Send(recv_array, block_size + LENGHT % nodes_count, MPI_INT, ROOT_NODE, current_node, MPI_COMM_WORLD);
		else
			MPI_Send(recv_array, block_size, MPI_INT, ROOT_NODE, current_node, MPI_COMM_WORLD);
	}
	else
	{
		int j = 0;
		int *mass = new int[block_size];
		int *mass1 = new int[block_size + LENGHT % nodes_count];

		for (int i = 0; i < block_size; i++)
		{
			array[j] = recv_array[i];
			j++;
		}

		for (int i = 1; i < nodes_count; i++)
		{
			if (diff_slices && i == LAST_NODE)
			{
				MPI_Recv(mass1, block_size + LENGHT % nodes_count, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
				for (int k = 0; k < block_size + LENGHT % nodes_count; k++)
				{
					array[j] = mass1[k];
					j++;
				}
			}
			else
			{
				MPI_Recv(mass1, block_size, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
				for (int k = 0; k < block_size; k++)
				{
					array[j] = mass1[k];
					j++;
				}
			}
		}
		for (int i = 0; i < LENGHT; i++)
			printf("%d ", array[i]);
		cout << endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}