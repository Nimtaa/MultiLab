/*
*				In His Exalted Name
*	Title:	Prefix Sum Sequential Code
*	Author: Ahmad Siavashi, Email: siavashi@aut.ac.ir
*	Date:	29/04/2018
*/

// Let it be.
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

void omp_check();
void fill_array(int *a, size_t n);
double prefix_sum(int *a, size_t n);
void print_array(int *a, size_t n);

int main(int argc, char *argv[]) {
	// Check for correct compilation settings
	omp_check();
	// Input N
	size_t n = 0;
	printf("[-] Please enter N: ");
	scanf("%uld\n", &n);
	// Allocate memory for array
	int * a = (int *)malloc(n * sizeof a);
	// Fill array with numbers 1..n
	fill_array(a, n);
	// Print array
	// print_array(a, n);
	// Compute prefix sum
	printf("Time: %f \n",prefix_sum(a, n));
	// print_array(a, n);
	// Free allocated memory
	free(a);
	return EXIT_SUCCESS;
}

double prefix_sum(int *a, size_t n) {
    double start_time = omp_get_wtime();
        for (int i = 1; i < n; i <<= 1)   {  
            #pragma omp parallel for
            for (int j = 0; j < n; j += 2 * i) 
               a[2 * i + j - 1] = a[2 * i + j - 1] + a[i + j - 1]; 
        }
     a[n - 1] = 0; 
 
     for (int i = n / 2; i > 0; i >>= 1) {    
         #pragma omp parallel for
               for (int j = 0; j < n; j += 2 * i) {  
                    int temp = a[i + j - 1];       
                   a[i + j - 1] = a[2 * i + j - 1];         
                    a[2 * i + j - 1] = temp + a[2 * i + j - 1];     
         } 
       } 
       return omp_get_wtime() - start_time;
    } 

void print_array(int *a, size_t n) {
	int i;
	printf("[-] array: ");
	for (i = 0; i < n; ++i) {
		printf("%d, ", a[i]);
	}
	printf("\b\b  \n");
}

void fill_array(int *a, size_t n) {
	int i;
	for (i = 0; i < n; ++i) {
		a[i] = i + 1;
	}
}

void omp_check() {
	printf("------------ Info -------------\n");
#ifdef _DEBUG
	printf("[!] Configuration: Debug.\n");
#pragma message ("Change configuration to Release for a fast execution.")
#else
	printf("[-] Configuration: Release.\n");
#endif // _DEBUG
#ifdef _M_X64
	printf("[-] Platform: x64\n");
#elif _M_IX86 
	printf("[-] Platform: x86\n");
#pragma message ("Change platform to x64 for more memory.")
#endif // _M_IX86 
#ifdef _OPENMP
	printf("[-] OpenMP is on.\n");
	printf("[-] OpenMP version: %d\n", _OPENMP);
#else
	printf("[!] OpenMP is off.\n");
	printf("[#] Enable OpenMP.\n");
#endif // _OPENMP
	printf("[-] Maximum threads: %d\n", omp_get_max_threads());
	printf("[-] Nested Parallelism: %s\n", omp_get_nested() ? "On" : "Off");
#pragma message("Enable nested parallelism if you wish to have parallel region within parallel region.")
	printf("===============================\n");
}
