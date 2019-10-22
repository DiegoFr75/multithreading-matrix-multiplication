
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <chrono> 


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

// Multiplies matrices A and B
DWORD WINAPI multiMatrix(LPVOID id) {
	int threadNumber = *static_cast<int*>(id);

	int reps = int(ARRAYSIZE/THREADS_QTI);
	int start = threadNumber*reps;
	int finish;
	(threadNumber+1 == THREADS_QTI)? finish = ARRAYSIZE : finish = start+reps;

	int startJ = 0;

	for(int i = start; i < finish; i++) {
		int sum = 0;
		int k = i%matrixSize;
		startJ = int(i/matrixSize)*matrixSize;

		for(int j = startJ; j < (startJ + matrixSize); j++) {
			sum = sum + A[j]*B[k];
			k = k+matrixSize;
		}
		C[i] = sum;
	}
}

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

	// Indexes of the threads
	int indexes[] = {0,1,2,3,4,5,6,7};

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

	// Time measurement will start here
	auto start = high_resolution_clock::now();

	// Threads ids
	DWORD *t_ids;
	// Threads
    HANDLE *threads;

    threads = new HANDLE[THREADS_QTI];
    t_ids = new DWORD[THREADS_QTI];   

    // Create the threads
    for (int h = 0; h < THREADS_QTI; h++) {
        threads[h] = CreateThread(NULL,0,multiMatrix,&indexes[h],0,&t_ids[h]);
        if (threads[h] == NULL) {
            cout << "An error occurred in the thread creation:" << h;
            return 1;
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < THREADS_QTI; i++) {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }

	// Time measurement will end here
	auto stop = high_resolution_clock::now(); 

	// Transforming the resulting vector into an matrix
	k = 0;
	for(int i = 0; i < matrixSize; i++) {
		for(int j = 0; j < matrixSize; j++) {
			matrixC[i][j] = C[k];
			k++;
		}	
	}

	auto duration = duration_cast<milliseconds>(stop - start);
	cout <<"Time taken by function: "<< duration.count() << endl; 

    return 0;
}