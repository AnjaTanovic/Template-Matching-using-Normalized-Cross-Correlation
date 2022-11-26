#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "img_file_1.h"
#include "tmpl_file_1.h"
#include "resp_file_1.h"
#include "sum1_file_1.h"
#include "sum2_file_1.h"

#define TMPL_COLS 159
#define TMPL_ROWS 143
#define IMG_COLS 951
//#define TMPL_COLS 3
//#define TMPL_ROWS 2
//#define IMG_COLS 5


using namespace std;

int tmpl_cols, tmpl_rows, img_cols;
int *stripe_resp;
int *stripe_sum1;
int stripe_sum2;
int ready;
		
void write_ip (const int tmpl_cols, const int tmpl_rows, const int img_cols, const int flag_init);
void write_bram(const int *data, int img_tmpl);
void read_ip();
void read_bram();

int main ()
{	
	tmpl_cols = TMPL_COLS;
	tmpl_rows = TMPL_ROWS;
	img_cols = IMG_COLS;
	
	stripe_resp = new int[img_cols - tmpl_cols];
	stripe_sum1 = new int[img_cols - tmpl_cols];
	
	//Template matching operation starts
	printf ("[APP] Cross correlation is about to start");

	//Initialize registers for dimensions (flag_init = 1)
        write_ip(tmpl_cols, tmpl_rows, img_cols, 1); 
	printf ("[APP] Initialisation done by software");
	

	//Write image in BRAM
	write_bram(image_stripe, 1);

	//Write template in BRAM
	write_bram(tmpl, 0);
	
	//Start ip and wait until ready becomes 0 (flag_init = 0)
    write_ip(tmpl_cols, tmpl_rows, img_cols, 0); 
	
	//Waiting for ready and reading sum2
	while (ready == 0)
		read_ip();
	
	printf ("[APP] Whole cross correlation done");
	
	//----------------------------Check results-------------------------------------
	read_bram();
    
	if (stripe_sum2 != sum2)
			printf("[APP] Sum2 is not correct! Expected %d, observed %d\n", sum2, stripe_sum2);
		else
			printf("[APP] Sum2 correct!\n");
   
	for (int i = 0; i < img_cols - tmpl_cols; i++)
	{
		if (stripe_resp[i] != resp[i])
			printf("[APP] Resp on location %d is not correct! Expected %d, observed %d\n", i, resp[i], stripe_resp[i]);
		else
			printf("[APP] Resp correct on location %d\n", i);
	}	
	
	for (int i = 0; i < img_cols - tmpl_cols; i++)
	{
		if (stripe_sum1[i] != sum1[i])
			printf("[APP] Sum1 on location %d is not correct! Expected %d, observed %d\n", i, sum1[i], stripe_sum1[i]);
		else
			printf("[APP] Sum1 correct on location %d\n", i);
	}	
	
	printf ("[APP] Checking done");
	
	delete[] stripe_resp;
	delete[] stripe_sum1;
			
	return EXIT_SUCCESS;
}

void write_ip (const int tmpl_cols, const int tmpl_rows, const int img_cols, const int flag_init)
{
	FILE *cross_correlation;
	cross_correlation = fopen ("/dev/xlnx,cross_correlation", "w");
	fprintf (cross_correlation, "%d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);
	printf ("[APP] %d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);
	fclose (cross_correlation);
}

void write_bram(const int *data, int img_tmpl)
{
	FILE *bram;
	
	//img_tmpl, 1 for image 0 for template
	
	//Write template
	if (img_tmpl == 0)
	{
		for (int j = 0; j < tmpl_rows; j++)
		{
			for (int k = 0; k < tmpl_cols; k++)
			{
				bram = fopen ("/dev/xlnx,bram_tmpl", "w");
		    	fprintf(bram, "%d, %d\n", j * tmpl_cols + k, data[j * tmpl_cols + k]);
		    	fclose (bram);
			}
		}
	}
	//Write image stripe
	else if (img_tmpl == 1)
	{
		for (int j = 0; j < tmpl_rows; j++)
		{
			for (int k = 0; k < img_cols; k++)
			{
				bram = fopen ("/dev/xlnx,bram_img", "w");
		    	fprintf(bram, "%d, %d\n", j * img_cols + k, data[j * img_cols + k]);
		    	fclose (bram);
			}
		}
	}
	else
	{
		printf("[APP] Wrong number for bram\n");
	}

}

void read_ip()
{
	FILE *cross_correlation;
	cross_correlation = fopen ("/dev/xlnx,cross_correlation", "r");
	fscanf (cross_correlation, "ready = %d, sum2 = %d\n", &ready, &stripe_sum2);
	if (ready == 1)
		printf ("[APP] ready = %d, sum2 = %d\n", ready, stripe_sum2);
	fclose (cross_correlation);
}

void read_bram()
{
	FILE *bram;
	int n;
	int sign;
	
	bram = fopen ("/dev/xlnx,bram_resp", "r");
	for (int j = 0; j < img_cols - tmpl_cols; j++)
	{
		fscanf (bram, "%d ", &n);
		stripe_resp[j] = n;

		//printf ("[APP] resp[%d] = %d\n", j, stripe_resp[j]);
	}
	fscanf (bram, "\n");
    	fclose (bram);


	bram = fopen ("/dev/xlnx,bram_sum1", "r");
	for (int j = 0; j < img_cols - tmpl_cols; j++)
	{
		fscanf (bram, "%d ", &n);
		stripe_sum1[j] = n;

		//printf ("[APP] sum1[%d] = %d\n", j, stripe_sum1[j]);
	}	
	fscanf (bram, "\n");
    	fclose (bram);
}
