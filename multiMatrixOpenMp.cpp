#include <iostream>
#include <cstdlib>
#include <chrono> 
#include <omp.h>

using namespace std::chrono; 
using namespace std;

#define SMALL_SIZE 500
#define MEDIUM_SIZE 1000
#define LARGE_SIZE 1500

// Orden of the matrix (only square matrix)
int matrixSize;

// Quantity of Threads, 1,2,4,8
int THREADS_QTI;

int ARRAYSIZE;

// Matrices
int *A;
int *B;

// Resulting matrix
int *C;


int main() {
	char size;
	cout << "Matrix order: "<<endl;
	cout << "s - small size ("<< SMALL_SIZE <<")\nm - medium size ("<< MEDIUM_SIZE <<")\nl - large size (" <<LARGE_SIZE <<")\n";
    cin >> size;
	switch(size) {
		case 's':
			matrixSize = SMALL_SIZE;
			break;
		case 'm':
			matrixSize = MEDIUM_SIZE;
			break;
		case 'l':
			matrixSize = LARGE_SIZE;
			break;
		default:
			cout<<"Invalid size";
			return 1;
	}

	cout << "Threads quantity (1, 2, 4, 8): ";
    cin >> THREADS_QTI;

	ARRAYSIZE = matrixSize*matrixSize;

	// Creating matrices for the tests
	int** matrixA = new int*[matrixSize];
	int** matrixB = new int*[matrixSize];
	int** matrixC = new int*[matrixSize];

	for(int i = 0; i < matrixSize; i++) {
		matrixA[i] = new int[matrixSize];
		matrixB[i] = new int[matrixSize];
		matrixC[i] = new int[matrixSize];
	}

	// Filling the matrices
	for(int i = 0; i < matrixSize; i++) {
		for(int j = 0; j < matrixSize; j++) {
			matrixA[i][j] = rand() % 10;
			matrixB[i][j] = rand() % 10;
		}
	}

	// Transforming the matrix into a 1 dimession array
	A = new int[ARRAYSIZE];
	B = new int[ARRAYSIZE];
	C = new int[ARRAYSIZE];

	int k = 0;

	for(int i = 0; i < matrixSize; i++) {
		for(int j = 0; j < matrixSize; j++) {
			A[k] = matrixA[i][j];
			B[k] = matrixB[i][j];
			k = k+1;
		}
	}

	// Set quantity of threads
	omp_set_num_threads(THREADS_QTI);
	
	// Time measurement will start here
	auto startC = high_resolution_clock::now();

	#pragma omp parallel 
	{
		int threadNumber = omp_get_thread_num();

		int reps = int(ARRAYSIZE/THREADS_QTI);
		int start = threadNumber*reps;
		int finish;
		(threadNumber+1 == THREADS_QTI)? finish = ARRAYSIZE : finish = start+reps;

		int startJ = 0;

		for(unsigned int i = start; i < finish; i++) {
			int sum = 0;
			int k = i%matrixSize;
			startJ = int(i/matrixSize)*matrixSize;

			for(unsigned int j = startJ; j < (startJ + matrixSize); j++) {
				sum = sum + A[j]*B[k];
				k = k+matrixSize;
			}
			C[i] = sum;
		}
	}

	// Time measurement will end here
	auto stopC = high_resolution_clock::now();

	// Transforming the resulting vector into an matrix
	k = 0;
	for(int i = 0; i < matrixSize; i++) {
		for(int j = 0; j < matrixSize; j++) {
			matrixC[i][j] = C[k];
			k++;
		}	
	} 

	auto duration = duration_cast<milliseconds>(stopC - startC);
	cout <<"Time taken by function: "<< duration.count() << endl; 

    return 0;
}