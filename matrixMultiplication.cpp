#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

using namespace std;

double matrixA[1000][1000];
double matrixB[1000][1000];
double matrixC[1000][1000];

void populateMatrices(int);
void serialMultiply(int);
void parallelMultiply(int);
void *cellSum(void *args);
void displayMatrix(int);

// The argument with two variables to be passed for pThreads.
struct arg_struct{
    int i;
    int noOfRows;
};

int main(){
	populateMatrices(1000);
	struct timespec start, finish;		//to save start finish time.
	double elapsed;				

	clock_gettime(CLOCK_MONOTONIC, &start);
	serialMultiply(1000);				
	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout << "Time for serial multiplication: " << elapsed << " seconds"; 
	cout << endl;

	clock_gettime(CLOCK_MONOTONIC, &start);
	parallelMultiply(1000);					//calculate multiplication serially.
	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout << "Time for parallel multiplication: " << elapsed << " seconds"; 
	cout << endl;
}

//To fill the two matrices with random values and to clean matrix C.
void populateMatrices(int noOfRows){
	srand(time(NULL));
	for(int i = 0; i < noOfRows; i++){
		for(int j =0; j< noOfRows; j++){
			
			matrixA[i][j] = rand();
			matrixB[i][j] = rand();
			matrixC[i][j] = 0;
		}
	}
}

//todo: remove this, only for testing.
void displayMatrix(int n){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			cout << " " << matrixC[i][j];
			cout << " ";

		}
		cout << endl;
	}
}	

//serial multiplication of two matrices.
void serialMultiply(int noOfRows){
	int sum = 0;
	double firstMatrixRow [noOfRows];
	double secondMatrixColumn [noOfRows];

	for (int i = 0; i < noOfRows; i++){
		for (int j = 0; j < noOfRows; j++){
			sum = 0;
			for (int k = 0; k < noOfRows; k++){
				sum += matrixA[i][k] * matrixB[k][j]; 
			}
			matrixC[i][j] = sum;
		}
	}
	cout << endl;
}

//parallel multiplication
void parallelMultiply(int noOfRows){
	pthread_t threads[(noOfRows)];
	for (int i = 0; i < noOfRows; i++){			//create one pThread to each raw of the resulting matrix.
			struct arg_struct args;
			args.i = i;
			args.noOfRows = noOfRows;
			pthread_create(&threads[i], NULL,cellSum, (void *)&args);	
	}
	for (int i = 0; i < noOfRows; i++){
		pthread_join(threads[i], NULL);			//waiting till all the pThreads finish job.
		
	};
}

void *cellSum(void *arguments){
	struct arg_struct *args = (struct arg_struct *)arguments;
	int i = (int)args->i;					//getting the parameters in the arguement passed.
	int noOfRows = (int)args->noOfRows;

	for (int j = 0; j < noOfRows; j++){		//filling the raw which is assigned to the certian pThread.
		int sum = 0;
		for (int k = 0; k < noOfRows; k++){
			sum += matrixA[i][k] * matrixB[k][j]; 
		}
		matrixC[i][j] = sum;
	}
}
