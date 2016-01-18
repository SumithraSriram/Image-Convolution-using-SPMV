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

int inv[10000000];
int outv[10000000];
int data[1000000];
int col[1000000];
int ptr[1000000];
int nnz,w,h;


void spmv()
 {
 	int i,j,k;
 	
	MyTimer timer;

	timer.Start();                // take initial reading
	Sleep(1000);                // wait a second 
	unsigned cpuspeed10 = (unsigned)(timer.Stop() / 100000);
	
	timer.Start();

    for (i = 0; i < nnz; i++) 
	{
		outv[i] = 0;
		for (k = ptr[i]; k < ptr[i+1]; k++) 
		{
			j = col[k];
			outv[i] = outv[i]+ (data[k] * inv[j]);
		}
	//	printf("%d\n", outv[i]);
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

  fp = fopen("Baboon512.txt", "r");
  
  for(i=0;i<h;i++)
  {
	  for(j=0;j<w;j++)
		fscanf(fp, "%d ", &(inv[(i*w)+j]));
  }
  fclose(fp);
  
  f1 = fopen("Kernel512.txt", "r");
  
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
  
  f2 = fopen("Output.txt", "w");
    
  for(i=0;i<h*w;i++)
  {
  	  fprintf(f2, "%d ", outv[i]);
	  if( (i+1)%w == 0)
		  fprintf(f2,"\n");
  }
  
  fclose(f2);
  getch();
  return 0;
}
  