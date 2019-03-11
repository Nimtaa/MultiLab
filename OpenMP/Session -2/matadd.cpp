#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

typedef struct {
	int *A, *B, *C;
	int n, m;
} DataSet;

double add(DataSet dataSet);
double add_row_parallel(DataSet dataSet);
double add_col_parallel(DataSet dataSet);
double add_item_parallel(DataSet dataSet);
double add_row_parallel(DataSet dataSet, int chunkSize);
double add_col_parallel(DataSet dataSet, int chunkSize);
double add_item_parallel(DataSet dataSet, int chunkSize);

void check_addition_methods_timing(DataSet dataSet);
void check_addition_methods_correctness(DataSet dataSet);
void printTime(char* description, double time, int thraeds, int chunkSize);

void fillDataSet(DataSet *dataSet);
void printDataSet(DataSet dataSet);
void closeDataSet(DataSet dataSet);
void printDataSet(DataSet dataSet, bool onlyResult);

int main(int argc, char *argv[]) {
	DataSet dataSet;
	if (argc < 3) {
		printf("[-] Invalid No. of arguments.\n");
		printf("[-] Try -> <n> <m> \n");
		printf(">>> ");
		scanf("%d %d", &dataSet.n, &dataSet.m);
	}
	else {
		dataSet.n = atoi(argv[1]);
		dataSet.m = atoi(argv[2]);
	}
	fillDataSet(&dataSet);
	
	check_addition_methods_timing(dataSet);
	
	closeDataSet(dataSet);
	system("PAUSE");
	return EXIT_SUCCESS;
}

void printTime(char* description, double time, int threads, int chunkSize) {
	printf("%s - Time: %f, threads: %d, chunkSize: %d\n", description, time, threads, chunkSize);
}

void check_addition_methods_timing(DataSet dataSet) {
	double time;
	int numThreads[] = {1, 4, 8, 16};
	int chunkSizes[] = {1, 128};
	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 4; t++) {
			omp_set_num_threads(numThreads[t]);
			time = add(dataSet);
			printTime("Normal", time, numThreads[t], chunkSizes[c]);
			time = add_row_parallel(dataSet, chunkSizes[c]);
			printTime("Row Parallel", time, numThreads[t], chunkSizes[c]);
			time = add_col_parallel(dataSet, chunkSizes[c]);
			printTime("Column Parallel", time, numThreads[t], chunkSizes[c]);
			time = add_item_parallel(dataSet, chunkSizes[c]);
			printTime("Item Parallel", time, numThreads[t], chunkSizes[c]);
			printf("\n######################################################\n\n");
		}
	}
}

void check_addition_methods_correctness(DataSet dataSet) {
	add(dataSet);
	printDataSet(dataSet);
	printf("Normal\n");
	add_row_parallel(dataSet);
	printDataSet(dataSet, true);
	printf("Row Parallel\n");
	add_col_parallel(dataSet);
	printDataSet(dataSet, true);
	printf("Column Parallel\n");
	add_item_parallel(dataSet);
	printDataSet(dataSet, true);
	printf("Item Parallel\n");
}

void fillDataSet(DataSet *dataSet) {
	int i, j;

	dataSet->A = (int *)malloc(sizeof(int) * dataSet->n * dataSet->m);
	dataSet->B = (int *)malloc(sizeof(int) * dataSet->n * dataSet->m);
	dataSet->C = (int *)malloc(sizeof(int) * dataSet->n * dataSet->m);

	srand(time(NULL));

	for (i = 0; i < dataSet->n; i++) {
		for (j = 0; j < dataSet->m; j++) {
			dataSet->A[i*dataSet->m + j] = rand() % 100;
			dataSet->B[i*dataSet->m + j] = rand() % 100;
		}
	}
}

void printDataSet(DataSet dataSet) {
	printDataSet(dataSet, false);
}

void printDataSet(DataSet dataSet, bool onlyResult) {
	int i, j;

	if (!onlyResult) {
		printf("[-] Matrix A\n");
		for (i = 0; i < dataSet.n; i++) {
			for (j = 0; j < dataSet.m; j++) {
				printf("%-4d", dataSet.A[i*dataSet.m + j]);
			}
			putchar('\n');
		}

		printf("[-] Matrix B\n");
		for (i = 0; i < dataSet.n; i++) {
			for (j = 0; j < dataSet.m; j++) {
				printf("%-4d", dataSet.B[i*dataSet.m + j]);
			}
			putchar('\n');
		}
	}

	printf("[-] Matrix C\n");
	for (i = 0; i < dataSet.n; i++) {
		for (j = 0; j < dataSet.m; j++) {
			printf("%-8d", dataSet.C[i*dataSet.m + j]);
		}
		putchar('\n');
	}
}

void closeDataSet(DataSet dataSet) {
	free(dataSet.A);
	free(dataSet.B);
	free(dataSet.C);
}

double add(DataSet dataSet) {
	double start_time = omp_get_wtime();
	for (int i = 0; i < dataSet.n; i++) {
		for (int j = 0; j < dataSet.m; j++) {
			dataSet.C[i * dataSet.m + j] = dataSet.A[i * dataSet.m + j] + dataSet.B[i * dataSet.m + j];
		}
	}
	return omp_get_wtime() - start_time;
}

double add_row_parallel(DataSet dataSet) {
	return add_row_parallel(dataSet, 1);
}

double add_row_parallel(DataSet dataSet, int chunkSize) {
	double start_time = omp_get_wtime();
	#pragma omp parallel for schedule(static, chunkSize)
	for (int i = 0; i < dataSet.n; i++) {
		for (int j = 0; j < dataSet.m; j++) {
			dataSet.C[i * dataSet.m + j] = dataSet.A[i * dataSet.m + j] + dataSet.B[i * dataSet.m + j];
		}
	}
	return omp_get_wtime() - start_time;
}

double add_col_parallel(DataSet dataSet) {
	return add_col_parallel(dataSet, 1);
}

double add_col_parallel(DataSet dataSet, int chunkSize) {
	double start_time = omp_get_wtime();
	#pragma omp parallel for schedule(static, chunkSize)
	for (int j = 0; j < dataSet.m; j++) {
		for (int i = 0; i < dataSet.n; i++) {
			dataSet.C[i * dataSet.m + j] = dataSet.A[i * dataSet.m + j] + dataSet.B[i * dataSet.m + j];
		}
	}
	return omp_get_wtime() - start_time;
}

double add_item_parallel(DataSet dataSet) {
	return add_item_parallel(dataSet, 1);
}

double add_item_parallel(DataSet dataSet, int chunkSize) {
	double start_time = omp_get_wtime();
	omp_set_nested(true);
	#pragma omp parallel for schedule(static, chunkSize)
	for (int i = 0; i < dataSet.n; i++) {
		#pragma omp parallel for schedule(static, chunkSize)
		for (int j = 0; j < dataSet.m; j++) {
			dataSet.C[i * dataSet.m + j] = dataSet.A[i * dataSet.m + j] + dataSet.B[i * dataSet.m + j];
		}
	}
	return omp_get_wtime() - start_time;
}