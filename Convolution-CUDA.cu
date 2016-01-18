
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

#define Mask_width  3
#define Mask_radius Mask_width/2
#define O_Tile_Width 8
#define Block_Width (O_Tile_Width + Mask_width -1)

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


__global__ void convolution(int *N, const int * __restrict__ M, int *O, int width, int height)
{
	
	__shared__ float ns[Block_Width][Block_Width];
	
	
	int tx= threadIdx.x;
	int ty = threadIdx.y;
	
	int row_o=blockIdx.y*O_Tile_Width +ty;
	int col_o=blockIdx.x*O_Tile_Width +tx;
	
	int row_i= row_o - Mask_radius;
	int col_i= col_o - Mask_radius;
	int out;
	
	if( (row_i >=0) &&( row_i<height) && (col_i >=0) && (col_i < width))
	{
		ns[ty][tx] = N[(row_i * width + col_i)]; 
	}
	else
	{
		ns[ty][tx] = 0.0f;
	}
	
	__syncthreads();
	
	if( ty< O_Tile_Width && tx< O_Tile_Width )
	{
		out=0.0f;
		for(int i=0;i<Mask_width;i++)
		{
			for(int j=0;j<Mask_width;j++)
			{
				out += M[i * Mask_width + j] * ns[i + ty][j+tx];
			}
		}
	}
	
	__syncthreads();
	
	if((row_o < height) && (col_o < width) &&(tx < O_Tile_Width) && (ty < O_Tile_Width))
	{
		O[(row_o * width + col_o)] = out;
	}
		
}				

int main()
{
    int k;
    int h;
    int w;
    int * hostInputImageData;
    int * hostOutputImageData;
    int * hostMaskData;
    int * deviceInputImageData;
    int * deviceOutputImageData;
    int * deviceMaskData;

    
  FILE *fp, *f1, *f2; 
  int i,j;
  
  printf("Enter the height of the image: ");
  scanf("%d",&h);
  printf("Enter the width of the image: ");
  scanf("%d",&w);
  printf("Enter the size of the kernel: ");
  scanf("%d",&k);

  hostInputImageData = (int *) malloc(w*h*sizeof(int));
  hostOutputImageData = (int *) malloc(w*h*sizeof(int));
  hostMaskData = (int *) malloc(k*k*sizeof(int));

  fp = fopen("Baboon512.txt", "r");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
	  {
		fscanf(fp, "%d ", (hostInputImageData+(w*i)+j));
		//printf("%d ", *(hostInputImageData+(w*i)+j));
	  }
	  //printf("\n");
  }
  
  fclose(fp);
  
  f1 = fopen("kernel.txt", "r");
  
  for(i=0;i<k;i++)
  {
	  for(j=0;j<k;j++)
	  {
		fscanf(f1, "%d ", (hostMaskData+(k*i)+j));
		//printf("%d ", *(hostMaskData+(k*i)+j));
	  }
	  //printf("\n");
  }
  fclose(f1);

    cudaMalloc((void **) &deviceInputImageData, w * h * sizeof(int));
    cudaMalloc((void **) &deviceOutputImageData, w * h * sizeof(int));
    cudaMalloc((void **) &deviceMaskData, k * k * sizeof(int));

	cudaMemcpy(deviceInputImageData, hostInputImageData, w * h * sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpy(deviceMaskData, hostMaskData, k * k * sizeof(int),cudaMemcpyHostToDevice);
    
	dim3 block(Block_Width, Block_Width,1);
	dim3 grid( (w-1)/O_Tile_Width +1 , (h-1)/O_Tile_Width +1 , 1);

	MyTimer timer;

	timer.Start();                // take initial reading
	Sleep(1000);                // wait a second 
	unsigned cpuspeed10 = (unsigned)(timer.Stop() / 100000);
	
	timer.Start();
	
	convolution<<<grid, block>>>(deviceInputImageData, deviceMaskData, deviceOutputImageData, w, h);	

	unsigned cycle = (unsigned)timer.Stop();
	unsigned speed = (unsigned)(cycle / 100000);

	printf("\n\nTime = %d\n", cycle * 10000 / cpuspeed10);
    
    cudaMemcpy(hostOutputImageData,deviceOutputImageData, w * h * sizeof(int),cudaMemcpyDeviceToHost);
    

   	f2 = fopen("OutputB512.txt", "w");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
		fprintf(f2, "%d ", *(hostOutputImageData+(w*i)+j));
	  fprintf(f2,"\n");
  }
  fclose(f2);

  cudaFree(deviceInputImageData);
    cudaFree(deviceOutputImageData);
    cudaFree(deviceMaskData);

    getch();
    return 0;
}
