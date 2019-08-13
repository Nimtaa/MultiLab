#include <cuda.h>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include <stdlib.h> 
#include <fstream>
#include <iostream>
#include <cufft.h>
#include <cufftXt.h> 
#include <algorithm> 
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <string>
#include <string>
#include <iostream>
#include <Windows.h>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <omp.h>
#include<limits>
#include <dirent.h>


#define BATCH 1
#define CHUNK_SIZE 2000
#define NUM_THREAD 4

#define MIN_FLOAT 0.000000
#define MAX_FLOAT 99999999.9

typedef std::vector<std::string> stringVectType;

using namespace std;


__global__ void similarityKernel(cufftComplex ** music_data, cufftComplex ** sample_data, float *out) {
	float dis_end[5];
	__shared__ float distance[1024];
	cufftComplex inner_sum;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	//each thread calculates the diffrence between window of song and sample.
	for (int j = 0; j < 445; j++) {
		for (int k = 0; k < CHUNK_SIZE; k++) {
			inner_sum.x += (music_data[j + i][k].x - sample_data[j][k].x);
			inner_sum.y += (music_data[j + i][k].y - sample_data[j][k].y);
		}
		__syncthreads();

		distance[i] = (float)(inner_sum.x + inner_sum.y);
	}
		//first thread of each block
		if (threadIdx.x == 0) {
			float min = MAX_FLOAT;
			for (int i = 0; i < 1024; i++) {
				if (distance[i] < min) min = distance[i];
			}
			dis_end[blockIdx.x] = min;
		}

		for(int j=0;j<5;j++){
			out[j] = dis_end[j];
		}
	
}


// Helper function to read the specific directory
stringVectType files_of_directory(string nameOfDirectory)
{
	DIR *dir;
	struct dirent *ent;
	stringVectType files;
	if ((dir = opendir(nameOfDirectory.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			//printf("%s\n", ent->d_name);
			files.push_back(ent->d_name);
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		//perror("");
		cout << "could not open directory" << endl;
		//return EXIT_FAILURE;
	}
	return files;
}

cufftComplex **  read_music_file(string address_of_music, bool isOriginal) {

	int temp;
	int row_numbers;

	if (isOriginal) {
		row_numbers = 5000;
	}
	else {
		row_numbers = 445;
	}

	cufftComplex** output = new cufftComplex *[row_numbers];
	for (int i = 0; i < row_numbers; ++i)
		output[i] = new cufftComplex[CHUNK_SIZE];

	ifstream input_music;
	input_music.open(address_of_music);

	if (!input_music) {
		cout << "A problem has occured during reading the file";
		cout << "problem with this address: " << address_of_music << endl;
		exit(1);
	}

	int i = 0;
	int j = 0;

	while (i < row_numbers)
	{

		if (input_music >> temp) {
			output[i][j].x = temp;
			output[i][j].y = 0;
		}
		else {
			output[i][j].x = 0;
			output[i][j].y = 0;
		}
		j++;
		if (j == CHUNK_SIZE) {
			j = 0;
			i++;
		}
	}
	return output;
}

float similarity_calculation(cufftComplex ** music_data, cufftComplex ** sample_data) {
	float* out;
	float* out_host;
	cufftComplex** music_data_dev;
	cufftComplex** sample_data_dev;
	cudaError_t cudaStatus;
	// GPU memory allocation
	cudaStatus = cudaMalloc((void **)&music_data_dev, sizeof(cufftComplex)*CHUNK_SIZE * 5000);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		cout << "problem with cudaMalloc!" << endl;
		exit(1);
	}
	cudaStatus = cudaMalloc((void **)&sample_data_dev, sizeof(cufftComplex)*CHUNK_SIZE * 440);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(1);
	}
	
	cudaStatus = cudaMalloc((void **)&out, 5 *sizeof(float));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(1);
	}
	// Copy from host to device
	cudaStatus = cudaMemcpy(sample_data_dev, sample_data, CHUNK_SIZE * 440 * sizeof(cufftComplex), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(1);
	}
	// Copy from host to device
	cudaStatus = cudaMemcpy(music_data_dev, music_data, CHUNK_SIZE * 5000 * sizeof(cufftComplex), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(1);
	}
	
	//Kernel execution
	similarityKernel <<<5, 1024>>> (music_data_dev, sample_data_dev, out);

	// cout<<"Kernel Launched!"<<endl;
	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "Kernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		exit(1);
	}
	// Copy from device to host
	cudaStatus = cudaMemcpy(out_host, out, 5 * sizeof(float), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		exit(1);
	}

	cudaFree(music_data_dev);
	cudaFree(sample_data_dev);
	cudaFree(out);

	float min = MAX_FLOAT;
	for (int i = 0;i < 5;i++) {
		if (out_host[i] < min)
			min = out_host[i];
	}
	return min;
}

cufftComplex ** get_fourier_from_input(cufftComplex ** song, bool isOriginal) {

	int music_size;

	if (isOriginal) {
		music_size = 5000;
	}
	else {
		music_size = 445;
	}
	cufftHandle Plan;
#pragma omp parallel for 
	for (int i = 0; i < music_size; i++) {

		cufftComplex * device_music;

		// GPU memory allocation
		cudaMalloc((void **)&device_music, sizeof(cufftComplex)*CHUNK_SIZE);

		// Copy from host to device
		cudaMemcpy(device_music, song[i], CHUNK_SIZE * sizeof(cufftComplex), cudaMemcpyHostToDevice);

		// Executing the FFT
		cufftPlan1d(&Plan, CHUNK_SIZE, CUFFT_C2C, BATCH);
		cufftExecC2C(Plan, device_music, device_music, CUFFT_FORWARD);

		// Waits for the kernel to finish, and returns any errors encountered during the execution.
		cudaDeviceSynchronize();

		// Copy results from device to host
		cudaMemcpy(song[i], device_music, CHUNK_SIZE * sizeof(cufftComplex), cudaMemcpyDeviceToHost);

		cudaFree(device_music);
	}
	cufftDestroy(Plan);
	return song;
}

int main(int argc, char* argv[])
{
	// for (int i = 0;i<argc;i++) {
	// 	cout << argv[i] << endl;
	// }

	string sample_directory;
	string music_directory;

	stringVectType sample_files;
	stringVectType song_files;

	sample_directory = argv[2];
	music_directory = argv[1];

	sample_files = files_of_directory(".\\" + sample_directory);
	song_files = files_of_directory(".\\" + music_directory);

	for (int i = 2; i < sample_files.size(); i++) {
		
		cufftComplex** sample_result = read_music_file(".\\"+ sample_directory +"\\" + sample_files[i], 0);
		sample_result = get_fourier_from_input(sample_result, 0);
		float min = 1000;
		int counter = 0;
		for (int j = 2; j < song_files.size();j++) {
			cufftComplex** music_result = read_music_file(".\\" + music_directory +"\\" + song_files[i], 1);
			music_result = get_fourier_from_input(music_result, 1);
			float distance = similarity_calculation(music_result, sample_result);
			if (distance < min) {
				min = distance;
				counter++;
			}
		}
		if (counter == 0) {
			cout << "S" << i - 1 << ".wav>>>" << "NotFound" << endl;
		}else{
			cout << "S" << i - 1 << ".wav>>>" << counter << ".txt"<<endl;
		}
	}
	getchar();
	return 0;
}
