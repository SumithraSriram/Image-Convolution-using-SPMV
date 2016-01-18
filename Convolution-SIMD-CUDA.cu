
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include<conio.h>
#include<stdlib.h>
#include<tmmintrin.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include<cuda.h>

#pragma once
#pragma warning(disable : 4035)

inline unsigned __int64 GetCycleCount(void)
{
	_asm    _emit 0x0F
	_asm    _emit 0x31
}

class MyTimer
{
	unsigned __int64  m_startcycle;

public:

	unsigned __int64  m_overhead;

	MyTimer(void)
	{
		m_overhead = 0;
		Start();
		m_overhead = Stop();
	}

	void Start(void)
	{
		m_startcycle = GetCycleCount();
	}

	unsigned __int64 Stop(void)
	{
		return GetCycleCount() - m_startcycle - m_overhead;
	}
};


__global__ void csrSpmv(int *x,int * y,int * ptr,int * indices,int * data,int num_rows)
{
	int row = blockDim.x * blockIdx.x + threadIdx.x ;
	if (row < num_rows)
	{
		int dot = 0;
		for (int j = ptr[row]; j < ptr[row+1]; j++) 
			dot += data[j] * x[indices[j]];

		y[row] = dot;
	}
}

int main()
{
    int k;
    int h;
    int w;
    int * hostInputImageData;
    int * hostOutputImageData;
    int * hostptr;
	int * hostcol;
	int * hostdata;
    int * deviceInputImageData;
    int * deviceOutputImageData;
    int * deviceMaskData;
	int * deviceptr;
	int * devicecol;
	int * devicedata;

    
  FILE *fp, *f1, *f2; 
  int i,j,nnz;
  
  printf("Enter the height of the image: ");
  scanf("%d",&h);
  printf("Enter the width of the image: ");
  scanf("%d",&w);
  
  hostInputImageData = (int *) malloc(w*h*sizeof(int));
  hostOutputImageData = (int *) malloc(w*h*sizeof(int));
  hostptr = (int *) malloc((w*h+1)*sizeof(int));
  
  fp = fopen("Cameraman160.txt", "r");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
	  {
		fscanf(fp, "%d ", (hostInputImageData+(w*i)+j));
	  }
  }
  
  fclose(fp);
  
  f1 = fopen("Kernel160.txt", "r");
  
  fscanf(f1,"%d\n",&nnz);

  hostcol = (int *) malloc(nnz*sizeof(int));
  hostdata = (int *) malloc(nnz*sizeof(int));

  printf("nnz=%d\n", nnz);
  for(i=0;i<nnz;i++)
  {
		fscanf(fp, "%d %d\n", (hostcol+i),(hostdata+i));
  }
  for(i=0;i<=(h*w);i++)
  {
		fscanf(fp, "%d ", (hostptr+i));
  }
    
  fclose(f1);

    cudaMalloc((void **) &deviceInputImageData, w * h * sizeof(int));
    cudaMalloc((void **) &deviceOutputImageData, w * h * sizeof(int));
    cudaMalloc((void **) &deviceptr, (w*h +1) * sizeof(int));
	cudaMalloc((void **) &devicecol, nnz * sizeof(int));
	cudaMalloc((void **) &devicedata, nnz * sizeof(int));

	cudaMemcpy(deviceInputImageData, hostInputImageData, w * h * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(deviceptr, hostptr, (w*h +1) * sizeof(int) ,cudaMemcpyHostToDevice);
	cudaMemcpy(devicecol, hostcol, nnz * sizeof(int) ,cudaMemcpyHostToDevice);
	cudaMemcpy(devicedata, hostdata, nnz * sizeof(int) ,cudaMemcpyHostToDevice);
    
	dim3 block(32,1,1);
	dim3 grid( (w*h)/32 , 1 , 1);

	MyTimer timer;

	timer.Start();                // take initial reading
	Sleep(1000);                // wait a second 
	unsigned cpuspeed10 = (unsigned)(timer.Stop() / 100000);
	
	timer.Start();
	
	csrSpmv<<<grid, block>>>(deviceInputImageData, deviceOutputImageData, deviceptr, devicecol, devicedata, (w*h));	

	unsigned cycle = (unsigned)timer.Stop();
	unsigned speed = (unsigned)(cycle / 100000);

	printf("\n\nTime = %d\n", cycle * 10000 / cpuspeed10);
    
    cudaMemcpy(hostOutputImageData,deviceOutputImageData, w * h * sizeof(int),cudaMemcpyDeviceToHost);
    

   	f2 = fopen("Output.txt", "w");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
		fprintf(f2, "%d ", *(hostOutputImageData+(w*i)+j));
	  fprintf(f2,"\n");
  }
  fclose(f2);

  cudaFree(deviceInputImageData);
    cudaFree(deviceOutputImageData);
    cudaFree(deviceptr);
	cudaFree(devicecol);
	cudaFree(devicedata);

    getch();
    return 0;
}
