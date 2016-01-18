#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
int kernel[10][10];
int data[1000000];
int col[1000000];
int ptr[1000000];
int w, h, k,c;

void makekernel()
{
	int i,j,m,n;
	int nn,mm,ii,jj;
	ptr[0]=0;
	c=0;
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
			        if( ii >= 0 && ii < h && jj >= 0 && jj < w && kernel[mm][nn]!=0 )
			        {
			            //ks[(i*w)+j][(ii*w)+jj]= kernel[mm][nn];
			            data[c]=kernel[mm][nn];
			            col[c]=(ii*w)+jj;
			            c++;
			        }
			    }
			}
			ptr[(i*w)+j+1]=c;
		}
	}
}

int main ()
{
  FILE *fp, *f1; 
  int i,j;
  
  printf("Enter the height of the image: ");
  scanf("%d",&h);
  printf("Enter the width of the image: ");
  scanf("%d",&w);
  printf("Enter the size of the kernel: ");
  scanf("%d",&k);

  fp = fopen("kernel.txt", "r");
  
  for(i=0;i<k;i++)
  {
	  for(j=0;j<k;j++)
		fscanf(fp, "%d ", &(kernel[i][j]));
  }
  fclose(fp);
  
   
  makekernel();
  
  f1=fopen("Kernel512.txt","w");
     
     fprintf(f1, "%d\n",c);
     for(i=0;i<c;i++)
     {
         fprintf(f1, "%d ",col[i]);
         fprintf(f1, "%d\n",data[i]);
     }
     for(i=0;i<=(h*w);i++)
     {
        fprintf(f1, "%d ",ptr[i]);
	 }
         
    fclose(f1);
  
   
  printf("End");
  getch();
  return 0;
}
  
