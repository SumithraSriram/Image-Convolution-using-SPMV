#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include<conio.h>
#include<stdlib.h>
#include<tmmintrin.h>

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

int image[1000][1000];
int out[1000][1000];
int kernel[10][10];
int w, h, k;


void convolution()
{
	int i,j,m,n;
	int flag;
	int nn,mm,ii,jj;
	
	MyTimer timer;

	timer.Start();                // take initial reading
	Sleep(1000);                // wait a second 
	unsigned cpuspeed10 = (unsigned)(timer.Stop() / 100000);
	
	timer.Start();

	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{
			for(m=0; m < k; ++m)     // kernel rows
			{
			    mm = k - 1 - m;      // row index of flipped kernel
			
			    for(n=0; n < k; ++n) // kernel columns
			    {
			        nn = k - 1 - n;  // column index of flipped kernel
			
			        // index of input signal, used for checking boundary
			        ii = i + (m - k/2);
			        jj = j + (n - k/2);
			
			        // ignore input samples which are out of bound
			        if( ii >= 0 && ii < h && jj >= 0 && jj < w )
			            out[i][j] += image[ii][jj] * kernel[mm][nn];
			    }
			}
			out[i][j]/=9;
		}
		
	}
	
	unsigned cycle = (unsigned)timer.Stop();
	unsigned speed = (unsigned)(cycle / 100000);

	printf("\n\nTime = %d\n", cycle * 10000 / cpuspeed10);
}

int main ()
{
  FILE *fp, *f1, *f2; 
  int i,j;
  
  printf("Enter the height of the image: ");
  scanf("%d",&h);
  printf("Enter the width of the image: ");
  scanf("%d",&w);
  printf("Enter the size of the kernel: ");
  scanf("%d",&k);

  fp = fopen("Baboon512.txt", "r");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
		fscanf(fp, "%d ", &(image[i][j]));
  }
  
  fclose(fp);
  
  f1 = fopen("kernel.txt", "r");
  
  for(i=0;i<k;i++)
  {
	  for(j=0;j<k;j++)
		fscanf(f1, "%d ", &(kernel[i][j]));
  }
  fclose(f1);
  
  convolution();
  
  f2 = fopen("OutputB512.txt", "w");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
		fprintf(f2, "%d ", (out[i][j]));
	  fprintf(f2,"\n");
  }
  fclose(f2);
  
  
  getch();
  return 0;
}
  
