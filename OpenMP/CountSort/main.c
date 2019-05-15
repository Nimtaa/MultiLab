#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int NUM_POINTS = 10;
void count_sort(int a[], int n)
{
	int i, j, count;
	int *temp = malloc(n*sizeof(int));

	#pragma omp parallel for shared(a, n, temp) private(i, j, count)
	for (i = 0; i < n; i++) {
		count = 0;
		for (j = 0; j < n; j++)
			if (a[j] < a[i])
				count++;
			else if (a[j] == a[i] && j < i)
				count++;
		temp[count] = a[i];
	}

	#pragma omp parallel for shared(a, n, temp) private(i)
	for (i = 0; i < n; i++)
		a[i] = temp[i];

	free(temp);
}
void my_count_sort(int a[], int n)
{
	int i, j, count=0;
	int *temp = malloc(n*sizeof(int));
	for (i = 0; i < n; i++) {
        count  = 0;
        #pragma omp parallel for reduction(+ : count)
		for (j = 0; j < n; j++){
			if (a[j] < a[i])
				count++;
			else if (a[j] == a[i] && j < i)
				count++;
        }
		temp[count] = a[i];
	}

	#pragma omp parallel for shared(a, n, temp)
	for (int t = 0; t < n; t++)
		a[t] = temp[t];

	free(temp);
}

void print_v(int a[], int n)
{
	for (int i = 0; i < n; i++)
		printf("%d\n", a[i]);
}


int main(int argc, char *argv[])
{
	int n, *a;	
	n = NUM_POINTS;
	a = malloc(n*sizeof(int));
	for (int i = 0; i < n; i++)
		// scanf("%d", &a[i]);
        a[i] = n-i;    
    double start_time = omp_get_wtime();   
    // count_sort(a, n);
    my_count_sort(a, n);
    printf("%f",omp_get_wtime() - start_time);
	print_v(a, n);

	return 0;
}