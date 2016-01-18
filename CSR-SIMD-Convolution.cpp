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

float *inv;
float *outv;
int data[1000000];
int col[1000000];
int ptr[1000000];
int nnz,w,h;


void spmv()
 {
	int i,j,k,t;
	int x;
	float *c;
	__m128 data1, vect1,res1, temp, tempmr;
	float *tempd;

	tempd=(float *) malloc (4*sizeof(float));
	c=(float *) malloc (sizeof(float));

	for(i=0;i<(w*h);i++)
		*(outv+i) = 0;
 	
	MyTimer timer;

	timer.Start();                // take initial reading
	Sleep(1000);                // wait a second 
	unsigned cpuspeed10 = (unsigned)(timer.Stop() / 100000);
	
	timer.Start();

    for (i = 0; i < (w*h); i++) 
	{
		for (k =ptr[i]; k < ptr[i+1]; k=k+4) 
		{
			x= ptr[i+1] - ptr[i];
			if( x-k < 4)
				t=x;
			else
				t=4;
			for(j=0;j<t;j++)
				*(tempd+j) = data[k+j];
			while(j<4)
			{
				*(tempd+j) = 0;
				j++;
			}

			j=col[k];
			
			data1=_mm_loadu_ps(tempd);
			vect1=_mm_loadu_ps((inv+j));

			temp=_mm_mul_ps(data1,vect1);

			res1=_mm_hadd_ps(temp,temp);
			res1=_mm_hadd_ps(res1,res1);
			_mm_storeu_ps(c,res1);
			*(outv+i)+=*c;
			
		}
		//printf("%f\n", *(res+i));
		
	}
	
	unsigned cycle = (unsigned)timer.Stop();
	unsigned speed = (unsigned)(cycle / 100000);

	printf("\n\nTime = %d\n", cycle * 10000 / cpuspeed10);
 }
 
int main ()
{
  FILE *fp, *f1, *f2; 
  int i,j;
  char line[1024];

  printf("Enter the height of the image: ");
  scanf("%d",&h);
  printf("Enter the width of the image: ");
  scanf("%d",&w);

  inv = (float *) malloc ((w*h)*sizeof(float));
  outv = (float *) malloc ((w*h)*sizeof(float));

  fp = fopen("Lena256.txt", "r");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
		fscanf(fp, "%lf ", (inv+(i*w)+j));
  }
  fclose(fp);
  
  f1 = fopen("KernelCSR.txt", "r");
  
  fscanf(f1,"%d\n",&nnz);
  //printf("%d", nnz);
  for(i=0;i<nnz;i++)
  {
		fscanf(fp, "%d %d\n", &(col[i]),&(data[i]));
  }
  for(i=0;i<=(h*w);i++)
  {
		fscanf(fp, "%d ", &(ptr[i]));
		//printf("%d", ptr[i]);
  }
  
  fclose(f1);
  
  spmv();
  
  f2 = fopen("OutputKS.txt", "w");
    
  for(i=0;i<h*w;i++)
  {
  	  fprintf(f2, "%d ", *(outv+i));
	  if( (i+1)%w == 0)
		  fprintf(f2,"\n");
  }
  
  fclose(f2);
  getch();
  return 0;
}
  