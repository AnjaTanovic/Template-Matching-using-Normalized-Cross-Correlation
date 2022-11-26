#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <time.h>

using namespace std;

int dimensions[4];
int tmpl_cols, tmpl_rows, img_cols, img_rows;

int *stripe_resp;
int *stripe_sum1;
int stripe_sum2;

int ready;

FILE *dim_file, *img0_file, *img1_file, *img2_file, *tmpl0_file, *tmpl1_file, *tmpl2_file, *res_file; 

void choose_files();		
void write_ip (const int tmpl_cols, const int tmpl_rows, const int img_cols, const int flag_init);
void write_bram(const int *data, int img_tmpl);
void read_ip();
void read_bram();

int main ()
{	
	//Choose images and their .txt files 
	choose_files();
	
	//Start of time measurement
	time_t start, end;	
	double dif;
	time (&start);
	
	//Store dimensions
	int k = 0;
	while (fscanf(dim_file, "%d", &dimensions[k]) == 1)  
	{ 
		if (k > 4)
		cout << "Dimensions file is not correct" << endl;

		k++;
	} 
	fclose(dim_file);
	
	img_rows = dimensions[0];
	img_cols = dimensions[1];
	tmpl_rows = dimensions[2];
	tmpl_cols = dimensions[3];
	
	//Store image
	int *img_b = new int[img_rows * img_cols];
	int *img_g = new int[img_rows * img_cols];
	int *img_r = new int[img_rows * img_cols];
	
	k = 0;
	while (fscanf(img0_file, "%d", &img_b[k]) == 1) 
	{
		k++;
	}
	fclose(img0_file);
	
	k = 0;
	while (fscanf(img1_file, "%d", &img_g[k]) == 1) 
	{
		k++;
	}
	fclose(img1_file);
	
	k = 0;
	while (fscanf(img2_file, "%d", &img_r[k]) == 1) 
	{
		k++;
	}
	fclose(img2_file);
	
	//Store template
	int *tmpl_b = new int[tmpl_rows * tmpl_cols];
	int *tmpl_g = new int[tmpl_rows * tmpl_cols];
	int *tmpl_r = new int[tmpl_rows * tmpl_cols];
	
	k = 0;	
	while (fscanf(tmpl0_file, "%d", &tmpl_b[k]) == 1) 
	{
		k++;
	}
	fclose(tmpl0_file);
	
	k = 0;	
	while (fscanf(tmpl1_file, "%d", &tmpl_g[k]) == 1) 
	{
		k++;
	}
	fclose(tmpl1_file);
	
	k = 0;
	while (fscanf(tmpl2_file, "%d", &tmpl_r[k]) == 1) 
	{
		k++;
	}
	fclose(tmpl2_file);
	
	//Template matching operation starts
	cout << "Cross correlation is about to start" << endl;

	int stripe = 0;
    int ch = 0; 
    
    double sum = 0;
    int sqrt_o = 0;
	
	stripe_resp = new int[img_cols - tmpl_cols];
	stripe_sum1 = new int[img_cols - tmpl_cols];
	
	double *res = new double[(img_rows - tmpl_rows)*(img_cols - tmpl_cols)];
	
	//flags
    bool done = 0;
    bool new_ch = 1;
    ready = 1;
    bool need_start = 0; //if this flag is 1 it means that we need to send start bit to ip_hard 
    bool calc_stripe = 0; //for sqrt and division
	
	//Initialize registers for dimensions (flag_init = 1)
    write_ip(tmpl_cols, tmpl_rows, img_cols, 1); 
    
	cout << "Initialisation done by software" << endl;
	
    while (!done)
    {
    
		if(new_ch)
		{
			//tmpl_bram defines template in bram.
			int *tmpl_bram = new int[tmpl_rows * tmpl_cols];
			
			for (int j = 0; j < tmpl_rows; j++)
			{
				for (int k = 0; k < tmpl_cols; k++)
				{
					if (ch == 0)
						tmpl_bram[j * tmpl_cols + k] = tmpl_b[j * tmpl_cols + k];
					else if (ch == 1)
						tmpl_bram[j * tmpl_cols + k] = tmpl_g[j * tmpl_cols + k];
					else 
						tmpl_bram[j * tmpl_cols + k] = tmpl_r[j * tmpl_cols + k];
				}
			}

			//Write template in bram (one color)
			write_bram(tmpl_bram, 0);

			delete[] tmpl_bram;
			
			new_ch = 0;
		}
			
		if (ready)
		{
			// stripe_b is used to change Mat to char*, defines stripe of image in bram
			int *stripe_b = new int[tmpl_rows * img_cols];
		   				
			for (int j = 0; j < tmpl_rows; j++)
			{
				for (int k = 0; k < img_cols; k++)
				{
					if (ch == 0)
						stripe_b[j * img_cols + k]= img_b[(j + stripe) * img_cols + k];
					else if (ch == 1)
						stripe_b[j * img_cols + k]= img_g[(j + stripe) * img_cols + k];
					else if (ch == 2)
						stripe_b[j * img_cols + k]= img_r[(j + stripe) * img_cols + k];
				}
			}
			
			//Write stripe of image in bram 
			write_bram(stripe_b, 1);
			
			delete[] stripe_b;
			
			need_start = 1;
	   	}

		//Ip_hard starts cross correlation (start will reset ready register in ip_hard)
		if (need_start)
		{
		    //Start ip and wait for ready (flag_init = 0)
		ready = 0;
    		write_ip(tmpl_cols, tmpl_rows, img_cols, 0); 
			need_start = 0;
		}

		if (calc_stripe)
		{
			
			for (int j = 0; j < img_cols - tmpl_cols; j++)
			{
				sum = (double)stripe_sum1[j] * (double)stripe_sum2;
				sqrt_o = (int)sqrt(sum);
				
				if ((stripe - 1) >= 0)
					res[(stripe - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;	
				else
					res[(img_rows - tmpl_rows - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;
				
			}
		}

		if (ch == 3)
		{
			done = 1; //exit loop
		}
		
		//Checking if ip_hard needs new stripe (ready) and read stripe_sum2
		read_ip();

		if(ready)
		{	
			
			read_bram();
			
			calc_stripe = 1;  //flag for sqrt and div calculation for next while iteration

			stripe++;
			
			cout << "STRIPE NUMBER :  " << stripe << endl;
			
			if (stripe >= img_rows - tmpl_rows)
			{
				stripe = 0; 
				ch++;
				new_ch = 1;
				
				if (ch == 3)
				{
					//all if conditions have to be disabled (except calc_stripe and ch == 3)
					new_ch = 0;
					ready = 0;
					need_start = 0;
				}
			} 
				
		}	
	}
	
	cout<<"Whole cross correlation done" << endl;
	
	delete[] stripe_resp;
	delete[] stripe_sum1;

	delete[] img_b;
	delete[] img_g;
	delete[] img_r;
	delete[] tmpl_b;
	delete[] tmpl_g;
	delete[] tmpl_r;
	
	//Searching for min and max (for normalization)
	
	double max = res[0];
	double min = res[0];
	
	for (int i = 0; i < img_rows - tmpl_rows; i++)
	{
		for (int j = 0; j < img_cols - tmpl_cols; j++)
		{
			if (max < res[i*(img_cols-tmpl_cols) + j])
				max = res[i*(img_cols-tmpl_cols) + j];	
			if (min > res[i*(img_cols-tmpl_cols) + j])
				min = res[i*(img_cols-tmpl_cols) + j];	
		}
	}

	//Normalization
	
	for (int i = 0; i < img_rows - tmpl_rows; i++)
	{
		for (int j = 0; j < img_cols - tmpl_cols; j++)
		{
			res[i*(img_cols - tmpl_cols) + j]  = ((res[i*(img_cols-tmpl_cols) + j] - min) * 255) / (max - min);
		}
	}
	
	//Storing result in file
	char result_name[35]= "postprocessing/result.txt";
		
	res_file = fopen(result_name, "w");
	if (!res_file)
	{  
		cout << "File can't be created" << endl; 
		exit(-1); 
	}
	
	for (int j = 0; j < img_rows - tmpl_rows; j++)
	{
		for (int k = 0; k < img_cols - tmpl_cols; k++)
		{
			fprintf(res_file, "%d\n", (int)res[j*(img_cols - tmpl_cols) + k]);
		}
	}
	fclose(res_file);	

	delete[] res;

	//End of measurement
	time (&end);
	dif = difftime(end, start);
	printf("The application took %.2lf seconds to run\n", dif);
				
	return EXIT_SUCCESS;
}

void choose_files()
{
	char *option;

	string dim_name;
	string img_name0;
	string img_name1;
	string img_name2;
	string tmpl_name0;
	string tmpl_name1;
	string tmpl_name2;
	
	string dim_name_s;
	string img_name0_s;
	string img_name1_s;
	string img_name2_s;
	string tmpl_name0_s;
	string tmpl_name1_s;
	string tmpl_name2_s;
	
	const char *dim_name_c;
	const char *img_name0_c;
	const char *img_name1_c;
	const char *img_name2_c;
	const char *tmpl_name0_c;
	const char *tmpl_name1_c;
	const char *tmpl_name2_c;
	
	string dim_loc = "preprocessing/";
	string img_loc0 = "preprocessing/";
	string img_loc1 = "preprocessing/";
	string img_loc2 = "preprocessing/";
	string tmpl_loc0 = "preprocessing/";
	string tmpl_loc1 = "preprocessing/";
	string tmpl_loc2 = "preprocessing/";
	
	printf("Choose the number of picture:\n");
	printf("1.bubbles\n");
	printf("2.pumpkins\n");
	printf("3.green\n");
	printf("4.diamonds\n");
	printf("5.ball\n");
	printf("6.logo\n");
	printf("7.stars\n");
	printf("8.sun\n");
	printf("9.Choose your own images\n");
    printf("If you want to upload your own images, choose option 8.\n");
	//baby yoda,image name 8
	//chocolate bubbles,image name9

	size_t num_b = 2;
	option = (char*) malloc(num_b+1);
	getline(&option, &num_b, stdin);
	
	switch (*option)
	{
		case '1':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_1.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_1_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_1_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_1_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_1_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_1_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_1_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;	
		case '2':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_3.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_3_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_3_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_3_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_3_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_3_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_3_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '3':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_0.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_0_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_0_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_0_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_0_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_0_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_0_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '4':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_2.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_2_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_2_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_2_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_2_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_2_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_2_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '5':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_4.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_4_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_4_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_4_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_4_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_4_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_4_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '6':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_5.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_5_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_5_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_5_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_5_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_5_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_5_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '7':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_6.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_6_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_6_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_6_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_6_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_6_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_6_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '8':
			//reading dimensions	
			dim_file = fopen("preprocessing/dimensions_7.txt", "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen("preprocessing/image_7_0.txt", "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen("preprocessing/image_7_1.txt", "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen("preprocessing/image_7_2.txt", "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen("preprocessing/template_7_0.txt", "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen("preprocessing/template_7_1.txt", "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen("preprocessing/template_7_2.txt", "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		case '9':
			printf("Before entering file names, make sure the image files are stored in the preprocessing directory.\n");
			printf("Required files are .txt files with:\n");
			printf("	dimensions (img rows, img cols, tmpl rows tmpl cols\n");
			printf("	image channel blue\n");
			printf("	image channel green\n");
			printf("	image channel red\n");
			printf("	template channel blue\n");
			printf("	template channel green\n");
			printf("	template channel red\n\n");
			
			printf("Enter file name for dimensions:\n");
			cin>>dim_name;	
	//	num_b = 30;
		//	dim_name = (char*) malloc(num_b+1);
		//	getline(&dim_name, &num_b, stdin);
			
			printf("Enter file name for image, blue channel:\n");
			//num_b = 30;
			//img_name0 = (char*) malloc(num_b+1);
			//getline(&img_name0, &num_b, stdin);
			cin>>img_name0;

			printf("Enter file name for image, green channel:\n");
			//num_b = 30;
			//img_name1 = (char*) malloc(num_b+1);
			//getline(&img_name1, &num_b, stdin);
			cin>>img_name1;
			
			printf("Enter file name for image, red channel:\n");
		//	num_b = 30;
		//	img_name2 = (char*) malloc(num_b+1);
		//	getline(&img_name2, &num_b, stdin);
		cin>>img_name2;
	
			printf("Enter file name for template, blue channel:\n");
		//	num_b = 30;
		//	tmpl_name0 = (char*) malloc(num_b+1);
		//	getline(&tmpl_name0, &num_b, stdin);
		cin>>tmpl_name0;
	
			printf("Enter file name for template, green channel:\n");
			//num_b = 30;
			//tmpl_name1 = (char*) malloc(num_b+1);
		//	getline(&tmpl_name1, &num_b, stdin);
		cin>>tmpl_name1;
	
			printf("Enter file name for template, red channel:\n");
		//	num_b = 30;
		//	tmpl_name2 = (char*) malloc(num_b+1);
		//	getline(&tmpl_name2, &num_b, stdin);
		cin>>tmpl_name2;

			dim_name_s = dim_loc + dim_name;
			img_name0_s = img_loc0 + img_name0;
			img_name1_s = img_loc1 + img_name1;
			img_name2_s = img_loc2 + img_name2;
			tmpl_name0_s = tmpl_loc0 + tmpl_name0;
			tmpl_name1_s = tmpl_loc1 + tmpl_name1;
			tmpl_name2_s = tmpl_loc2 + tmpl_name2;

		//	strcat(dim_loc, dim_name_c);
		//	strcat(img_loc0, img_name0_c);
		//	strcat(img_loc1, img_name1_c);
		//	strcat(img_loc2, img_name2_c);
		//	strcat(tmpl_loc0, tmpl_name0_c);
		//	strcat(tmpl_loc1, tmpl_name1_c);
		//	strcat(tmpl_loc2, tmpl_name2_c);
			
			dim_name_c = dim_name_s.c_str();
			img_name0_c = img_name0_s.c_str();
			img_name1_c = img_name1_s.c_str();
			img_name2_c = img_name2_s.c_str();
			tmpl_name0_c = tmpl_name0_s.c_str();
			tmpl_name1_c = tmpl_name1_s.c_str();
			tmpl_name2_c = tmpl_name2_s.c_str();
			
			//reading dimensions	
			dim_file = fopen(dim_name_c, "r"); 
			if (!dim_file)
            {  
				cout << "File dimensions can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 0
			img0_file = fopen(img_name0_c, "r");
			if (!img0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			img1_file = fopen(img_name1_c, "r");
			if (!img1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 2
			img2_file = fopen(img_name2_c, "r");
			if (!img2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 0
			tmpl0_file = fopen(tmpl_name0_c, "r");
			if (!tmpl0_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading image, channel 1
			tmpl1_file = fopen(tmpl_name1_c, "r");
			if (!tmpl1_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			//reading template, channel 2
			tmpl2_file = fopen(tmpl_name2_c, "r");
			if (!tmpl2_file)
            {  
				cout << "File can't be read" << endl; 
				exit(-1); 
            }
			break;
		default:
			printf("The choosen image does not exist\n");
			break;
	}
}

void write_ip (const int tmpl_cols, const int tmpl_rows, const int img_cols, const int flag_init)
{
	FILE *cross_correlation;
	cross_correlation = fopen ("/dev/xlnx,cross_correlation", "w");
	fprintf (cross_correlation, "%d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);
	//printf ("[APP] %d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);
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
		printf("Wrong number for bram\n");
	}

}

void read_ip()
{
	FILE *cross_correlation;
	cross_correlation = fopen ("/dev/xlnx,cross_correlation", "r");
	fscanf (cross_correlation, "ready = %d, sum2 = %d\n", &ready, &stripe_sum2);
	//if (ready == 1)
		//printf ("[APP] ready = %d, sum2 = %d\n", ready, stripe_sum2);
	fclose (cross_correlation);
}

void read_bram()
{
	FILE *bram;
	int n;
	
	bram = fopen ("/dev/xlnx,bram_resp", "r");
	for (int j = 0; j < img_cols - tmpl_cols; j++)
	{
		fscanf (bram, "%d ", &n);
		stripe_resp[j] = n;
   
	}
	fscanf(bram, "\n");
 	fclose (bram);

	bram = fopen ("/dev/xlnx,bram_sum1", "r");		
	for (int j = 0; j < img_cols - tmpl_cols; j++)
	{
		fscanf (bram, "%d", &n);
		stripe_sum1[j] = n;
    	}
	fscanf(bram, "\n");
	fclose (bram);
	
	//cout<<("STRIPE DONE !!!!!!!!!! ") << endl;
}
