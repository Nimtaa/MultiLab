// Factorial Program (Recursive, Iterative)

// Optimizes code for maximum speed
#pragma optimize( "2", on )
#include <stdio.h>
#include <omp.h>
// Adds an additional library so that timeGetTime() can be used
#pragma comment(lib, "winmm.lib")
#define INPUT 7
int IterativeFatorial();
int main(){
	printf("%d\n",IterativeFactorial());
	printf("%d\n",RecursiveDriver());
	
}
int IterativeFactorial(){
	int result= 1;
	#pragma omp parallel num_threads(4)
	{
	int private_result = 1;
	#pragma omp for
	for (int i=1;i<=INPUT;i++){
		private_result *= i;
	}
	#pragma omp critical
		result *= private_result;
	}
	
	return result;
}
int RecursiveFactorial(int n){
	if(n<2) return n;
	
	int m = 0;

	#pragma omp task shared(m)
	m = RecursiveFactorial(n - 1);

	#pragma omp taskwait
	return n * m;


}
int RecursiveDriver(){
	int result=0;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp single nowait
        {
            result = RecursiveFactorial(INPUT);
        }
    }
	return result;
}