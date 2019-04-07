#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include<stdbool.h>

typedef struct {
	int *A, *B, *C;
	int n, m, p;
} DataSet;

void fillDataSet(DataSet *dataSet);
void printDataSet(DataSet dataSet, bool resultOnly);
void closeDataSet(DataSet dataSet);
double multiply(DataSet dataSet);
double multiply_parallel_1D(DataSet dataSet);
double multiply_parallel_2D(DataSet dataSet);
void printTime(char* description, double time, int thraeds);
void check_addition_methods_timing(dataSet);

int main(int argc, char *argv[]){
	DataSet dataSet;
	if(argc < 4){
		printf("[-] Invalid No. of arguments.\n");
		printf("[-] Try -> <n> <m> <p>\n");
		printf(">>> ");
		scanf("%d %d %d", &dataSet.n, &dataSet.m, &dataSet.p);
	}else{
		dataSet.n = atoi(argv[1]);
		dataSet.m = atoi(argv[2]);
		dataSet.p = atoi(argv[3]);
	}
	fillDataSet(&dataSet);
	check_addition_methods_timing(dataSet);
	// multiply(dataSet);
	// printDataSet(dataSet);
	closeDataSet(dataSet);
	system("PAUSE");
	return EXIT_SUCCESS;
}

void fillDataSet(DataSet *dataSet){
	int i, j;
	
	dataSet->A = (int *) malloc(sizeof(int) * dataSet->n * dataSet->m);
	dataSet->B = (int *) malloc(sizeof(int) * dataSet->m * dataSet->p);
	dataSet->C = (int *) malloc(sizeof(int) * dataSet->n * dataSet->p);
	
	srand(time(NULL));

	for(i = 0; i < dataSet->n; i++){
		for(j = 0; j < dataSet->m; j++){
			dataSet->A[i*dataSet->m + j] = rand() % 100;
		}
	}

	for(i = 0; i < dataSet->m; i++){
		for(j = 0; j < dataSet->p; j++){
			dataSet->B[i*dataSet->p + j] = rand() % 100;
		}
	}
}

void printTime(char* description, double time, int threads) {
	printf("%s - Time: %f, threads: %d \n", description, time, threads);
}

void check_addition_methods_timing(DataSet dataSet) {
	double time;
	int numThreads[] = {1,2,4,8};
	time = multiply(dataSet);
	printf("Normal %f \n", time);
		for (int t = 0; t < 4; t++) {
			omp_set_num_threads(numThreads[t]);
			time = multiply_parallel_1D(dataSet);
			printTime("Parallel 1D", time, numThreads[t]);
			time = multiply_parallel_2D(dataSet);
			printTime("Parallel 2D", time, numThreads[t]);		
			printf("\n######################################################\n\n");
		}
}


void printDataSet(DataSet dataSet, bool resultOnly){
	int i, j;
	if(!resultOnly){
		printf("[-] Matrix A\n");
		for(i = 0; i < dataSet.n; i++){
			for(j = 0; j < dataSet.m; j++){
				printf("%-4d", dataSet.A[i*dataSet.m + j]);
			}
			putchar('\n');
		}
		
		printf("[-] Matrix B\n");
		for(i = 0; i < dataSet.m; i++){
			for(j = 0; j < dataSet.p; j++){
				printf("%-4d", dataSet.B[i*dataSet.p + j]);
			}
			putchar('\n');
		}
	}
	printf("[-] Matrix C\n");
	for(i = 0; i < dataSet.n; i++){
		for(j = 0; j < dataSet.p; j++){
			printf("%-8d", dataSet.C[i*dataSet.p + j]);
		}
		putchar('\n');
	}
}

void closeDataSet(DataSet dataSet){
	free(dataSet.A);
	free(dataSet.B);
	free(dataSet.C);
}

double multiply(DataSet dataSet){

	int i, j, k, sum;
	double start_time = omp_get_wtime();
	for(i = 0; i < dataSet.n; i++){
		for(j = 0; j < dataSet.p; j++){
			sum = 0;
			for(k = 0; k < dataSet.m; k++){
				sum += dataSet.A[i * dataSet.m + k] * dataSet.B[k * dataSet.p + j];
			}
			dataSet.C[i * dataSet.p + j] = sum;
		}
	}
	return omp_get_wtime() - start_time;
}
double multiply_parallel_1D(DataSet dataSet){
	double start_time = omp_get_wtime();
	#pragma omp parallel for
	for(int i = 0; i < dataSet.n; i++){
		for(int j = 0; j < dataSet.p; j++){
			int sum = 0;
			for(int k = 0; k < dataSet.m; k++){
				sum += dataSet.A[i * dataSet.m + k] * dataSet.B[k * dataSet.p + j];
			}
			dataSet.C[i * dataSet.p + j] = sum;
		}
	}
	return omp_get_wtime() - start_time;
}

double multiply_parallel_2D(DataSet dataSet){
	double start_time = omp_get_wtime();
	int i,j,sum;
	#pragma omp parallel for private(i)
	for(i = 0; i < dataSet.n; i++){
		#pragma omp parallel for private(j)
		for( j = 0; j < dataSet.p; j++){
			int sum = 0;
			for(int k = 0; k < dataSet.m; k++){
				sum += dataSet.A[i * dataSet.m + k] * dataSet.B[k * dataSet.p + j];
			}
			dataSet.C[i * dataSet.p + j] = sum;
		}
	}
	return omp_get_wtime() - start_time;
}
